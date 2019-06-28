#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <esp_spiffs.h>
#include "p_device.h"
#include "p_sht21.h"
#include "p_wifi.h"
#include "p_httpd.h"
#include "macros.h"


void app_main() {
  tcpip_adapter_init();
  ERETV( esp_event_loop_create_default() );
  ERETV( i2c_init(I2C_NUM_0, GPIO_NUM_18, GPIO_NUM_19, 100000) );
  ERETV( nvs_init() );
  ERETV( spiffs_init() );
  ERETV( wifi_init() );
}
