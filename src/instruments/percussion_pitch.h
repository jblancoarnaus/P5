#ifndef PERCUSSIONPITCH
#define PERCUSSIONPITCH

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class PercussionPitch: public upc::Instrument {
    EnvelopeADSR adsr;
    float index;
    int total_attack_length;   //total length of the attack+decline phase in samples
    int total_samples_played;  //counter to check how many samples have been played of a note
    int interrupted_count;     //counter when a note gets interrupted
    bool gotInterrupted;       //flag to keep track of interrupted notes
    int N;
	  float A, index_step, decay_constant;
    std::vector<float> tbl;
  public:
    PercussionPitch(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
