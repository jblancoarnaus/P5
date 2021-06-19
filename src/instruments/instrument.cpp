#include <iostream>
#include "instrument_dumb.h"
#include "seno.h"
#include "seno_fm.h"
#include "percussion.h"
#include "percussion_pitch.h"
#include "percussion_sample.h"
#include "strings.h"


/*
  For each new instrument:
  - Add the header in this file
  - Add the call to the constructor in get_instrument() (also in this file)
  - Add the source file to src/meson.build
*/

using namespace std;

namespace upc {
  Instrument * get_instrument(const string &name,
			      const string &parameters) {
    Instrument * pInst = 0;
    //    cout << name << ": " << parameters << endl;
    if (name == "InstrumentDumb") {
      pInst = (Instrument *) new InstrumentDumb(parameters);
    }
    if (name == "Seno") {
      pInst = (Instrument *) new Seno(parameters);
    }
    if (name == "SenoFM") {
      pInst = (Instrument *) new SenoFM(parameters);
    }
    if (name == "Percussion") {
      pInst = (Instrument *) new Percussion(parameters);
    }
    if (name == "PercussionPitch") {
      pInst = (Instrument *) new PercussionPitch(parameters);
    }
    if (name == "PercussionSample") {
      pInst = (Instrument *) new PercussionSample(parameters);
    }
    if (name == "Strings") {
      pInst = (Instrument *) new Strings(parameters);
    }
    return pInst;
  }
}
