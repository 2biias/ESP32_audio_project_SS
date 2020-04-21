#ifndef _EQUALIZER_H_
#define _EQUALIZERC_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "AudioElement.h"

class Equalizer : public AudioElement<uint8_t, short>
{
private:
    TaskHandle_t taskHandle;
    uint32_t MonoMix(uint8_t* input, short* output, uint32_t len);
    void EqualizerTask();
    static void taskWrapper(void* arg);
public:
    Equalizer(Ringbuffer<uint8_t>* read, Ringbuffer<short>* write);
    ~Equalizer();
};

#endif
