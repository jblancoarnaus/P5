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
    adsr.start();
    index = 0;
    if (vel > 127)
      vel = 127;
    A = vel / 127.;
  }
}

const vector<float> &PercussionSample::synthesize()
{

  if (not adsr.active())
  {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive)
    return x;

  for (unsigned int i = 0; i < x.size(); ++i)
  {

    if (index < (unsigned int)N)
    {
      x[i] = A * tbl[index++];
    }
    else //fill signal with 0 if the sound has been fully played
    {
      for(long unsigned int j=i;j<x.size();j++){
        x[j] = 0;
      }
      adsr.end();
      bActive=false;
    }
  }

  return x;
}
