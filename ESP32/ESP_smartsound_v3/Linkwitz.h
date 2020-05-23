#ifndef _ESPCONTROLLER_H_
#define _ESPCONTROLLER_H_

typedef struct LinkwitzCoefficient{

};

// 4. order Linkwitz Riley filter
class Linkwitz {
public:
  Linkwitz(float* lp_coefficients, float* hp_coefficients);
  uint32_t filter(short* buffer, uint32_t len);
  ~Linkwitz();
private:
  // Lowpass
  float lp_coefficients[5];
  float biquads_lp_oldinput[4];
  float biquads_lp_oldoutput[4];
  // Highpass
  float hp_coefficients[5];
  float biquads_hp_oldinput[4];
  float biquads_hp_oldoutput[4];
};

#endif
