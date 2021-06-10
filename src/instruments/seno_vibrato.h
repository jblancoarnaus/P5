#ifndef SENO_VIBRATO
#define SENO_VIBRATO

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc
{
  class SenoVibrato : public upc::Instrument
  {
    EnvelopeADSR adsr;
    float index;
    int N;
    float A, index_step;
    std::vector<float> tbl;
    long double fase_mod, inc_fase_mod;
    long double fase_sen, inc_fase_sen;
    std::vector<float> buffer;
    float fm, I;

  public:
    SenoVibrato(const std::string &param = "");
    void command(long cmd, long note, long velocity = 1);
    const std::vector<float> &synthesize();
    bool is_active() const { return bActive; }
  };
}

#endif
