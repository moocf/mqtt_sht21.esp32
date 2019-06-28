#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include "macros.h"



static esp_err_t nvs_init() {
  printf("- Initialize NVS\n");
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ERET( nvs_flash_erase() );
    ERET( nvs_flash_init() );
  }
  ERET( ret );
  return ESP_OK;
}


static esp_err_t wifi_ap() {
  printf("- Set WiFi mode as AP\n");
  ERET( esp_wifi_set_mode(WIFI_MODE_AP) );
  printf("- Set WiFi configuration\n");
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
