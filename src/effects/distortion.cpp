#include <iostream>
#include <math.h>
#include "distortion.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

static float SamplingRate = 44100;

Distortion::Distortion(const std::string &param)
{

  KeyValue kv(param);

  if (!kv.to_float("t", t))
    t = 0.75; //default value
  if (!kv.to_float("tm", tm))
    tm = 0.01;

  tm = tm * SamplingRate;
}

void Distortion::command(unsigned int comm)
{
  if (comm == 0)
    t = 1;
}

void Distortion::operator()(std::vector<float> &x)
{
  float max, min;
  int window_count = 0;
  
  for (unsigned int i = 0; i < x.size(); i++)
  {

    //update maximum and minimum clipping value
    if (window_count == 0)
    {
      max = 0;
      min = 0;

      for (unsigned int j = i; j < i + tm; j++)
      {
        //quit search if the end of x has been reached
        if (j > x.size())
        {
          break;
        }

        if (max < x[j])
        {
          max = x[j];
        }
        if (min > x[j])
        {
          min = x[j];
        }
      }
    }
    //clip the signal if needed
    if (((max * t) < x[i]))
    {
      x[i] = max * t;
    }
    if (((min * t) > x[i]))
    {
      x[i] = min * t;
    }

    window_count++;
    if (window_count > tm)
    {
      window_count = 0;
    }
  }
}
