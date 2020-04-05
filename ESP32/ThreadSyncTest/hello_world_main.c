/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#define SIZE 1000

struct buffer {
    int buffer[SIZE];
    SemaphoreHandle_t xSemaphore;
};

struct buffer Buffer1, Buffer2;

void threadfunc(void* arg){
  for (;;) {
    /* code */
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
    xSemaphoreTake( Buffer1.xSemaphore, portMAX_DELAY );
    printf("Thread 1 working.\n");
    for (size_t i = 0; i < SIZE; i++) {
      /* code */
        Buffer2.buffer[i] = Buffer1.buffer[i];
    }
    xSemaphoreGive(Buffer2.xSemaphore);
  }
}


void threadfunc2(void* arg){

  for (;;) {
    /* code */
    xSemaphoreTake( Buffer2.xSemaphore, portMAX_DELAY );
    printf("Thread 2 working.\n");
    for (size_t i = 0; i < SIZE; i++) {
      /* code */
      Buffer2.buffer[i] = i+1;
    }
  }
}

void app_main()
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    Buffer1.xSemaphore = xSemaphoreCreateBinary();
    Buffer2.xSemaphore = xSemaphoreCreateBinary();

    //Creating thread
    BaseType_t threadRet;
    TaskHandle_t threadHandle, threadHandle2 = NULL;

    threadRet = xTaskCreate(threadfunc, "Thread", 10000, NULL, 2 | portPRIVILEGE_BIT, &threadHandle);
    threadRet = xTaskCreate(threadfunc2, "Thread", 10000, NULL, 2 | portPRIVILEGE_BIT, &threadHandle2);

    for (;;) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Main thread working.\n");
        for (size_t i = 0; i < SIZE; i++){
            Buffer1.buffer[i] = i+10;
        }
        xSemaphoreGive(Buffer1.xSemaphore);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
