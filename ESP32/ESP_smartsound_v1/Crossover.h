#ifndef _CROSSOVER_H_
#define _CROSSOVER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "AudioElement.h"

class Crossover : public AudioElement<short, short>
{
private:
    TaskHandle_t taskHandle;
    void CrossoverTask();
    static void taskWrapper(void* arg);
    void dummyfilter(short*, short*, uint32_t);
public:
    Crossover(Ringbuffer<short>* read, Ringbuffer<short>* write);
    ~Crossover();
};

#endif
