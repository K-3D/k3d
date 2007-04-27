/* fire.sl
 *
 * animated fire -- adpated from shader by Flip Phillips
 *
 */

#include "k3d_rmannotes.h"

surface k3d_fire(float frame = 1)
{
  color layer_color, surface_color;
  color surface_opac, layer_opac;
  float width, cutoff, fade, f, turb, maxfreq = 16;
  float flame;
  float ss, tt;
  color red = color (1, .3, .1);
  color orange = color (.95, .7, .05);
  color yellow = color (.95, .95, .1);
  color hot = color (1, 1, .8);

  surface_color = 0;
  surface_opac = 0;

  /* compress ss & offset both by factor of current frame */

  ss = s * 5 + frame * 0.01;
  tt = t + frame * 0.1;

  /* compute turbulence */

  width = max(filterwidth(ss), filterwidth(tt));
  cutoff = clamp(0.5 / width, 0, maxfreq);

  turb = 0;
  for (f = 1; f < 0.5 * cutoff; f *= 2) 
    turb += abs(snoise2(ss * f, tt * f)) / f;
  fade = clamp(2 * (cutoff - f) / cutoff, 0, 1);
  turb += fade * abs(snoise2(ss * f, tt * f)) / f;
  turb *= 0.5;

  /* index into color spline using turbulence */

  flame = clamp(t - turb, 0, 1);
  layer_opac = flame;
  layer_color = spline(flame, red, red, red, red, orange, yellow, hot, hot);
  surface_color = blend(surface_color, layer_color, layer_opac);
  surface_opac = union(surface_opac, layer_opac);

  /* output */

  Oi = surface_opac;
  Ci = surface_color;
}
