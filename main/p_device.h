#pragma once
#include <driver/gpio.h>
#include <driver/i2c.h>


esp_err_t i2c_init(i2c_port_t port, gpio_num_t sda, gpio_num_t scl, uint32_t clk_speed);
esp_err_t nvs_init();
esp_err_t spiffs_init();
