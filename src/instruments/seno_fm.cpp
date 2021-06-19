#include <iostream>
#include <math.h>
#include "seno_fm.h"
#include "keyvalue.h"
#include "wavfile_mono.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

SenoFM::SenoFM(const std::string &param)
    : adsr(SamplingRate, param), adsr2(SamplingRate, param)
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

  //signal amplitude adsr
  if (!kv.to_float("ADSR_A", adsr_a))
    adsr_a = 0.1; //"Attack" adsr parameter

  if (!kv.to_float("ADSR_D", adsr_d))
    adsr_d = 0.05; //"Decay" adsr parameter

  if (!kv.to_float("ADSR_S", adsr_s))
    adsr_s = 0.5; //"Sustain" adsr parameter

  if (!kv.to_float("ADSR_R", adsr_r))
    adsr_r = 0.1; //"Release" adsr parameter

  //modulation index adsr
  if (!kv.to_float("ADSR_A2", adsr_a2))
    adsr_a2 = adsr_a; //"Attack" adsr parameter

  if (!kv.to_float("ADSR_D2", adsr_d2))
    adsr_d2 = adsr_d; //"Decay" adsr parameter

  if (!kv.to_float("ADSR_S2", adsr_s2))
    adsr_s2 = adsr_s; //"Sustain" adsr parameter

  if (!kv.to_float("ADSR_R2", adsr_r2))
    adsr_r2 = adsr_r; //"Release" adsr parameter

  if (!kv.to_float("max_level", max_level))
    max_level = 0.02; //maximum level of the signal

  if (!kv.to_float("I1", I1))
    I1 = 1; //modulation index 1

  if (!kv.to_float("I2", I2))
    I2 = 1; //modulation index 2

  if (!kv.to_float("N1", N1))
    N1 = 1; //default N1 value
  if (!kv.to_float("N2", N2))
    N2 = 1; //default N2 value
  if (!kv.to_float("setting", setting))
    setting = 0; //default setting (standard adsr == 0, exp >0, string adsr ==-1)

  adsr2.set(adsr_a2, adsr_d2, adsr_s2, adsr_r2, 1.5F);

  if (setting == -1)
  {
    adsr.set(adsr_a, 0, adsr_s, adsr_r, 1.5F);
    adsr2.set(adsr_a2, 0, adsr_s2, adsr_r2, 1.5F);
  }
  mod_phase = 0;
  index_sen = 0;

  index = 0;

  std::string file_name;
  static string kv_null;
  int error = 0;
  if ((file_name = kv("file")) == kv_null)
  {
   //Create a tbl with one period of a sinusoidal wave if there's no input file
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
    unsigned int sampfreq;
    error = readwav_mono(file_name, sampfreq, tbl);
    if (error < 0)
    {
      cerr << "Error: no se puede leer el fichero " << file_name << " para un instrumento FicTabla" << endl;
      throw -1;
    }
    N = tbl.size();
  }
}

void SenoFM::command(long cmd, long note, long vel)
{
  if (cmd == 9)
  { //'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    adsr2.start();
    float f0note = pow(2, ((float)note - 69) / 12) * 440; //convert note in semitones to frequency (Hz)
    Nnote = 1 / f0note * SamplingRate;                    //obtain note period in samples
    index_step = (float)N / Nnote;                        //obtain step (relationship between table period and note period)

    //reset counters/phase
    index = 0;
    index_sen = 0;
    mod_phase = 0;
    decay_count = 0;
    decay_count_I = 0;

    fm = f0note * N2 / N1;                         //modulating frequency
    mod_phase_step = 2 * M_PI * fm / SamplingRate; //step of the modulating sine I*sin(2*pi*fm/SamplingRate)

    note_int = round(Nnote);
    x_tm.resize(note_int);

    if (vel > 127)
      vel = 127;

    A = vel / 127.;
  }
  else if (cmd == 8)
  { //'Key' released: sustain ends, release begins
    adsr.stop();
    adsr2.stop();
  }
  else if (cmd == 0)
  { 
  //release faster, but don't end it abruptly
    adsr.set(adsr_s, adsr_a, adsr_d, adsr_r / 4, 1.5F);
    adsr.stop();
    adsr2.set(adsr_s2, adsr_a2, adsr_d2, adsr_r2 / 4, 1.5F);
    adsr2.stop();
  }
}

const vector<float> &SenoFM::synthesize()
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
  float weight, weight_fm;              //interpolation weights
  int index_floor_fm, next_index_fm;    //frequency interpolation weights
  std::vector<float> I_array(x.size()); //I is now a function of t

  //fill array with the user-input I value (constant)
  for (unsigned int i = 0; i < x.size(); i++)
  {
    I_array[i] = (I2 - I1);
    //apply exponential envelope if selected
    if (setting > 0)
      I_array[i] = I_array[i] * pow(setting, decay_count_I);
  }

  //apply time-varying function to the array
  if (setting <= 0)
    adsr2(I_array);

  for (unsigned int i = 0; i < x.size(); i++)
  {
    I_array[i] = (I1 + I_array[i]);
  }

  //fill x_tm with a period of the new signal
  for (unsigned int i = 0; i < (unsigned int)note_int; ++i)
  {

    //check if the floating point index is out of bounds
    if ((long unsigned int)floor(index) > tbl.size() - 1)
      index = index - floor(index);

    //Obtain the index as an integer
    index_floor = (int)floor(index);
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

    x_tm[i] = ((1 - weight) * tbl[index_floor] + (weight)*tbl[next_index]);
    //update real index
    index = index + index_step;
  }

  //modulate the signal
  for (unsigned int i = 0; i < x.size(); ++i)
  {
    //check if the floating point index is out of bounds
    if (index_sen < 0)
    {
      index_sen = Nnote + index_sen;
    }
    if ((int)floor(index_sen) > note_int - 1)
    {

      index_sen = index_sen - (note_int - 1);
    }
    //Obtain the index as an integer
    index_floor_fm = floor(index_sen);
    weight_fm = index_sen - index_floor_fm;

    //fix interpolation indexes if needed
    if (index_floor_fm == note_int - 1)
    {
      next_index_fm = 0;
      index_floor_fm = note_int - 1;
    }
    else
    {
      next_index_fm = index_floor_fm + 1;
    }
    //interpolate table values
    x[i] = A * ((1 - weight_fm) * x_tm[index_floor_fm] + weight_fm * (x_tm[next_index_fm]));

    //update real index (phase) and modulated phase
    index_sen = index_sen + 1 - I_array[i] * sin(mod_phase);
    mod_phase = mod_phase + mod_phase_step;
  }

  while (mod_phase > M_PI)
    mod_phase -= 2 * M_PI;

  //apply exponential or adsr envelope

  for (unsigned int i = 0; i < x.size(); i++)
  {
    if (setting != 0)
    {
      if (setting > 0) //decrease exponentially according to the setting
      {
        x[i] = x[i] * max_level * pow(setting, decay_count);
        decay_count++;
      }
      else if (adsr.active() && setting == -1)
      {
        x[i] = x[i] * adsr_s; //correct level so that the attack doesn't reach values over the sustain threshold
      }
    }
    else
      x[i] = x[i] * max_level; //correct max level so it doesn't saturate when overlapping multiple signals
  }
  if (setting <= 0)
    adsr(x); //apply envelope to x and update internal status of ADSR
  return x;
}
