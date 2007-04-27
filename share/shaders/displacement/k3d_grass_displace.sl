/* 
 * grass.sl
 *
 * I took used some of RManNotes function to assist in
 * creating a random look on the grass texture.
 *
 * This is simply uses the noise function and a checkerboard
 * pattern.  The displacements are circular.
 *
 *   by Lawrence D. Chin, cs184-bo
 */

#include "k3d_rmannotes.h"

displacement
k3d_grass_displace ( float height = 1.0,
              fac = 1.0,
              size = 1.0,
	      size2 = 0.75; )
{

  float noifreq = 5;
  float noiscale = 0.4;
  float ss, tt;

  float noi = noise(s * noifreq, t * noifreq);
  ss = s + snoise(noi + 912) * noiscale;
  tt = t + snoise(noi + 333) * noiscale;

  float smod = mod(ss*fac,1),
        tmod = mod(tt*fac,1),
        x,y,z;

  if (smod < 0.5) {
    if (tmod < 0.5) 
      P += 0.5;
    else {
      y = (tmod - 0.5)*10;
      x = smod*10; 
      x = abs(x - 2.5);
      y = abs(y - 2.5);
      z = sqrt(pow(x,2) + pow(y,2));
      if (z <= size2 )
	P += N * height;
      else
        P += 0.5;
      }
  } else {
    if (tmod < 0.5) {
      x = (smod - 0.5)*10;
      y = tmod*10; 
      x = abs(x - 2.5);
      y = abs(y - 2.5);
      z = sqrt(pow(x,2) + pow(y,2));
      if (z <= size )
	P += N * height;
      else
        P += 0.5;
      } 
    else
      P += 0.5;
 }    
  N = calculatenormal(P);
}
