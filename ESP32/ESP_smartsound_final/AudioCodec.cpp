#include "AudioCodec.h"
#include <iostream>
#include <bitset>

AudioCodec::AudioCodec(Ringbuffer<short>* read)
{
  bool res;
  //Setting up buffers
  res = setReadBuffer(read);
  std::cout << "Audio Codec setReadBuffer=" << res << "\n";
  setWriteBuffer(nullptr);

  xTaskCreate(this->taskWrapper, "AudioCodecTask", 25000, this, configMAX_PRIORITIES-1, &taskHandle); //2 | portPRIVILEGE_BI

  std::cout << "Audio Codec setup done" << std::endl;
}
AudioCodec::~AudioCodec()
{
  setReadBuffer(nullptr);
  setWriteBuffer(nullptr);
}

uint32_t AudioCodec::ShortToBytes(short* input, uint8_t* output, uint32_t len)
{
  // Splitting short into 2 bytes
  for (size_t i = 0; i < len; i++) {
    output[(2*i)+1] = (input[i] >> 8);
    output[(2*i)] = (input[i] & 0xff);
  }
  // Returns bytes
  return (2*len);
}

void AudioCodec::AudioCodecTask()
{
  uint32_t readlen, bytes = 0;
  short inputbuffer[MAXSAMPLES/2] = {0};
  uint8_t outputbuffer[MAXSAMPLES] = {0};

  uint32_t i2s_bytes_written  = 0;

  Ringbuffer<short>* ReadBufPtr = getReadBuffer();

  for(;;){
    ReadBufPtr->TakeBinarySemaphore();

    readlen = read(inputbuffer, MAXSAMPLES/2);

    bytes = ShortToBytes(inputbuffer, outputbuffer, readlen);

    i2s_write(I2S_NUM_0, outputbuffer, bytes, &i2s_bytes_written, portMAX_DELAY);

    gpio_set_level(GPIO_NUM_15, 0);
  }
}

void AudioCodec::taskWrapper(void* _this){
    ((AudioCodec*)_this)->AudioCodecTask();
}
