
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

#include "filter1.h"

int main() {

    std::ifstream ifile;
    std::ofstream ofile;
    ifile.open("InputFile.data", std::ifstream::binary|std::ios::in|std::ios::ate);
    ofile.open("OutputFile.data", std::ifstream::binary|std::ios::out|std::ios::ate);

    uint64_t size;
    std::vector<short> filebuffer;

    if (ifile.is_open()) 
    {
        size = ifile.tellg();
        uint64_t count = 1;

        std::vector<short> data16((size)/2);
        std::vector<char> data8(size);
        std::vector<char>::iterator data8_it = data8.begin();
        //std::vector<char>::iterator data16_it = data16.begin();

        ifile.seekg(0, std::ios::beg);

        // Read data as 16-bit signed little endian
        for (auto &data16_it : data16)
        {
            ifile.read((&(*data8_it)), 2);
            data16_it = (*(data8_it+1) << 8 ) | (*data8_it & 0xff);
            // Jump 2 bytes
            data8_it+=2;
            count++;
        }
        data8_it = data8.begin();

        // Mono mix
        for (auto data16_it = data16.begin() ; data16_it!= data16.end() ; data16_it+=2)
        {
            
            int avg = (*data16_it + *(data16_it+1))/2;
            *data16_it = avg;
            *(data16_it+1) = avg;
        }

        // Crossover



        // Write back elements in big endian
        for (auto &data16_it : data16)
        {
            short value = data16_it;
            *(data8_it+1) = value & 0xff;
            *(data8_it) = (value >> 8);
            ofile.write((&(*data8_it)), 2);
            data8_it+=2;
        }
        
    }
    ifile.close();
    ofile.close();
}