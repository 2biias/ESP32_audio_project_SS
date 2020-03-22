#ifndef _ESPCONTROLLER_H_
#define _ESPCONTROLLER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

#include <nvs.h>
#include "nvs_flash.h"

#include <driver/i2s.h>
#include <driver/i2c.h>

#include "es8388_registers.h"

#define I2S_NUM I2S_NUM_0
#define I2S_READLEN 200
//#define I2S_READLEN 50 * 4

#define I2C_NUM I2C_NUM_0
#define ES8388_ADDR 0x20

class ESPcontroller{
public:
  ESPcontroller();
  ~ESPcontroller();
  esp_err_t InitFlash();
  esp_err_t WriteCodecI2C(uint8_t slave_add, uint8_t reg_add, uint8_t data);
  esp_err_t CodecInit();
  esp_err_t I2SInit();
private:
};

#endif
