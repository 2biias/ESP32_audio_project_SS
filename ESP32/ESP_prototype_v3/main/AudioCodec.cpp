#include "AudioCodec.h"

AudioCodec::AudioCodec(Ringbuffer<uint8_t>* read)
{
  bool res;
  //Setting up buffers
  res = setReadBuffer(read);
  printf("AC setReadBuffer = %u\n", res);
  setWriteBuffer(nullptr);
  xTaskCreate(this->taskWrapper, "AudioCodecTask", 80000, this, configMAX_PRIORITIES-1, &taskHandle); //2 | portPRIVILEGE_BI
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

  Ringbuffer<uint8_t>* thisRingbuffer = getReadBuffer();

  for(;;){
    //vTaskDelay(10 / portTICK_PERIOD_MS);
    //gpio_set_level(GPIO_NUM_13, 1);
    thisRingbuffer->TakeBinarySemaphore();
    bytesRead = read(buffer, 4096);
    //gpio_set_level(GPIO_NUM_13, 0);

    //memcpy(buffer16, buffer, 256);
    gpio_set_level(GPIO_NUM_13, 1);
    i2s_write(I2S_NUM_0, buffer, bytesRead, &i2s_bytes_written, 100);
    //i2s_write_expand(I2S_NUM_0, buffer, bytesRead, 8, 16, &i2s_bytes_written, 100);
    gpio_set_level(GPIO_NUM_13, 0);
    //printf("Ring: %i, I2S %u\n", bytesRead, i2s_bytes_written);
    //i2s_write(I2S_NUM_0, (char*)buffer, 200, &i2s_bytes_written, 100);
  }

}

void AudioCodec::taskWrapper(void* _this){
    ((AudioCodec*)_this)->AudioCodecTask();
}
