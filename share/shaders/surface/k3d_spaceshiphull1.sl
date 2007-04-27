#include "k3d_noises.h"


float fractalsum(point Q)
{
	float f;
	float value = 0;

	for(f = 1; f < 8; f *= 2)
		value += snoise(Q * f) / f;

	return value;
}



surface k3d_spaceshiphull1(
	float Ka = 1;
	float Kd = 1;
	float Ks = 0.25;
	float roughness = 0.8; color specularcolor = 1;
	float width = .1; float height = .06;
	float lumavary = .4;
	float rowvary = 3.0;
	float grime = 0.1)
{
  float ss, tt, splate, tplate, platespecular;
  color platecolor;
  point Nf;
  point V;

  // Figure out which row we're in  
  ss = s / width;
  tt = t / height;

  tplate = floor(tt);

  ss += rowvary * noise(tplate + 0.3);
  splate = floor(ss);

  // Calculate plate color
  platecolor = Cs - (lumavary * float noise(splate + 0.4, tplate + 0.5));

  // Add grime
  platecolor -= grime * fractalsum(P);

  // Add specular contribution
  platespecular = Ks * noise(splate + 0.6, tplate + 0.7);

  Nf = faceforward(normalize(N), I);
  V = normalize(-I);
  Oi = Os;
  Ci = Os * platecolor * (Ka * ambient() + Kd * diffuse(Nf)) + specularcolor * platespecular * specular(Nf, V, roughness);
}
