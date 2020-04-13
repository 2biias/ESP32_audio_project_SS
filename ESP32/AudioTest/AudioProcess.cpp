
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>


int main() {

    std::ifstream ifile;
    std::ofstream ofile;
    ifile.open("Input/InputFile.data", std::ifstream::binary|std::ios::in|std::ios::ate);
    ofile.open("Output/OutputFile.data", std::ifstream::binary|std::ios::out|std::ios::ate);
    
    uint64_t size;
    std::vector<short> filebuffer;

    if (ifile.is_open()) 
    {
        size = ifile.tellg();
        uint64_t count = 1;

        std::vector<short> data16((size)/2);
        std::vector<char> data8(size);
        std::vector<char>::iterator data8_it = data8.begin();

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
        //Old output samples
        short oldOutput[2] = {0, 0};
        short oldInput[2] = {0, 0};

        float b[3] = {0.003, 0.007, 0.003};
        float a[2] = {1.814, -0.824};

        /*
        std::vector<short>output(data16.size());
        std::vector<short>::iterator output_it = output.begin();
        */

        for(auto data16_it = data16.begin(); data16_it != data16.end(); data16_it+=2)
        {
            *data16_it = short(float(*data16_it)*b[0] + float(oldInput[0]) * b[1] + float(oldInput[1]) * b[2]) + (float(oldOutput[0]) * a[0] + float(oldOutput[1]) * a[1]);

            oldOutput[0] = *data16_it;
            oldOutput[1] =  oldOutput[0];
            oldInput[0] = *data16_it;
            oldInput[1] = oldInput[0];
        }


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