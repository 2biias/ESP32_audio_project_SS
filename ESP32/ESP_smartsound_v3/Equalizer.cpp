#include "Equalizer.h"
#include <iostream>
#include <bitset>

Equalizer::Equalizer(Ringbuffer<uint8_t>* read, Ringbuffer<short>* write)
{
  //Setting up buffers
  bool res;
  res = setReadBuffer(read);
  std::cout << "Equalizer setReadBuffer=" << res << "\n";
  res = setWriteBuffer(write);
  std::cout << "Equalizer setWriteBuffer=" << res << "\n";
  // Hardcoding filters until GUI-interface is ready. Would potentilly be set
  // to passthrough
  // Shelves init
  for (size_t i = 0; i < 2; i++) {
    Lowshelf.Ic_eq[i] = 0;
    Highshelf.Ic_eq[i] = 0;
  }
  // Low shelf constants (for now)
  Lowshelf.fc = 88;
  Lowshelf.Q = 0.5;
  Lowshelf.A = 1.995*correction; // 10 dB  - 10^(dB/40)
  Lowshelf.g = tan((pi * Lowshelf.fc / fs))/sqrt(Lowshelf.A);
  //Lowshelf.A = Lowshelf.A * correction; // Correction factor
  Lowshelf.k = 1/Lowshelf.Q;
  Lowshelf.a1 = 1/(1 + Lowshelf.g*(Lowshelf.g + Lowshelf.k));
  Lowshelf.a2 = Lowshelf.g * Lowshelf.a1;
  Lowshelf.k1 = Lowshelf.k * (Lowshelf.A - 1);
  Lowshelf.k2 = ((Lowshelf.A * Lowshelf.A) - 1);

  // High shelf constants (for now)
  Highshelf.fc = 5680;
  Highshelf.Q = 0.5;
  Highshelf.A = 1.995*correction; //*correction;
  Highshelf.g = tan((pi * Highshelf.fc / fs))*sqrt(Highshelf.A);
  //Highshelf.A = Highshelf.A * correction;
  Highshelf.k = 1/Highshelf.Q;
  Highshelf.a1 = 1/(1 + Highshelf.g*(Highshelf.g + Highshelf.k));
  Highshelf.a2 = Highshelf.g * Highshelf.a1;
  Highshelf.k1 = Highshelf.A * Highshelf.A;
  Highshelf.k2 = Highshelf.k * (1 - Highshelf.A) * Highshelf.A;
  Highshelf.k3 = (1 - (Highshelf.A * Highshelf.A));

  // Bells init
  for (size_t i = 0; i < 12; i++) {
    Bells.Ic_eq[i] = 0;
  }
  // Setting bells constants (fow now)
  for (size_t i = 0; i < 6; i++) {

    Bells.fc[i] = 125*pow(2, i);
    Bells.Q[i] = 0.5;
    Bells.A[i] = 1.995*correction; //*correction; // 10 dB
    Bells.g[i] = tan((pi * Bells.fc[i] / fs));
    Bells.k[i] = 1/(Bells.Q[i] * Bells.A[i]);
    Bells.a1[i] = 1/(1 + Bells.g[i]*(Bells.g[i] + Bells.k[i]));
    Bells.a2[i] = Bells.g[i] * Bells.a1[i];
    Bells.R[i] = Bells.k[i] * ((Bells.A[i] * Bells.A[i]) - 1);
  }

  // Start equalizer task
  xTaskCreate(this->taskWrapper, "EqualizerTask", 20000, this, configMAX_PRIORITIES-1, &taskHandle);

  std::cout << "Equalizer setup done" << std::endl;
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
  }
  return (bytes/4);
}

uint32_t Equalizer::filter_lowshelf(short* input, float* output, uint32_t len)
{
  float v0, v1, v2 = 0;

  for(size_t i = 0; i < len; i++) {
    v0 = (float)(input[i]);
    v1 = Lowshelf.a1 * Lowshelf.Ic_eq[0] + Lowshelf.a2 * (v0 - Lowshelf.Ic_eq[1]);
    v2 = Lowshelf.g * v1 + Lowshelf.Ic_eq[1];
    // Update equivalent currents
    Lowshelf.Ic_eq[0] = 2 * v1 - Lowshelf.Ic_eq[0];
    Lowshelf.Ic_eq[1] = 2 * v2 - Lowshelf.Ic_eq[1];
    // Reset output accumulator
    output[i] = (v0 + Lowshelf.k1 * v1 + Lowshelf.k2 * v2); // Filtered signal
  }
  return len;
}

uint32_t Equalizer::filter_highshelf(short* input, float* output, uint32_t len)
{
  float v0, v1, v2 = 0;

  for (size_t i = 0; i < len; i++) {
    v0 = (float)(input[i]);
    // Calculating bandpass and lowpass voltages
    v1 = Highshelf.a1 * Highshelf.Ic_eq[0] + Highshelf.a2 * v0 - Highshelf.a2 * Highshelf.Ic_eq[1];
    v2 = Highshelf.g * v1 + Highshelf.Ic_eq[1];
    // Updating equivalent currents
    Highshelf.Ic_eq[0] = 2 * v1 - Highshelf.Ic_eq[0];
    Highshelf.Ic_eq[1] = 2 * v2 - Highshelf.Ic_eq[1];
    // Add to output accumulator
    output[i] += (Highshelf.k1*v0 + Highshelf.k2*v1 + Highshelf.k3*v2);
  }
  return len;
}

