import spidev #Spi library

class Spi(spidev):
    def __init__(self):
        spidev.__init__()
        self.bus = 0;
        self.device = 0;
        self.open(self.bus, self.device)
        self.bits_per_word = 8
        self.max_speed_hz = 5000
        self.mode = 0

    def adjustPasband(self, volume, pasband):
        self.xfer([0b10100000])
        self.xfer([pasband])
        self.xfer([volume])
        
        if self.xfer([0]) == 0b10100000:
            return True
        else:
            return False
        
