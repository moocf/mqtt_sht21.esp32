#include <nvs_flash.h>
#include <mqtt_client.h>
#include "p_device.h"
#include "p_json.h"
#include "p_mqtt.h"
#include "macros.h"


#define MQTT_URI_KEY      "mqtt_uri"
#define MQTT_URI_DEFAULT  "mqtt://10.2.28.74:1883"
static char uri[128];


esp_err_t mqtt_config_json(char *buff) {
  sprintf(buff, "{\"uri\": \"%s\"}", uri);
  return ESP_OK;
}


esp_err_t mqtt_set_config_json(esp_mqtt_client_handle_t handle, const char *json) {
  json_string(json, "\"uri\":", uri);
  ERET( esp_mqtt_client_set_uri(handle, uri) );
  NVS_WRITE(nvs, nvs_set_str(nvs, MQTT_URI_KEY, uri));
  return ESP_OK;
}


esp_err_t mqtt_init(esp_mqtt_client_handle_t *handle) {
  size_t length = sizeof(uri);
  NVS_READ(nvs, nvs_get_str(nvs, MQTT_URI_KEY, uri, &length));
  if (strstr(uri, "mqtt://") != uri) strcpy(uri, MQTT_URI_DEFAULT);
  printf("- Init MQTT client: usr=%s\n", uri);
  esp_mqtt_client_config_t c = {
    .uri = uri
  };
  *handle = esp_mqtt_client_init(&c);
  if (*handle == NULL) {
    printf("MQTT init failed!\n");
    return ESP_FAIL;
  }
  return ESP_OK;
}


esp_err_t mqtt_deinit(esp_mqtt_client_handle_t handle) {
  ERET( esp_mqtt_client_stop(handle) );
  ERET( esp_mqtt_client_destroy(handle) );
  return ESP_OK;
}
