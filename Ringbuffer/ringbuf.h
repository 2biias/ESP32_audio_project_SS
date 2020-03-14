
class ringbuf{
public:
ringbuf(size_t len);
~ringbuf();
void read(uint8_t data*, len);
void write(uint8_t data*, len);
private:
const size_t len;
uint8_t* buffer;
uint8_t* writePtr;
uint8_t* readPtr;
}
