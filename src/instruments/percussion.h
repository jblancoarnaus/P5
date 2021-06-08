#ifndef PERCUSSION
#define PERCUSSION

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class Percussion: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index;
    int total_attack_length;   //total length of the attack+decline phase in samples
    int total_samples_played;  //counter to check how many samples have been played of a note
    int interrupted_count;     //counter when a note gets interrupted
    bool gotInterrupted;       //flag to keep track of interrupted notes
	float A;
    std::vector<float> tbl;
  public:
    Percussion(const std::string &param = "");
    void command(long cmd, long note, long velocity=1); 
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;} 
  };
}

#endif
