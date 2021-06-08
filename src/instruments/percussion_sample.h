#ifndef PERCUSSIONSAMPLE
#define PERCUSSIONSAMPLE

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class PercussionSample: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index;
    int N;
	float A;
    std::vector<float> tbl;
  public:
    PercussionSample(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
