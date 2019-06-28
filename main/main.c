#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_spiffs.h>
#include "p_device.h"
#include "p_sht21.h"
#include "p_wifi.h"
#include "p_httpd.h"
#include "macros.h"


static const char *TYPE_JSON = "application/json";
static i2c_port_t i2c = I2C_NUM_0;
static httpd_handle_t httpd = NULL;


static esp_err_t deinit() {
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
  ERET( httpd_resp_set_type(req, TYPE_JSON) );
  ERET( httpd_resp_sendstr(req, json) );
  return ESP_OK;
}


static esp_err_t on_wifi_config_sta(httpd_req_t *req) {
  char json[256];
  ERET( wifi_config_sta_json(json) );
  printf("- On WiFi get config station: json=%s\n", json);
  ERET( httpd_resp_set_type(req, TYPE_JSON) );
  ERET( httpd_resp_sendstr(req, json) );
  return ESP_OK;
}


static esp_err_t on_wifi_set_config_sta(httpd_req_t *req) {
  char json[256];
  httpd_req_recv(req, json, req->content_len);
  printf("- On WiFi set config station: json=%s\n", json);
  ERET( wifi_set_config_sta_json(json) );
  ERET( httpd_resp_set_type(req, TYPE_JSON) );
  ERET( httpd_resp_sendstr(req, "1") );
  return ESP_OK;
}


static esp_err_t on_restart(httpd_req_t *req) {
  printf("- On Restart\n");
  esp_restart();
  return ESP_OK;
}


static void on_wifi(void *arg, esp_event_base_t base, int32_t id, void *data) {
  if (id == WIFI_EVENT_AP_START) {
    ERETV( httpd_init(&httpd) );
    ERETV( httpd_on(httpd, "/sht21", HTTP_GET, &on_sht21) );
    ERETV( httpd_on(httpd, "/wifi_config_sta", HTTP_GET,  &on_wifi_config_sta) );
    ERETV( httpd_on(httpd, "/wifi_config_sta", HTTP_POST,  &on_wifi_set_config_sta) );
    ERETV( httpd_on(httpd, "/restart", HTTP_POST,  &on_restart) );
    ERETV( httpd_on(httpd, "/*", HTTP_GET, &httpd_on_static) );
  }
  else {
    wifi_on_sta(arg, base, id, data);
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
  ERETV( wifi_start_ap() );
}