uint32_t Equalizer::filter_bell(short* input, float* output, uint32_t len)
{
  float v0, v1, v2 = 0;

  for (size_t i = 0; i < len; i++) {

    v0 = (float)(input[i]);

    // Looping through bell-filters
    for (size_t ii = 0; ii < 6; ii++) {
      // Calculating bandpass and lowpass voltages
      v1 = Bells.a1[ii] * Bells.Ic_eq[2*ii] + Bells.a2[ii] * (v0 - Bells.Ic_eq[2*ii + 1]);
      v2 = Bells.Ic_eq[2*ii + 1] + Bells.g[ii] * v1;
      // Updating equivalent currents
      Bells.Ic_eq[2*ii] = 2 * v1 - Bells.Ic_eq[2*ii];
      Bells.Ic_eq[2*ii + 1] = 2 * v2 - Bells.Ic_eq[2*ii + 1];
      // Add to output accumulator
      output[i] += (v0 + Bells.R[ii] * v1);
    }
  }
  return len;
}

void Equalizer::adjustEqualizer(float* Q_factor_array, float* gain_array, float* fc_array)
{

  // Adjusting lowshelf
  Lowshelf.Q = Q_factor_array[0];
  Lowshelf.A = gain_array[0];
  Lowshelf.fc = fc_array[0];

  Lowshelf.g = tan((pi * Lowshelf.fc / fs))/sqrt(Lowshelf.A);
  //Lowshelf.A = Lowshelf.A * correction; // Correction factor
  Lowshelf.k = 1/Lowshelf.Q;
  Lowshelf.a1 = 1/(1 + Lowshelf.g*(Lowshelf.g + Lowshelf.k));
  Lowshelf.a2 = Lowshelf.g * Lowshelf.a1;
  Lowshelf.k1 = Lowshelf.k * (Lowshelf.A - 1);
  Lowshelf.k2 = ((Lowshelf.A * Lowshelf.A) - 1);

  // Adjusting Highshelf
  Highshelf.Q = Q_factor_array[1];
  Highshelf.A = gain_array[1];
  Highshelf.fc = fc_array[1];

  Highshelf.k = 1/Highshelf.Q;
  Highshelf.a1 = 1/(1 + Highshelf.g*(Highshelf.g + Highshelf.k));
  Highshelf.a2 = Highshelf.g * Highshelf.a1;
  Highshelf.k1 = Highshelf.A * Highshelf.A;
  Highshelf.k2 = Highshelf.k * (1 - Highshelf.A) * Highshelf.A;
  Highshelf.k3 = (1 - (Highshelf.A * Highshelf.A));

  // Adjusting bells
  for (size_t i = 0; i < 6; i++) {
    Bells.Q[i] = Q_factor_array[i+2];
    Bells.A[i] = gain_array[i+2];
    Bells.fc[i] = fc_array[i+2];

    Bells.g[i] = tan((pi * Bells.fc[i] / fs));
    Bells.k[i] = 1/(Bells.Q[i] * Bells.A[i]);
    Bells.a1[i] = 1/(1 + Bells.g[i]*(Bells.g[i] + Bells.k[i]));
    Bells.a2[i] = Bells.g[i] * Bells.a1[i];
    Bells.R[i] = Bells.k[i] * ((Bells.A[i] * Bells.A[i]) - 1);
  }
}

void Equalizer::EqualizerTask()
{
  uint32_t len, bytes = 0;
  uint8_t inputbuffer[4096] = {0}; // For receiving raw bytes
  short outputbuffer[1024] = {0}; // For mono-mixed 16-bit samples
  float accumulator[1024] = {0}; // For accumulating equalized samples

  // Pointers to buffer will be used for signaling with semaphores
  Ringbuffer<uint8_t>* readBufPtr = getReadBuffer();
  Ringbuffer<short>* writeBufPtr = getWriteBuffer();

  for(;;){

    readBufPtr->TakeBinarySemaphore();
    bytes = read(inputbuffer, 4096);

    // Mix received signal into mono-signal
    len = MonoMix(inputbuffer, outputbuffer, bytes);

    // Filter bank ouput to accumulor
    len = filter_lowshelf(outputbuffer, accumulator, len);
    len = filter_highshelf(outputbuffer, accumulator, len);
    len = filter_bell(outputbuffer, accumulator, len);


    //normalise to 12dB, account for number of filters in bank
    for (size_t i = 0; i < len; i++){
      outputbuffer[i] = (short)(accumulator[i]*0.027); // * 1/n filters (-18dB) * 1/4.467 (-13dB)
    }

    len = write(outputbuffer, len);

    writeBufPtr->GiveBinarySemaphore();
  }
}

void Equalizer::taskWrapper(void* _this){
    ((Equalizer*)_this)->EqualizerTask();
}
