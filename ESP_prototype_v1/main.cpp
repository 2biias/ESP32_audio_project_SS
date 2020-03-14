#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spi_flash.h"

#include "esp_log.h"
#include "esp_system.h"
#include "sdkconfig.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#endif

#include "Bluetooth.h"
#include "ESPcontroller.h"

extern "C"{

void app_main(){

  ESPcontroller ESPcontroller;
  ESPcontroller.InitFlash();

  //Initializing and starting Bluetooth service by given configations.
  //If no other bluetooth sevice is not started. Configures bt-controller.
  //Enables controller. Init/Enables bluedroid. Initialises controller as
  //A2DP_sink and sets stream type to AUDIO_STREAM_READER.'
  std::cout << "Setting up Bluetooth A2DP sink" << std::endl;

  Bluetooth Bluetooth;
  Bluetooth.Bluetooth_a2dp_sink_init("Smart_Sound_BT");

  ESPcontroller.CodecInit();
  ESPcontroller.I2SInit();
  


  std::cout << "Success" << std::endl;
  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
  while(1){
    vTaskDelay(xDelay);
  }


}

//End extern
}
