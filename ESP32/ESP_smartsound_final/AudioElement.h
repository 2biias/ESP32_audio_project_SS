#ifndef _AUDIOELEMENT_H_
#define _AUDIOELEMENT_H_

#include "Ringbuffer.h"

template<class R, class W>
class AudioElement
{
private:
    Ringbuffer<R>* readbuffer;
    Ringbuffer<W>* writebuffer;
public:
    AudioElement()
    : readbuffer(nullptr), writebuffer(nullptr)
    {}

    virtual ~AudioElement()
    {}

    bool setReadBuffer(Ringbuffer<R>* buffer)
    {
        if(buffer == nullptr){
          readbuffer = nullptr;
          return false;
        }

        readbuffer = buffer;
        return true;
    }

    bool setWriteBuffer(Ringbuffer<W>* buffer)
    {
        if(buffer == nullptr){
            writebuffer = nullptr;
            return false;
        }
        writebuffer = buffer;
        return true;
    }

    Ringbuffer<R>* getReadBuffer()
    {
      if(readbuffer != nullptr)
        return readbuffer;
      return nullptr;
    }

    Ringbuffer<W>* getWriteBuffer()
    {
      if(writebuffer != nullptr)
        return writebuffer;
      return nullptr;
    }

    int read(R *buf, uint32_t len)
    {
        if(readbuffer != nullptr)
        {
            return readbuffer->read(buf, len);
        }
        return 0;
    }

    int write(const W* buf, uint32_t len)
    {
        if(writebuffer != nullptr)
        {
            return writebuffer->write(buf, len);
        }
        return 0;
    }
};
#endif
