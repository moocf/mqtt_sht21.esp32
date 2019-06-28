#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_spiffs.h>
#include "p_device.h"
#include "p_sht21.h"
#include "p_wifi.h"
#include "p_httpd.h"
#include "macros.h"



static void on_wifi(void *arg, esp_event_base_t base, int32_t id, void *data) {
  if (id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t *d = (wifi_event_ap_staconnected_t*) data;
    printf("Station " MACSTR " joined, AID = %d (event)\n", MAC2STR(d->mac), d->aid);
  } else if (id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t *d = (wifi_event_ap_stadisconnected_t*) data;
    printf("Station " MACSTR " left, AID = %d (event)\n", MAC2STR(d->mac), d->aid);
  }
}

static void on_ip(void *arg, esp_event_base_t base, int32_t id, void *data) {
  if (id == IP_EVENT_STA_GOT_IP) {
  } else if (id == IP_EVENT_STA_LOST_IP) {
  }
}


static esp_err_t wifi_ap() {
  tcpip_adapter_init();
  ERET( esp_event_loop_create_default() );
  printf("- Initialize WiFi\n");
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ERET( esp_wifi_init(&cfg) );
  ERET( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &on_wifi, NULL) );
}


void app_main() {
  tcpip_adapter_init();
  ERETV( esp_event_loop_create_default() );
  ERETV( i2c_init(I2C_NUM_0, GPIO_NUM_18, GPIO_NUM_19, 100000) );
  ERETV( nvs_init() );
  ERETV( spiffs_init() );
}
