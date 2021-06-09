#ifndef DISTORTION_H
#define DISTORTION_H

#include <vector>
#include <string>
#include "effect.h"

namespace upc {
  class Distortion: public upc::Effect {
    private:
      float fase, inc_fase;
	  float	fm, A;
    public:
      Distortion(const std::string &param = "");
	  void operator()(std::vector<float> &x);
	  void command(unsigned int);
  };
}

#endif
