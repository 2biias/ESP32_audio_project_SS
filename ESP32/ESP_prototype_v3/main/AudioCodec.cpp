#include "AudioCodec.h"

AudioCodec::AudioCodec(Ringbuffer<uint8_t>* read)
{
  bool res;
  //Setting up buffers
  res = setReadBuffer(read);
  printf("AC setReadBuffer = %u\n", res);
  setWriteBuffer(nullptr);
  xTaskCreate(this->taskWrapper, "AudioCodecTask", 80000, this, 2 | portPRIVILEGE_BIT, &taskHandle);
}
AudioCodec::~AudioCodec()
{
  setReadBuffer(nullptr);
  setWriteBuffer(nullptr);
}

void AudioCodec::AudioCodecTask()
{
  int bytesRead = 0;
  //Stack size 50000 writing 512
  uint8_t buffer[4096] = {0};
  //int16_t buffer16[128] = {0};
  uint32_t i2s_bytes_written  = 0;
  vTaskDelay(15000 / portTICK_PERIOD_MS);
  for(;;){
    //vTaskDelay(10 / portTICK_PERIOD_MS);
    //gpio_set_level(GPIO_NUM_13, 1);
    bytesRead = read(buffer, 4096);
    //gpio_set_level(GPIO_NUM_13, 0);
    if(bytesRead != 0){
      //memcpy(buffer16, buffer, 256);
      i2s_write(I2S_NUM_0, buffer, 4096, &i2s_bytes_written, 100);
      //gpio_set_level(GPIO_NUM_13, 0);
    }else{
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    //printf("AudioCodec writing %i to codec\n", bytesRead);
    //i2s_write(I2S_NUM_0, (char*)buffer, 200, &i2s_bytes_written, 100);
  }

}

void AudioCodec::taskWrapper(void* _this){
    ((AudioCodec*)_this)->AudioCodecTask();
}
