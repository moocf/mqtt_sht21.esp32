#pragma once
#include <esp_http_server.h>


const char* httpd_media_type(const char *path);
esp_err_t httpd_send_file(httpd_req_t *req, const char *path);
esp_err_t httpd_init();
