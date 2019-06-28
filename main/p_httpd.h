#pragma once
#include <esp_http_server.h>


const char* httpd_media_type(const char *path);
esp_err_t httpd_send_file(httpd_req_t *req, const char *path);
esp_err_t httpd_on(httpd_handle_t handle,
  const char *uri, httpd_method_t method,
  esp_err_t (*handler)(httpd_req_t *r)
);
esp_err_t httpd_on_static(httpd_req_t *req);
esp_err_t httpd_init(httpd_handle_t *handle);
