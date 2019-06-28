#include <esp_log.h>
#include <esp_spiffs.h>
#include <esp_http_server.h>
#include "macros.h"
#include "p_httpd.h"


#define FILE_BUFFER_SIZE  1024
static const char *TAG = "httpd";


static esp_err_t spiffs_init() {
  printf("- Mount SPIFFS as VFS\n");
  esp_vfs_spiffs_conf_t config = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = false,
  };
  ERET( esp_vfs_spiffs_register(&config) );
  size_t total, used;
  ERET( esp_spiffs_info(NULL, &total, &used) );
  printf("Total = %d, Used = %d\n", total, used);
  return ESP_OK;
}


static const char* httpd_media_type(const char *path) {
  const char *ext = strrchr(path, '.');
  if (strcmp(ext, ".html") == 0) return "text/html";
  if (strcmp(ext, ".css") == 0) return "text/css";
  if (strcmp(ext, ".js") == 0) return "text/javascript";
  if (strcmp(ext, ".png") == 0) return "image/png";
  if (strcmp(ext, ".jpg") == 0) return "image/jpeg";
  return "text/plain";
}


static esp_err_t httpd_send_file(httpd_req_t *req, const char *path) {
  const char *type = httpd_media_type(path);
  ERET( httpd_resp_set_type(req, type) );
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "Cannot open file %s", path);
    return ESP_FAIL;
  }
  char buff[FILE_BUFFER_SIZE];
  size_t read;
  do {
    read = fread(buff, 1, sizeof(buff), f);
    ERET( httpd_resp_send_chunk(req, buff, read) );
  } while(read == sizeof(buff));
  ERET( httpd_resp_send_chunk(req, NULL, 0) );
  return ESP_OK;
}
