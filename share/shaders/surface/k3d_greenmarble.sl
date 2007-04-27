/*
 * greenmarble.sl -- RenderMan compatible shader for green veined marble.
 *
 * DESCRIPTION:
 *   Makes a marble-like surface using a turbulence function.
 *   
 * PARAMETERS:
 *   Ka, Kd, Ks, roughness, specularcolor - work just like the plastic
 *   txtscale - overall scaling for the texture
 *   darkcolor, lightcolor - colors of the underlying substrate
 *   veincolor - color of the bright veins
 *   veinfreq - controls the frequency of the veining effects
 *   sharpness - how sharp the veins appear
 *
 *
 * AUTHOR: Larry Gritz, 1994
 *
 */

#include "k3d_noises.h"
#include "k3d_rayserver.h"
#include "k3d_material.h"


surface k3d_greenmarble(float Ka = 0.1, Kd = 0.6, Ks = 0.4, roughness = 0.1;
			color specularcolor = 1;
			color darkcolor = color(0.01, 0.12, 0.004);
			color lightcolor = color(0.06, 0.18, 0.02);
			color veincolor = color(0.47, 0.57, 0.03);
			float veinfreq = 1;
			float sharpness = 25; float txtscale = 1;
  )
{
  color Ct;
  float freq;
  float turbsum, turb, i;

  point PP = txtscale * transform("shader", P);
  float dPP = filterwidthp(PP);

  /*
   * First calculate the underlying color of the substrate
   *    Use turbulence - use frequency clamping
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
  for(i = 0; i < 3; i += 1)
    {
      turb = abs(filteredsnoise(PP * freq, dPP * freq));
      turb = pow(smoothstep(0.8, 1, 1 - turb), sharpness) / freq;
      turbsum += (1 - turbsum) * turb;
      freq *= 2;
    }
  turbsum *=
    smoothstep(-0.1, 0.05, snoise(2 * (PP + vector(-4.4, 8.34, 27.1))));

  Ct = mix(Ct, veincolor, turbsum);

  /*
   * Shade like plastic
   */
  normal Nf = faceforward(normalize(N), I);
  Ci = MaterialPlastic(Nf, Ct, Ka, Kd, Ks, roughness);
  Oi = Os;
  Ci *= Os;
}
