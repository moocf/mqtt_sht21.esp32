#include <esp_wifi.h>
#include <esp_event.h>
#include "macros.h"





esp_err_t wifi_init() {
  return ESP_OK;
}


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


esp_err_t wifi_ap() {
  printf("- Set WiFi mode as AP\n");
  ERET( esp_wifi_set_mode(WIFI_MODE_AP) );
  wifi_config_t wifi_config = {.ap = {
    .ssid = "charmender",
    .password = "charmender",
    .ssid_len = 0,
    .channel = 0,
    .authmode = WIFI_AUTH_WPA_PSK,
    .ssid_hidden = 0,
    .max_connection = 4,
    .beacon_interval = 100,
  }};
  ERET( esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config) );
  printf("- Start WiFi\n");
  ERET( esp_wifi_start() );
  return ESP_OK;
}
