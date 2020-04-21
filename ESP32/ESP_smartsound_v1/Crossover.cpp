#include "Crossover.h"
#include <iostream>
#include <bitset>

Crossover::Crossover(Ringbuffer<short>* read, Ringbuffer<short>* write)
{
  //Setting up buffers
  bool res;
  res = setReadBuffer(read);
  printf("Crossover setReadBuffer = %u\n", res);
  res = setWriteBuffer(write);
  printf("Crossover setWriteBuffer = %u\n", res);
  xTaskCreate(this->taskWrapper, "CrossoverTask", 20000, this, configMAX_PRIORITIES-1, &taskHandle);
}
Crossover::~Crossover()
{
  setReadBuffer(nullptr);
  setWriteBuffer(nullptr);
}

void Crossover::CrossoverTask()
{
  uint32_t len = 0;
  short inputbuffer[1024] = {0};
  short outputbuffer[2048] = {0};
  // Pointers to buffer will be used for signaling
  Ringbuffer<short>* readBufPtr = getReadBuffer();
  Ringbuffer<short>* writeBufPtr = getWriteBuffer();

  for(;;){
    // Block untill lock given
    readBufPtr->TakeBinarySemaphore();

    // Read 1024 signed 16-bit from equalizer
    len = read(inputbuffer, 1024);
    //std::cout << len*4 << "\n";

    //std::cout << std::bitset<16>(inputbuffer[0]) << "\n";

    // Filter using Linkwitz Riley crossover
    //bytes = crossoverfilter(buffer, 1024);

    dummyfilter(inputbuffer, outputbuffer, len);

    //std::cout << len*4 << "\n";
    //std::cout << std::bitset<16>(outputbuffer[0]) << " " << std::bitset<16>(outputbuffer[1]) << "\n";

    len = write(outputbuffer, len*2);
    //std::cout << len*4 << "\n";

    writeBufPtr->GiveBinarySemaphore();
  }
}

void Crossover::taskWrapper(void* _this){
    ((Crossover*)_this)->CrossoverTask();
}

void Crossover::dummyfilter(short* in, short* out, uint32_t len){

  for (size_t i = 0; i < len; i++) {
    /* code */
    out[2*i] = in[i];
    out[(2*i)+1] = in[i];
  }
}
