#ifndef STRINGS
#define STRINGS

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class Strings: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index;
    int N;
  	float A, index_step;
    std::vector<float> tbl;
  public:
    Strings(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
