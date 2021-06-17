#ifndef PERCUSSIONSAMPLE
#define PERCUSSIONSAMPLE

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc
{
  class PercussionSample : public upc::Instrument
  {
    EnvelopeADSR adsr;
    unsigned int index;
    int N;
    float A, volume;
    int total_samples_played; //counter to check how many samples have been played of a note
    int interrupted_count;    //counter when a note gets interrupted
    bool gotInterrupted;      //flag to keep track of interrupted notes
    int interrupt;            //==0 dont interrupt, ==constant interrupt with that decay constant
    std::vector<float> tbl;

  public:
    PercussionSample(const std::string &param = "");
    void command(long cmd, long note, long velocity = 1);
    const std::vector<float> &synthesize();
    bool is_active() const { return bActive; }
  };
}

#endif
