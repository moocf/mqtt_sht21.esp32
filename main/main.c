#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_spiffs.h>
#include "macros.h"



static void on_wifi(void* arg, esp_event_base_t base, int32_t id, void* data) {
  if (id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t *d = (wifi_event_ap_staconnected_t*) data;
    printf("Station " MACSTR " joined, AID = %d (event)\n", MAC2STR(d->mac), d->aid);
  } else if (id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t *d = (wifi_event_ap_stadisconnected_t*) data;
    printf("Station " MACSTR " left, AID = %d (event)\n", MAC2STR(d->mac), d->aid);
  } else if(id == WIFI_EVENT_SCAN_DONE) {
    printf("- WiFi scan done (event)\n");
    printf("- Get scanned AP records\n");
    static uint16_t count = 32;
    static wifi_ap_record_t records[32];
    ERETV( esp_wifi_scan_get_ap_records(&count, records) );
    for(int i=0; i<count; i++) {
      printf("%d. %s : %d\n", i+1, records[i].ssid, records[i].rssi);
    }
  }
}


static esp_err_t wifi_ap() {
  printf("- Initialize TCP/IP adapter\n");
  tcpip_adapter_init();
  printf("- Create default event loop\n");
  ERET( esp_event_loop_create_default() );
  printf("- Initialize WiFi with default config\n");
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ERET( esp_wifi_init(&cfg) );
  printf("- Register WiFi event handler\n");
  ERET( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &on_wifi, NULL) );
}


void app_main() {
  printf("- Initialize NVS\n");
  ESP_ERROR_CHECK( nvs_init() );
  ESP_ERROR_CHECK( wifi_ap() );
}
