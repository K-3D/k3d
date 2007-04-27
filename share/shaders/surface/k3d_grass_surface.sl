/* 
 * Again, similar structure of grass.sl
 *
 *  by Lawrence D. Chin, cs184-bo
 */

#include "k3d_rmannotes.h"

surface
k3d_grass_surface ( float height = 1.0, Ka=0.5, Kd=0.1, Ks= 1, roughness = 0.25,
              fac = 1.0,
              size = 1.0,
	      size2 = 0.75; )
{

  color color1 = color(0,1,0), color2 = color(0.5,0.5,0);
  point Nf = faceforward(normalize(N),I);

  float noifreq = 5;
  float noiscale = 0.4;
  float ss, tt;

  float noi = noise(s * noifreq, t * noifreq);
  ss = s + snoise(noi + 912) * noiscale;
  tt = t + snoise(noi + 333) * noiscale;

  float smod = mod(ss*fac,1),
        tmod = mod(tt*fac,1);

  if (smod > tmod) {
    if (smod < 0.5) {
      float d, fuzz = 0.025;
      point center = (0.5, 0.5, 0);  /* location of center of disk */
      float radius = 0.35;           /* radius of disk */
      d = distance(center,(s,t,0));
      color layer_opac = 1 - smoothstep(radius - fuzz, radius, d);
      Ci = blend(color1, color2, layer_opac);
    } else     
      Ci = color1;
  }
  else 
    Ci = color1;
  

  Oi = 1;
  Ci = Ci * (Ka *ambient() + Kd * diffuse(Nf) + Ks* specular(Nf,-I,roughness));
}

