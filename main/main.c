#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_spiffs.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "p_device.h"
#include "p_sht21.h"
#include "p_wifi.h"
#include "p_httpd.h"
#include "p_mqtt.h"
#include "macros.h"


static i2c_port_t i2c = I2C_NUM_0;
static httpd_handle_t httpd = NULL;
static esp_mqtt_client_handle_t mqtt = NULL;
static bool mqtt_connected = false;


static esp_err_t deinit() {
  ERET( mqtt_deinit(mqtt) );
  ERET( httpd_deinit(httpd) );
  ERET( esp_wifi_deinit() );
  ERET( spiffs_deinit() );
  ERET( nvs_deinit() );
  ERET( i2c_deinit(i2c) );
  return ESP_OK;
}


static esp_err_t on_sht21(httpd_req_t *req) {
  char json[256];
  ERET( sht21_json(i2c, json) );
  printf("- On SHT21: json=%s\n", json);
  ERET( httpd_resp_send_json(req, json) );
  return ESP_OK;
}


static esp_err_t on_wifi_config_sta(httpd_req_t *req) {
  char json[256];
  ERET( wifi_config_sta_json(json) );
  printf("- On WiFi get config station: json=%s\n", json);
  ERET( httpd_resp_send_json(req, json) );
  return ESP_OK;
}


static esp_err_t on_wifi_set_config_sta(httpd_req_t *req) {
  char json[256];
  httpd_req_recv(req, json, req->content_len);
  printf("- On WiFi set config station: json=%s\n", json);
  ERET( wifi_set_config_sta_json(json) );
  ERET( httpd_resp_send_json(req, json) );
  return ESP_OK;
}


static esp_err_t on_mqtt_config(httpd_req_t *req) {
  char json[256];
  ERET( mqtt_config_json(json) );
  printf("- On MQTT get config: json=%s\n", json);
  ERET( httpd_resp_send_json(req, json) );
  return ESP_OK;
}

static esp_err_t on_mqtt_set_config(httpd_req_t *req) {
  char json[256];
  httpd_req_recv(req, json, req->content_len);
  printf("- On MQTT set config: json=%s\n", json);
  ERET( mqtt_set_config_json(mqtt, json) );
  ERET( httpd_resp_send_json(req, json) );
  return ESP_OK;
}


static esp_err_t on_restart(httpd_req_t *req) {
  printf("- On Restart\n");
  esp_restart();
  return ESP_OK;
}


void on_mqtt(void *arg, esp_event_base_t base, int32_t id, void *data) {
  esp_mqtt_event_handle_t d = (esp_mqtt_event_handle_t) data;
  esp_mqtt_client_handle_t h = d->client;
  switch(d->event_id) {
    case MQTT_EVENT_CONNECTED:
    printf("- Connected to MQTT broker\n");
    mqtt_connected = true;
    break;
    case MQTT_EVENT_DISCONNECTED:
    printf("- Disconnected from MQTT broker\n");
    mqtt_connected = false;
    break;
    default:
    break;
  }
}


static void on_wifi(void *arg, esp_event_base_t base, int32_t id, void *data) {
  if (id == WIFI_EVENT_AP_START) {
    ERETV( httpd_init(&httpd) );
    ERETV( httpd_on(httpd, "/sht21", HTTP_GET, &on_sht21) );
    ERETV( httpd_on(httpd, "/wifi_config_sta", HTTP_GET,  &on_wifi_config_sta) );
    ERETV( httpd_on(httpd, "/wifi_config_sta", HTTP_POST,  &on_wifi_set_config_sta) );
    ERETV( httpd_on(httpd, "/mqtt_config", HTTP_GET, &on_mqtt_config) );
    ERETV( httpd_on(httpd, "/mqtt_config", HTTP_POST, &on_mqtt_set_config) );
    ERETV( httpd_on(httpd, "/restart", HTTP_POST,  &on_restart) );
    ERETV( httpd_on(httpd, "/*", HTTP_GET, &httpd_on_static) );
    ERETV( mqtt_init(&mqtt) );
    ERETV( esp_mqtt_client_register_event(mqtt, ESP_EVENT_ANY_ID, on_mqtt, mqtt) );
    ERETV( esp_mqtt_client_start(mqtt) );
    printf("all init done\n");
  }
  else {
    wifi_on_sta(arg, base, id, data);
  }
}


static void on_ip(void *arg, esp_event_base_t base, int32_t id, void *data) {
  if (id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *d = (ip_event_got_ip_t*) data;
    printf("- On Got IP: IP=%s\n", ip4addr_ntoa(&d->ip_info.ip));
  }
}


void app_main() {
  tcpip_adapter_init();
  ERETV( esp_event_loop_create_default() );
  ERETV( i2c_init(i2c, GPIO_NUM_18, GPIO_NUM_19, 100000) );
  ERETV( nvs_init() );
  ERETV( spiffs_init() );
  ERETV( wifi_init() );
  esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &on_wifi, NULL);
  esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_ip, NULL);
  ERETV( wifi_start_ap() );
  while (true) {
    vTaskDelay(1000 / portTICK_RATE_MS);
    if (!mqtt_connected) continue;
    char json[256];
    ERET( sht21_json(i2c, json) );
    printf("- Send to MQTT broker\n");
    printf(": topic=%s, json=%s\n", "/charmender", json);
    esp_mqtt_client_publish(mqtt, "/charmender", json, strlen(json)+1, 0, 0);
  }
}
