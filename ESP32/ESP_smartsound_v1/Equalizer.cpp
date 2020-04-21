#include "Equalizer.h"
#include <iostream>
#include <bitset>


Equalizer::Equalizer(Ringbuffer<uint8_t>* read, Ringbuffer<short>* write)
{
  //Setting up buffers
  bool res;
  res = setReadBuffer(read);
  printf("Equalizer setReadBuffer = %u\n", res);
  res = setWriteBuffer(write);
  printf("Equalizer setWriteBuffer = %u\n", res);
  xTaskCreate(this->taskWrapper, "EqualizerTask", 20000, this, configMAX_PRIORITIES-1, &taskHandle);
}
Equalizer::~Equalizer()
{
  setReadBuffer(nullptr);
  setWriteBuffer(nullptr);
}

uint32_t Equalizer::MonoMix(uint8_t* input, short* output, uint32_t bytes)
{

  // Read data as 16-bit signed little endian
  short valueL, valueR;
  for (size_t i = 0; i < bytes; i+=4) {

    valueL = (input[i+1] << 8) | (input[i] & 0xff);
    valueR = (input[i+3] << 8) | (input[i+2] & 0xff);

    // Calculate mean and place in left channel
    valueL = (valueL/2)+(valueR/2);
    output[(i/4)] = valueL;
    /*
    iobuffer[i+1] = (valueL >> 8);
    iobuffer[i] = valueL & 0xff;
    */
  }
  return (bytes/4);
}

void Equalizer::EqualizerTask()
{
  uint32_t len, bytes = 0;
  uint8_t inputbuffer[4096] = {0};
  short outputbuffer[1024] = {0}; // For mainxed signal

  // Pointers to buffer will be used for signaling with semaphores
  Ringbuffer<uint8_t>* readBufPtr = getReadBuffer();
  Ringbuffer<short>* writeBufPtr = getWriteBuffer();

  for(;;){

    readBufPtr->TakeBinarySemaphore();
    bytes = read(inputbuffer, 4096);

    //std::cout << bytes << "\n";
    std::cout << std::bitset<8>(inputbuffer[1]) << std::bitset<8>(inputbuffer[0]) << " "<< std::bitset<8>(inputbuffer[3]) << std::bitset<8>(inputbuffer[2]) << "\n";

    // Mix received signal into mono-signal
    len = MonoMix(inputbuffer, outputbuffer, bytes);

    //std::cout << len*4 << "\n";

    std::cout << std::bitset<16>(outputbuffer[0]) << "\n";

    // Equalize the mixed signal
    //Equalize(buffer, len);

    len = write(outputbuffer, len);

    writeBufPtr->GiveBinarySemaphore();
  }
}

void Equalizer::taskWrapper(void* _this){
    ((Equalizer*)_this)->EqualizerTask();
}
