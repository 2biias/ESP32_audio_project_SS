#ifndef _EQUALIZER_H_
#define _EQUALIZERC_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "math.h"
#include <iostream>
#include "AudioElement.h"

#define pi 3.14159265

class Equalizer : public AudioElement<uint8_t, short>
{
private:
    TaskHandle_t taskHandle;

    uint32_t fs = 48000;
    float correction = 2.498; // Factor used to correct filter gains. (n*A - n-1)/A
    struct Lowshelf_t {
      float Ic_eq[2];
      float fc, Q, A, g, k, a1, a2, k1, k2;
    };
    Lowshelf_t Lowshelf;

    struct Highshelf_t {
      float Ic_eq[2];
      float fc, Q, A, g, k, a1, a2, k1, k2, k3;
    };
    Highshelf_t Highshelf;

    struct Bells_t {
      float Ic_eq[12];
      float fc[6];
      float Q[6];
      float A[6];
      float g[6];
      float k[6];
      float a1[6];
      float a2[6];
      float R[6];
    };
    Bells_t Bells;

    uint32_t MonoMix(uint8_t* input, short* output, uint32_t len);
    uint32_t filter_lowshelf(short* input, float* output, uint32_t len);
    uint32_t filter_highshelf(short* input, float* output, uint32_t len);
    uint32_t filter_bell(short* input, float* output, uint32_t len);
    void adjustEqualizer(float* Q_factor_array, float* gain_array, float* fc_array);
    void EqualizerTask();
    static void taskWrapper(void* arg);
public:
    Equalizer(Ringbuffer<uint8_t>* read, Ringbuffer<short>* write);
    ~Equalizer();
};

#endif
