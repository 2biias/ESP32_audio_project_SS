#include "Crossover.h"
#include <iostream>
#include <bitset>

Crossover::Crossover(Ringbuffer<short>* read, Ringbuffer<short>* write, float* lp_coeffs, float* hp_coeffs)
{
  //Setting up buffers
  bool res;
  res = setReadBuffer(read);
  std::cout << "Crossover setReadBuffer=" << res << "\n";
  res = setWriteBuffer(write);
  std::cout << "Crossover setWriteBuffer=" << res << "\n";

  // Passing filter coefficients
  for (size_t i = 0; i < 5; i++) {
    lp_coefficients[i] = lp_coeffs[i];
    hp_coefficients[i] = hp_coeffs[i];
  }
  // Zeroing input and output history buffers
  for (size_t i = 0; i < 4; i++) {
    biquads_lp_oldinput[i] = 0;
    biquads_lp_oldoutput[i] = 0;
    biquads_hp_oldinput[i] = 0;
    biquads_hp_oldoutput[i] = 0;
  }

  xTaskCreate(this->taskWrapper, "CrossoverTask", 25000, this, configMAX_PRIORITIES-1, &taskHandle);

  std::cout << "Crossover setup done" << std::endl;
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

    // Read a maximum of 1024 signed 16-bit from equalizer
    len = read(inputbuffer, 1024);

    // Filter using Linkwitz Riley crossover and output dual-channel
    len = filter_LinkwitzRiley(inputbuffer, outputbuffer, len);

    // Write to next ringubffer
    len = write(outputbuffer, len*2);

    writeBufPtr->GiveBinarySemaphore();
  }
}

void Crossover::taskWrapper(void* _this){
    ((Crossover*)_this)->CrossoverTask();
}

uint32_t Crossover::filter_LinkwitzRiley(short* input, short* output, uint32_t len)
{

  float accumulator;
  float f_input;

  for (size_t i = 0; i < len; i++) {

    /*
    // ############################## LP (left channel) ############################## //
    f_input = (float)(input[i]);

    // ******************* Filtering through first lowpass biquad ******************* //
    accumulator = biquads_lp_oldinput[1] * lp_coefficients[2]; //b2
    accumulator += biquads_lp_oldinput[0] * lp_coefficients[1]; //b1
    accumulator += f_input * lp_coefficients[0]; //b0

    // Reorder first biquad input history
    biquads_lp_oldinput[1] = biquads_lp_oldinput[0];
    biquads_lp_oldinput[0] = f_input;

    accumulator += biquads_lp_oldoutput[1] * lp_coefficients[4]; // a2
    accumulator += biquads_lp_oldoutput[0] * lp_coefficients[3]; // a1

    // Reorder first biquad output history
    biquads_lp_oldoutput[1] = biquads_lp_oldoutput[0];
    biquads_lp_oldoutput[0] = accumulator;

    // ******************* Filtering through second lowpass biquad ******************* //
    f_input = accumulator;

    accumulator = biquads_lp_oldinput[3] * lp_coefficients[2]; //b2
    accumulator += biquads_lp_oldinput[2] * lp_coefficients[1]; //b1
    accumulator += f_input * lp_coefficients[0]; //b0

    // Reorder second input history
    biquads_lp_oldinput[3] = biquads_lp_oldinput[2];
    biquads_lp_oldinput[2] = f_input;

    accumulator += biquads_lp_oldoutput[3] * lp_coefficients[4]; // a2
    accumulator += biquads_lp_oldoutput[2] * lp_coefficients[3]; // a1

    // Reorder second output history
    biquads_lp_oldoutput[3] = biquads_lp_oldoutput[2];
    biquads_lp_oldoutput[2] = accumulator;
    */
    output[2*i] = input[i];
    //(short)(accumulator*0.98)

    /*
    // ############################## HP (right channel) ############################## //
    f_input = (float)(input[i]);

    // ******************* Filtering through first hp biquad ******************* //
    accumulator = biquads_hp_oldinput[1] * hp_coefficients[2]; //b2
    accumulator += biquads_hp_oldinput[0] * hp_coefficients[1]; //b1
    accumulator += f_input * hp_coefficients[0]; //b0

    // Reorder first biquad input history
    biquads_hp_oldinput[1] = biquads_hp_oldinput[0];
    biquads_hp_oldinput[0] = f_input;

    accumulator += biquads_hp_oldoutput[1] * hp_coefficients[4]; // a2
    accumulator += biquads_hp_oldoutput[0] * hp_coefficients[3]; // a1

    // Reorder first biquad output history
    biquads_hp_oldoutput[1] = biquads_hp_oldoutput[0];
    biquads_hp_oldoutput[0] = accumulator;

    f_input = accumulator;

    // ******************* Filtering through second hp biquad ******************* //

    accumulator = biquads_hp_oldinput[3] * hp_coefficients[2]; //b2
    accumulator += biquads_hp_oldinput[2] * hp_coefficients[1]; //b1
    accumulator += f_input * hp_coefficients[0]; //b0

    // Reorder second input history
    biquads_hp_oldinput[3] = biquads_hp_oldinput[2];
    biquads_hp_oldinput[2] = f_input;

    accumulator += biquads_hp_oldoutput[3] * hp_coefficients[4]; // a2
    accumulator += biquads_hp_oldoutput[2] * hp_coefficients[3]; // a1

    // Reorder second output history
    biquads_hp_oldoutput[3] = biquads_hp_oldoutput[2];
    biquads_hp_oldoutput[2] = accumulator;
    */
    output[(2*i)+1] = input[i];
  }
  return len;
}
