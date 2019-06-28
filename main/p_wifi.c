#include <esp_wifi.h>
#include <esp_event.h>
#include "p_httpd.h"
#include "macros.h"


esp_err_t wifi_config_ap(char *buff) {
  wifi_config_t c;
  ERET( esp_wifi_get_config(WIFI_IF_AP, &c) );
  sprintf(buff,
    "{\"ssid\": \"%s\", \"password\": \"%s\", \"channel\": %d, \"authmode\": %d}",
    c.ap.ssid, c.ap.password, c.ap.channel, c.ap.authmode);
  return ESP_OK;
}


esp_err_t wifi_config_sta(char *buff) {
  wifi_config_t c;
  ERET( esp_wifi_get_config(WIFI_IF_STA, &c) );
  sprintf(buff,
    "{\"ssid\": \"%s\", \"password\": \"%s\"}",
    c.sta.ssid, c.sta.password);
  return ESP_OK;
}


static void on_wifi(void *arg, esp_event_base_t base, int32_t id, void *data) {
  if (id == WIFI_EVENT_AP_START) {
    ERETV( httpd_init() );
  }
  else if (id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t *d = (wifi_event_ap_staconnected_t*) data;
    printf("Station " MACSTR " joined, AID = %d (event)\n", MAC2STR(d->mac), d->aid);
  } else if (id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t *d = (wifi_event_ap_stadisconnected_t*) data;
    printf("Station " MACSTR " left, AID = %d (event)\n", MAC2STR(d->mac), d->aid);
  }
}


esp_err_t wifi_init() {
  printf("- Init WiFi as AP\n");
  wifi_init_config_t iconfig = WIFI_INIT_CONFIG_DEFAULT();
  ERET( esp_wifi_init(&iconfig) );
  ERET( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &on_wifi, NULL) );
  ERET( esp_wifi_set_mode(WIFI_MODE_AP) );
  wifi_config_t config = {.ap = {
    .ssid = "charmender",
    .password = "charmender",
    .ssid_len = 0,
    .channel = 0,
    .authmode = WIFI_AUTH_WPA_PSK,
    .ssid_hidden = 0,
    .max_connection = 4,
    .beacon_interval = 100,
  }};
  ERET( esp_wifi_set_config(ESP_IF_WIFI_AP, &config) );
  ERET( esp_wifi_start() );
  return ESP_OK;
}
