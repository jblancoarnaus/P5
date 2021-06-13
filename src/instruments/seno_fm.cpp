#include <iostream>
#include <math.h>
#include "seno_fm.h"
#include "keyvalue.h"
#include "wavfile_mono.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

SenoFM::SenoFM(const std::string &param)
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

  // I is the maximum downward pitch shift in semitones
  // As the shift is sinusoidal, the maximum posible shift is 2 (one octave)
  // In this way, we prevent negative shifts
  if (!kv.to_float("I", I))
    I = 1; //default downward variation shift in semitones
  // Pass I in semitones to linear I
    //I = 1. - pow(2, -I / 12.);

  if (!kv.to_float("N1", N1))
    N1 = 3; //default value
  if (!kv.to_float("N2", N2))
    N2 = 2; //default value

  fase_sen = 0;
  fase_mod = 0;


  index = 0;

  std::string file_name;
  static string kv_null;
  int error = 0;
  if ((file_name = kv("file")) == kv_null)
  {
    cerr << "Error: no se ha encontrado el campo con el fichero de la señal para un instrumento FicTabla.\nUsando sinusoide por defecto..." << endl;
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
  if (cmd == 9){ 
    //'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
    float f0note = pow(2, ((float)note - 69) / 12) * 440; //convert note in semitones to frequency (Hz)
    float Nnote = 1 / f0note * SamplingRate;              //obtain note period in samples
    index_step = (float)N / Nnote;                        //obtain step (relationship between table period and note period)
    
    fm = f0note*N2/N1;
    inc_fase_mod = 2 * M_PI * fm / SamplingRate;

    if (vel > 127) vel = 127;

    A = vel / 127.;
  }
  else if (cmd == 8)
  { //'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0)
  { //Sound extinguished without waiting for release to end
    adsr.end();
    buffer.resize(0);
  }
  fase_mod = 0;
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

  //vibrato parameters
	std::vector<float> xout(x.size());
  unsigned int tot = 0;
  float xant, xpos, rho;
  unsigned int index_floor, next_index; //interpolation indexes
  float weight;   //interpolation weights
  for (unsigned int i = 0; i < x.size(); ++i)
  {
    //check if the floating point index is out of bounds
   if (floor(index) > tbl.size()-1){
     index = index-floor(index);
      
   }
      
    //Obtain the index as an integer
    index_floor = floor(index);
    weight = index - index_floor;

    //fix interpolation indexes if needed
    if (index_floor == (unsigned int)N-1)
    {
      next_index = 0;
      index_floor = N-1;
    }
    else
    {
      next_index = index_floor + 1;
    }
    //interpolate table values
    x[i] = A * ((1 - weight) * tbl[index_floor] + (weight)*tbl[next_index]);
    x[i] = A*tbl[round(index_step*index)];
    

    //update real index
    index = index + index_step;
  }
  	// Usamos resta en la modulación para garantizar que no nos quedaremos sin
	// muestras en el vector x: fase_sen += 1 - I * sin(fase_mod).
	//
	// En el fondo, esta trampa permite garantizar que el sistema es siempre causal...

	// Si el buffer no está vacío tomamos los valores de él hasta vaciarlo (o no).
	for (tot = 0; fase_sen < buffer.size() and tot < x.size(); tot++) {
		xant = buffer[(int) fase_sen];
		xpos = ((unsigned int) fase_sen < buffer.size() - 1 ? buffer[(int) fase_sen + 1] : x[0]);
		rho = fase_sen - (int) fase_sen;

		xout[tot] = xant + rho * (xpos - xant);

		fase_sen += 1 - I * sin(fase_mod);
		fase_mod += inc_fase_mod;
	}
	if (fase_sen > buffer.size()) {
		fase_sen -= buffer.size();
		buffer.resize(0);
	}
	else {
		buffer.erase(buffer.begin(), buffer.begin() + (int) fase_sen);
		fase_sen -= (int) fase_sen;
	}

	// Completamos la señal con muestras del vector actual
	while (tot < x.size()) {
		// Si podemos, interpolamos; si no, extrapolamos
		if (fase_sen < x.size() - 1) {
			xant = x[(int) fase_sen];
			xpos = x[(int) fase_sen + 1];
			rho = fase_sen - (int) fase_sen;
		}
		else {
			xant = x[(int) fase_sen - 1];
			xpos = x[(int) fase_sen];
			rho = fase_sen - (int) fase_sen + 1;
		}
		xout[tot] = xant + rho * (xpos - xant);

		if (++tot < x.size()) {
			fase_sen += 1 - I * sin(fase_mod);
			fase_mod += inc_fase_mod;
		}
	}

	// Guardamos los valores restantes de x en el buffer
	buffer.insert(buffer.end(), x.begin() + (int) fase_sen, x.end());
	fase_sen -= (int) fase_sen;

	while (fase_mod > M_PI) fase_mod -= 2 * M_PI;

	// Copimos los valores de xout en x
	x = xout;
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
