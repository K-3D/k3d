/* dturb.sl
 *
 * turbulence displacement
 */

#include "k3d_rmannotes.h"
#include "k3d_filterwidth.h"

displacement k3d_dturb(float Km = 0.1, freq = 10, flatness = 1)
{
  float magnitude, layer_mag;
  point PP;
  float width, cutoff, fade, f, turb, maxfreq = 16;

  /* compute turbulence */

  PP = transform("shader", P) * freq;

  width = filterwidthp(PP);
  cutoff = clamp(0.5 / width, 0, maxfreq);

  turb = 0;
  for (f = 1; f < 0.5 * cutoff; f *= 2) 
    turb += abs(snoise(PP * f)) / f;
  fade = clamp(2 * (cutoff - f) / cutoff, 0, 1);
  turb += fade * abs(snoise(PP * f)) / f;

  /* raise to power to create flat areas */

  magnitude = pow(turb, flatness);

  /* output */

  P += Km * magnitude * normalize(N);
  N = calculatenormal(P);
}
