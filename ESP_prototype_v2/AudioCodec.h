#ifndef _AUDIOCODEC_H_
#define _AUDIOCODEC_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/i2s.h>

#include "AudioElement.h"

class AudioCodec : public AudioElement<uint8_t>
{
private:
    TaskHandle_t taskHandle;
    void AudioCodecTask();
    static void taskWrapper(void* arg);
public:
    AudioCodec(Ringbuffer<uint8_t>*);
    ~AudioCodec();
};

#endif
