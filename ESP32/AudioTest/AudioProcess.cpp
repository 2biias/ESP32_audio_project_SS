
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

        {
            // ******************************* LP (left channel) ******************************* //
            float biquad1_oldinput[4] = {0, 0};
            float biquad1_oldoutput[4] = {0, 0};
            
            float biquad2_oldinput[4] = {0, 0};
            float biquad2_oldoutput[4] = {0, 0};

            float filter_coefficients[5] = 
            {
                0.003916126660547371, 0.007832253321094742, 0.003916126660547371, 1.815341082704568, -0.8310055893467575//// b0, b1, b2, a1, a2
            };

            float accumulator;
            float output, input;

            for(auto data16_it = data16.begin(); data16_it != data16.end(); data16_it+=2)
            {
                accumulator = 0;
                input = 0;
                output = 0;
                
                input = (float)(*data16_it);

                // ******************* Filtering through biquad 1 *******************//
                accumulator = biquad1_oldinput[1] * filter_coefficients[2]; //b2
                accumulator += biquad1_oldinput[0] * filter_coefficients[1]; //b1
                accumulator += input * filter_coefficients[0]; //b0

                // Reorder biquad1 input history
                biquad1_oldinput[1] = biquad1_oldinput[0];
                biquad1_oldinput[0] = input;

                accumulator += biquad1_oldoutput[1] * filter_coefficients[4]; // a2
                accumulator += biquad1_oldoutput[0] * filter_coefficients[3]; // a1

                // Reorder biquad1 output history
                biquad1_oldoutput[1] = biquad1_oldoutput[0];
                biquad1_oldoutput[0] = accumulator;

                output = accumulator;

                
                // ******************* Filtering through biquad 2 *******************//
                // Resetting accumulator
                
                accumulator = 0;
                input = output;

                accumulator = biquad2_oldinput[1] * filter_coefficients[2]; //b2
                accumulator += biquad2_oldinput[0] * filter_coefficients[1]; //b1
                accumulator += input * filter_coefficients[0]; //b0

                // Reorder biquad1 input history
                biquad2_oldinput[1] = biquad2_oldinput[0];
                biquad2_oldinput[0] = input;

                accumulator += biquad2_oldoutput[1] * filter_coefficients[4]; // a2
                accumulator += biquad2_oldoutput[0] * filter_coefficients[3]; // a1

                // Reorder biquad1 output history
                biquad2_oldoutput[1] = biquad2_oldoutput[0];
                biquad2_oldoutput[0] = accumulator;

                *data16_it = (short)(accumulator*0.99);
                
            }
        }

        {
            // ******************************* HP (left channel) ******************************* //
            float biquad1_oldinput[4] = {0, 0};
            float biquad1_oldoutput[4] = {0, 0};
            
            float biquad2_oldinput[4] = {0, 0};
            float biquad2_oldoutput[4] = {0, 0};

            float filter_coefficients[5] = 
            {
                0.9115866680128322, -1.8231733360256643, 0.9115866680128322, 1.815341082704568, -0.8310055893467575// b0, b1, b2, a1, a2
            };

            float accumulator;
            float output, input;

            for(auto data16_it = data16.begin()+1; data16_it != (data16.end()-1); data16_it+=2)
            {
                accumulator = 0;
                input = 0;
                output = 0;
                
                input = (float)(*data16_it);

                // ******************* Filtering through biquad 1 *******************//
                accumulator = biquad1_oldinput[1] * filter_coefficients[2]; //b2
                accumulator += biquad1_oldinput[0] * filter_coefficients[1]; //b1
                accumulator += input * filter_coefficients[0]; //b0

                // Reorder biquad1 input history
                biquad1_oldinput[1] = biquad1_oldinput[0];
                biquad1_oldinput[0] = input;

                accumulator += biquad1_oldoutput[1] * filter_coefficients[4]; // a2
                accumulator += biquad1_oldoutput[0] * filter_coefficients[3]; // a1

                // Reorder biquad1 output history
                biquad1_oldoutput[1] = biquad1_oldoutput[0];
                biquad1_oldoutput[0] = accumulator;

                output = accumulator;
                
                // ******************* Filtering through biquad 2 *******************//
                // Resetting accumulator
                
                accumulator = 0;
                input = output;

                accumulator = biquad2_oldinput[1] * filter_coefficients[2]; //b2
                accumulator += biquad2_oldinput[0] * filter_coefficients[1]; //b1
                accumulator += input * filter_coefficients[0]; //b0

                // Reorder biquad1 input history
                biquad2_oldinput[1] = biquad2_oldinput[0];
                biquad2_oldinput[0] = input;

                accumulator += biquad2_oldoutput[1] * filter_coefficients[4]; // a2
                accumulator += biquad2_oldoutput[0] * filter_coefficients[3]; // a1

                // Reorder biquad1 output history
                biquad2_oldoutput[1] = biquad2_oldoutput[0];
                biquad2_oldoutput[0] = accumulator;

                *data16_it = (short)(accumulator*0.99);
                
            }
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