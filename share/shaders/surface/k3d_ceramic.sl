/* Author & copyright unknown */

#include "k3d_material.h"

surface k3d_ceramic(float Ka = 1, Kd = 0.5, Ks = .5, roughness = 0.1;
		    float Kr = 1, blur = 0, eta = 1.5;
		    float specsharpness = 0.5;
		    DECLARE_DEFAULTED_ENVPARAMS;)
{
  normal Nf = faceforward(normalize(N), I);
  Ci = MaterialCeramic(Nf, Cs, Ka, Kd, Ks, roughness, specsharpness);
  Oi = Os;
  Ci *= Oi;
}
