#ifndef _AUDIOELEMENT_H_
#define _AUDIOELEMENT_H_

#include "Ringbuffer.h"

template<class T>
class AudioElement
{
private:
    Ringbuffer<T>* readbuffer;
    Ringbuffer<T>* writebuffer;
public:
    AudioElement()
    : readbuffer(nullptr), writebuffer(nullptr)
    {}

    virtual ~AudioElement()
    {}

    bool setReadBuffer(Ringbuffer<T>* buffer)
    {
        if(buffer == nullptr){
          readbuffer = nullptr;
          return false;
        }

        readbuffer = buffer;
        return true;
    }

    bool setWriteBuffer(Ringbuffer<T>* buffer)
    {
        if(buffer == nullptr){
            writebuffer = nullptr;
            return false;
        }
        writebuffer = buffer;
        return true;
    }

    int read(T *buf, uint32_t len)
    {
        if(readbuffer != nullptr)
        {
            return readbuffer->read(buf, len);
        }
        return 0;
    }

    int write(const T* buf, uint32_t len)
    {
        if(writebuffer != nullptr)
        {
            return writebuffer->write(buf, len);
        }
        return 0;
    }

};

#endif
