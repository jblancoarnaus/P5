#include <iostream>
#include <math.h>
#include "percussion.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

Percussion::Percussion(const std::string &param)
    : adsr(SamplingRate, param)
{
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */
  KeyValue kv(param);
  int N;
  float adsr_a, adsr_d;

  if (!kv.to_int("N", N))
    N = 40; //default value
  if (!kv.to_float("ADSR_A", adsr_a))
    adsr_a = 0.01; //default value
  if (!kv.to_float("ADSR_D", adsr_d))
    adsr_d = 0.01; //default value

  total_attack_length = round((adsr_d + adsr_a) * (float)SamplingRate);
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

void Percussion::command(long cmd, long note, long vel)
{
  if (cmd == 9)
  { //'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
    total_samples_played = 0;
    gotInterrupted = false; //reset status for every new note

    interrupted_count = 0;
    A = vel / 127.;
  }
  else if (cmd == 8)
  { //'Key' released: sustain ends, release begins
    adsr.stop();
    gotInterrupted = true;
    interrupted_count = 0;
  }
  else if (cmd == 0)
  { //Sound extinguished without waiting for release to end
    adsr.end();
  }
}

const vector<float> &Percussion::synthesize()
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
    x[i] = A * tbl[index++];
    if (total_samples_played != -1)
    {
      total_samples_played++;
    }

    if (gotInterrupted)
    {
      x[i] = x[i] * pow(0.99935, (int)interrupted_count);
      interrupted_count++;
    }

    //printf("interrupted  i %d \n", interrupted_count);
    if (total_samples_played >= total_attack_length)
    {
      adsr.stop(); //begin release phase immediately
      total_samples_played = -1;
    }
    if (index == tbl.size())
      index = 0;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
