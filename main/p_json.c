#include <string.h>
#include "p_json.h"


void json_string(const char *json, const char *key, char *buff) {
  char *k = strstr(json, key);
  char *start = strchr(k+7, '\"')+1;
  char *end = strchr(start, '\"');
  memcpy(buff, start, end-start);
  buff[end-start] = '\0';
}
