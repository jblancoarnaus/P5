#include <iostream>
#include <math.h>
#include "strings.h"
#include "keyvalue.h"
#include "wavfile_mono.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

Strings::Strings(const std::string &param)
    : adsr(SamplingRate, param)
{
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */
  KeyValue kv(param);
  if (!kv.to_int("N", N))
    N = 40; //default value
  if (!kv.to_float("ADSR_A", adsr_a))
    adsr_a = 0.01; //default value
  if (!kv.to_float("ADSR_S", adsr_s))
    adsr_s = 0.5; //default value
  if (!kv.to_float("ADSR_R", adsr_r))
    adsr_r = 0.1; //default value

  adsr.set(adsr_s, adsr_a, 0, adsr_r, 1.5F);  //create envelope with no decay state
  index = 0;
  
  std::string file_name;
  static string kv_null;
  int error = 0;
  if ((file_name = kv("file")) == kv_null)
  {
    //Create a tbl with one period of a sinusoidal wave
    tbl.resize(N);
    float phase = 0, step = 2 * M_PI / (float)N;
    index = 0;
    for (int i = 0; i < N; ++i)
    {
      tbl[i] = sin(phase);
      phase += step;
    }
  }
  else
  {
    unsigned int fm;
    error = readwav_mono(file_name, fm, tbl);
    if (error < 0)
    {
      cerr << "Error: no se puede leer el fichero " << file_name << " para un instrumento FicTabla" << endl;

      throw -1;
    }
    N = tbl.size();
  }
}

void Strings::command(long cmd, long note, long vel)
{
  if (cmd == 9)
  { //'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
    float f0note = pow(2, ((float)note - 69) / 12) * 440; //convert note in semitones to frequency (Hz)
    float Nnote = 1 / f0note * SamplingRate;              //obtain note period in samples
    index_step = (float)N / Nnote;                        //obtain step (relationship between table period and note period)
    if (vel > 127)
      vel = 127;

    A = vel / 127.;
  }
  else if (cmd == 8)
  { //'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0)
  { //Faster release, but don't end it abruptly
  
  adsr.set(adsr_s, adsr_a, 0, adsr_r/4, 1.5F);
  adsr.stop();
  }
}

const vector<float> &Strings::synthesize()
{
  if (not adsr.active())
  {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive)
    return x;
  unsigned int index_floor, next_index; //interpolation indexes
  float weight;                         //interpolation weights

  for (unsigned int i = 0; i < x.size(); ++i)
  {
    //check if the floating point index is out of bounds
    if (floor(index) > tbl.size() - 1)
      index = index - floor(index);

    //Obtain the index as an integer
    index_floor = floor(index);
    weight = index - index_floor;

    //fix interpolation indexes if needed
    if (index_floor == (unsigned int)N - 1)
    {
      next_index = 0;
      index_floor = N - 1;
    }
    else
    {
      next_index = index_floor + 1;
    }
    //interpolate table values
    x[i] = A * ((1 - weight) * tbl[index_floor] + (weight)*tbl[next_index]);

    if (adsr.active())
    {
      x[i] = x[i] * adsr_s;
    }
    //update real index
    index = index + index_step;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}