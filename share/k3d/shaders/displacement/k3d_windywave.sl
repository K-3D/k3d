#include "k3d_noises.h"

displacement k3d_windywave(float Km = 0.1; float txtscale = 1;
			   float windfreq = 0.5; float windamp = 1;
			   float minwind = 0.3)
{
  float offset;
  point PP;
  float wind;
  float turb, a, i;

  PP = txtscale * windfreq * transform("shader", P);

  offset = Km * (snoise(PP) + 0.5 * snoise(2 * PP));

  turb = 0;
  a = 1;
  PP *= 8;
  for(i = 0; i < 4; i += 1)
    {
      turb += abs(a * snoise(PP));
      PP *= 2;
      a /= 2;
    }
  wind = minwind + windamp * turb;

/*  P += wind * offset * normalize(N); */
  N = calculatenormal(P + wind * offset * normalize(N));
}
