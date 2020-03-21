#ifndef _SCOPEDLOCKER_H_
#define _SCOPEDLOCKER_H_

#include "freertos/semphr.h"

#define INCLUDE_vTaskSuspend 1

class ScopedLocker
{
private:
    SemaphoreHandle_t* SemHandle;
public:
    ScopedLocker(SemaphoreHandle_t* mutex) : SemHandle(mutex)
    {
      //Take lock
      xSemaphoreTake(*SemHandle, portMAX_DELAY);
    }
    ~ScopedLocker()
    {
      //Give lock
      xSemaphoreGive(*SemHandle);
    }
};

#endif
