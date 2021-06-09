#include <iostream>
#include <math.h>
#include "distortion.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

static float SamplingRate = 44100;

Distortion::Distortion(const std::string &param) {
  fase = 0;

  KeyValue kv(param);

  if (!kv.to_float("A", A))
    A = 0.5; //default value

  if (!kv.to_float("fm", fm))
    fm = 10; //default value

  inc_fase = 2 * M_PI * fm / SamplingRate;
}

void Distortion::command(unsigned int comm) {
  if (comm == 1) fase = 0;
}

void Distortion::operator()(std::vector<float> &x){
  for (unsigned int i = 0; i < x.size(); i++) {
    x[i] *= ((2 - A) + A * sin(fase)) / 2;
    fase += inc_fase;

    while(fase > 2 * M_PI) fase -= 2 * M_PI;
  }
}
