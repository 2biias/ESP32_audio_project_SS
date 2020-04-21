#include "AudioCodec.h"
#include <iostream>
#include <bitset>

AudioCodec::AudioCodec(Ringbuffer<short>* read)
{
  bool res;
  //Setting up buffers
  res = setReadBuffer(read);
  printf("AudioCodec setReadBuffer = %u\n", res);
  setWriteBuffer(nullptr);
  xTaskCreate(this->taskWrapper, "AudioCodecTask", 20000, this, configMAX_PRIORITIES-1, &taskHandle); //2 | portPRIVILEGE_BI
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
  short inputbuffer[2048] = {0};
  uint8_t outputbuffer[4096] = {0};

  uint32_t i2s_bytes_written  = 0;

  Ringbuffer<short>* ReadBufPtr = getReadBuffer();

  for(;;){

    //gpio_set_level(GPIO_NUM_13, 1);
    ReadBufPtr->TakeBinarySemaphore();
    readlen = read(inputbuffer, 2048);
    //std::cout << readlen << "\n";
    //std::cout << std::bitset<16>(inputbuffer[0]) << " " << std::bitset<16>(inputbuffer[1]) << "\n";

    bytes = ShortToBytes(inputbuffer, outputbuffer, readlen);

    //std::cout << bytes << "\n";

    //std::cout << std::bitset<8>(outputbuffer[1]) << std::bitset<8>(outputbuffer[0]) << " " << std::bitset<8>(outputbuffer[3]) << std::bitset<8>(outputbuffer[2]) << "\n";

    //gpio_set_level(GPIO_NUM_13, 1);
    i2s_write(I2S_NUM_0, outputbuffer, bytes, &i2s_bytes_written, 100);
    //std::cout << i2s_bytes_written << "\n";
    gpio_set_level(GPIO_NUM_15, 0);

  }
}

void AudioCodec::taskWrapper(void* _this){
    ((AudioCodec*)_this)->AudioCodecTask();
}
