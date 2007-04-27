/*
 * gmarbtile_polish.sl -- polished green marble tiles
 *
 * DESCRIPTION:
 *   Makes the same marble texture as greenmarble, but polished (with ray
 *   traced reflections) and cut into tiles (divided in xy texture space).
 * 
 * PARAMETERS:
 *   Ka, Kd, Ks, roughness, specularcolor - work just like the plastic
 *   Kr - reflectivity (shininess) of the surface
 *   txtscale - overall scaling for the texture
 *   darkcolor, lightcolor - colors of the underlying substrate
 *   veincolor - color of the bright veins
 *   veinfreq - controls the frequency of the veining effects
 *   sharpness - how sharp the veins appear
 *   groovecolor - the color of the grooves between the tiles.
 *   groovewidth - the width of the grooves
 *   tilesize - how big each tile is
 *
 * AUTHOR: written by Larry Gritz
 */

#include "k3d_noises.h"
#include "k3d_rayserver.h"
#include "k3d_reflections.h"


surface k3d_gmarbtile_polish(float Ka = 0.5, Kd = 0.4, Ks = 0.2;
			     float Kr = 0.2, roughness = 0.05;
			     color specularcolor = 1;
			     float txtscale = 1;
			     color darkcolor = color(0.01, 0.12, 0.004);
			     color lightcolor = color(0.06, 0.18, 0.02);
			     color veincolor = color(0.47, 0.57, 0.03);
			     color groovecolor = color(.02, .02, .02);
			     float veinfreq = 1;
			     float sharpness = 25;
			     float tilesize = 1;
			     float groovewidth = 0.015;
			     string envname = "", envspace = "NDC";
			     uniform float envrad = 100, rayjitter =
			     0, raysamples = 1;
			     float blur = 0;
  )
{
  vector offset;
  vector refldir;
  color Ct, env;
  float turbsum, turb;
  uniform float i, freq;
  float whichs, whicht;

  point PP = txtscale * transform("shader", P);
  float dPP = filterwidthp(PP);

  float ss = xcomp(PP) / tilesize;
  float dss = filterwidth(ss);
  float tt = ycomp(PP) / tilesize;
  float dtt = filterwidth(tt);

  float groovy = tilepattern(ss, tt, dss, dtt, groovewidth, groovewidth,
			     whichs, whicht, ss, tt);

  offset = vector(7 * whichs, 15 * whicht, 0 /*-23*floor(zcomp(PQ))*/ );
  PP += offset;

  /*
   * First calculate the underlying color of the substrate
   */
  turb = 0.5 * turbulence(PP, dPP, 5, 2, 0.5);
  Ct = mix(darkcolor, lightcolor, smoothstep(0.1, .35, turb));

  /*
   * Now we layer on the veins
   */

  /* perturb the lookup */
  PP += vector(35.2, -21.9, 6.25) + 0.5 * vfBm(PP, dPP, 6, 2, 0.5);

  /* Now calculate the veining function for the lookup area */
  turbsum = 0;
  freq = 1;
  PP *= veinfreq;
  dPP *= veinfreq;
  for(i = 0; i < 3; i += 1)
    {
      turb = abs(filteredsnoise(PP * freq, dPP * freq));
      turb = pow(smoothstep(0.8, 1, 1 - turb), sharpness) / freq;
      turbsum += (1 - turbsum) * turb;
      freq *= 2;
    }
  turbsum *=
    smoothstep(-0.1, 0.05,
	       filteredsnoise(2 * (PP + vector(-4.4, 8.34, 27.1)), 2 * dPP));

  Ct = mix(Ct, veincolor, turbsum);

  Ct = mix(groovecolor, Ct, groovy);

  normal Nf = faceforward(normalize(N), I);
  vector V = normalize(I);

  env = Ks * specular(Nf, -V, roughness);
  env += SampleEnvironment(P, reflect(V, Nf), Kr, blur, ENVPARAMS);
  env *= groovy * specularcolor;

  Oi = Os;
  Ci = Oi * (Ct * (Ka * ambient() + Kd * diffuse(Nf)) + env);
}
