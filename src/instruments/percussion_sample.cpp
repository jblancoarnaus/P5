#include <iostream>
#include <math.h>
#include "percussion_sample.h"
#include "keyvalue.h"
#include "wavfile_mono.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

PercussionSample::PercussionSample(const std::string &param)
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

  if (!kv.to_float("volume", volume))
    volume = 1; //default value

      if (!kv.to_int("interrupt", interrupt))
    interrupt = 0; //dont interrupt by default

  std::string file_name;
  static string kv_null;
  int error = 0;
  if ((file_name = kv("file")) == kv_null)
  {
    cerr << "Error: no se ha encontrado el campo con el fichero de la señal para un instrumento FicTabla" << endl;
    throw -1;
  }

  unsigned int fm;
  error = readwav_mono(file_name, fm, tbl);
  index = 0;
  if (error < 0)
  {
    cerr << "Error: no se puede leer el fichero " << file_name << " para un instrumento FicTabla" << endl;

    throw -1;
  }
  N = tbl.size();
}

void PercussionSample::command(long cmd, long note, long vel)
{
  if (cmd == 9)
  { //'Key' pressed: attack begins
    bActive = true;
    index = 0;
    total_samples_played = 0;
    gotInterrupted = false; //reset status for every new note. By default, it can't get interrupted (interrupt==0)
    interrupted_count = 0;  
    if (vel > 127)
      vel = 127;
    A = vel / 127.;
  }
}

const vector<float> &PercussionSample::synthesize()
{

  if (not bActive)
    return x;

  for (unsigned int i = 0; i < x.size(); ++i)
  {

    if (index < (unsigned int)N)
    {
      x[i] = volume * A * tbl[index++];

      if (total_samples_played != -1)
      {
        total_samples_played++;
      }

      if (gotInterrupted&&interrupt!=0) //check if it got interrupted and the settings allow it
      {
        x[i] = x[i] * pow(interrupt, (int)interrupted_count);
        interrupted_count++;
      }
    }
    else //fill signal with 0 if the sound has been fully played
    {
      for (long unsigned int j = i; j < x.size(); j++)
      {
        x[j] = 0;
      }
      bActive = false;
    }
  }

  return x;
}
