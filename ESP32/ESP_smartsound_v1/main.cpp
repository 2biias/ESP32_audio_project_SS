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
#include "AudioCodec.h"
#include "Equalizer.h"
#include "Crossover.h"

extern "C"{

void app_main(){

  printf("Heapsize: %i\n",xPortGetFreeHeapSize());
  ESPcontroller ESPcontroller;
  ESPcontroller.InitFlash();

  //Setting up Ringbuffers
  Ringbuffer<uint8_t> buffer1(12288);
  Ringbuffer<short> buffer2(6144);
  Ringbuffer<short> buffer3(6144);
  ESPcontroller.I2SInit();
  ESPcontroller.CodecInit();

  //Initializing and starting Bluetooth service by given configations.
  //If no other bluetooth sevice is not started. Configures bt-controller.
  //Enables controller. Init/Enables bluedroid. Initialises controller as
  //A2DP_sink and sets stream type to AUDIO_STREAM_READER.'
  std::cout << "Setting up Bluetooth A2DP sink" << std::endl;
  printf("Heapsize: %i\n",xPortGetFreeHeapSize());
  Bluetooth Bluetooth(&buffer1);
  Equalizer Equalizer(&buffer1, &buffer2);
  Crossover Crossover(&buffer2, &buffer3);
  AudioCodec AudioCodec(&buffer3);

  printf("Heapsize: %i\n",xPortGetFreeHeapSize());
  Bluetooth.Bluetooth_a2dp_sink_init("Smart_Sound_BT");
  printf("Heapsize: %i\n",xPortGetFreeHeapSize());

  std::cout << "Bluetooth setup completed" << std::endl;
  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
  while(1){
    vTaskDelay(xDelay);
  }

}

//End extern
}
