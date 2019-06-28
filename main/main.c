#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_spiffs.h>
#include "p_device.h"
#include "p_sht21.h"
#include "p_wifi.h"
#include "p_httpd.h"
#include "macros.h"


static i2c_port_t i2c = I2C_NUM_0;
static httpd_handle_t httpd = NULL;


static void on_wifi(void *arg, esp_event_base_t base, uint32_t id, void *data) {
  if (id == WIFI_EVENT_AP_START) {
    ERETV( httpd_init(&httpd) );
    ERETV( httpd_on(httpd, "/*", HTTP_GET, httpd_on_static) );
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
  ERETV( wifi_start_ap() );
  char buff[256];
  ERETV( sht21_json(i2c, buff) );
  printf(buff);
}
