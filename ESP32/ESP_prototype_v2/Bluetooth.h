#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spi_flash.h"
#include <nvs.h>
#include "nvs_flash.h"

#include "esp_log.h"
#include "esp_system.h"
//#include "sdkconfig.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#endif

#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"

#include <driver/i2c.h>

#include "AudioElement.h"

class Bluetooth : AudioElement<uint8_t>{
public:
Bluetooth(Ringbuffer<uint8_t>*);
~Bluetooth();
esp_err_t Bluetooth_a2dp_sink_init(const char* device_name);
static void bt_a2d_sink_data_cb(const uint8_t *data, uint32_t len);
static void bt_a2d_sink_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);
static const char* TAG;
private:
static Bluetooth* this_;
};

#endif
