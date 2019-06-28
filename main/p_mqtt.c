#include <nvs_flash.h>
#include <mqtt_client.h>
#include "p_json.h"
#include "p_mqtt.h"
#include "macros.h"


esp_err_t mqtt_set_config_json(esp_mqtt_client_handle_t handle, const char *json) {
  char uri[256];
  int length;
  json_string(json, "\"uri\":", uri);
  ERET( esp_mqtt_client_set_uri(handle, uri) );
  nvs_handle_t nvs;
  ERET( nvs_open("storage", NVS_READWRITE, &nvs) );
  ERET( nvs_set_blob(nvs, "mqtt_broker_uri", uri, strlen(uri)+1) );
  ERET( nvs_commit(nvs) );
  nvs_close(nvs);
  return ESP_OK;
}


esp_err_t mqtt_init(esp_mqtt_client_handle_t *handle) {
  char uri[256];
  size_t length;
  nvs_handle_t nvs;
  ERET( nvs_open("storage", NVS_READWRITE, &nvs) );
  nvs_get_blob(nvs, "mqtt_broker_uri", uri, &length);
  nvs_close(nvs);
  esp_mqtt_client_config_t c = {
    .uri = uri
  };
  *handle = esp_mqtt_client_init(&c);
  ERET( esp_mqtt_client_start(*handle) );
  return ESP_OK;
}


esp_err_t mqtt_deinit(esp_mqtt_client_handle_t handle) {
  ERET( esp_mqtt_client_stop(handle) );
  ERET( esp_mqtt_client_destroy(handle) );
  return ESP_OK;
}
