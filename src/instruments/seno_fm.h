#ifndef SENO_FM
#define SENO_FM

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc
{
  class SenoFM : public upc::Instrument
  {
    EnvelopeADSR adsr, adsr2;
    float index, N1, N2, setting, setting2;
    int N, decay_count, decay_count_I, note_int;
    float A,D, index_step, index_sen, Nnote,f0note,adsr_s,adsr_a,adsr_r,adsr_d,adsr_s2,adsr_a2,adsr_r2,adsr_d2, max_level;
    std::vector<float> tbl, x_tm;
    long double mod_phase, mod_phase_step;
    float fm, I1, I2;

  public:
    SenoFM(const std::string &param = "");
    void command(long cmd, long note, long velocity = 1);
    const std::vector<float> &synthesize();
    bool is_active() const { return bActive; }
  };
}

#endif
