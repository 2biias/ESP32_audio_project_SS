#ifndef _AUDIOCODEC_H_
#define _AUDIOCODEC_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/i2s.h>
#include <driver/gpio.h>
#include <stdio.h>

#include "AudioElement.h"

#define MAXSAMPLES 10240

class AudioCodec : public AudioElement<short, uint8_t>
{
private:
    TaskHandle_t taskHandle;
    void AudioCodecTask();
    uint32_t ShortToBytes(short* input, uint8_t* output, uint32_t len);
    static void taskWrapper(void* arg);
public:
    AudioCodec(Ringbuffer<short>*);
    ~AudioCodec();
};

#endif
