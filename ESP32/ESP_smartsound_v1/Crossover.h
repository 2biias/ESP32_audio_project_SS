#ifndef _CROSSOVER_H_
#define _CROSSOVER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "AudioElement.h"

class Crossover : public AudioElement<short, short>
{
public:
    Crossover(Ringbuffer<short>* read, Ringbuffer<short>* write, float* lp_coefficients, float* hp_coefficients);
    uint32_t filter_LinkwitzRiley(short* input, short* output, uint32_t len);
    ~Crossover();
private:
    TaskHandle_t taskHandle;
    void CrossoverTask();
    static void taskWrapper(void* arg);
    // Lowpass
    float lp_coefficients[5];
    float biquads_lp_oldinput[4];
    float biquads_lp_oldoutput[4];
    // Highpass
    float hp_coefficients[5];
    float biquads_hp_oldinput[4];
    float biquads_hp_oldoutput[4];
};

#endif
