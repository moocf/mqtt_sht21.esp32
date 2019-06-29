#include <string.h>
#include <stdio.h>
#include "p_json.h"


void json_string(const char *json, const char *key, char *buff) {
  printf("json=%s, key=%s\n", json, key);
  char *k = strstr(json, key);
  printf("k=%s\n", k);
  char *start = strchr(k+strlen(key), '\"')+1;
  printf("start=%s\n", start);
  char *end = strchr(start, '\"');
  printf("end=%s\n", end);
  memcpy(buff, start, end-start);
  buff[end-start] = '\0';
}
