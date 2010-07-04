/*
 * castucco.sl -- dispacement shader for stucco.
 *
 * Description:
 *   I call this "castucco" because it's the stuff on the walls *everywhere*
 *   in Northern California.  I never really saw it on the East Coast,
 *   but in CA it's truly ubiquitous.
 * 
 * Parameters:
 *   freq - basic frequency of the texture
 *   Km - amplitude of the mesas.
 *   octaves - how many octaves of fBm to sum
 *   trough, peak - define the shape of the valleys and mesas of the stucco.
 *
 * $Revision: 1.1 $    $Date: 2006/02/25 20:11:53 $
 *
 */

#include "k3d_noises.h"
#include "k3d_displace.h"

displacement k3d_castucco(float freq = 1;
			  float Km = 0.2;
			  float octaves = 3;
			  float trough = -0.15, peak = 0.35)
{
  point Pshad;			/* Point to be shaded, in shader space */
  float fwidth;			/* Estimated change in P between image samples */
  float disp;			/* Amount to displace */

  /* Do texture calcs in "shader" space, get approximate filter size */
  Pshad = freq * transform("shader", P);
  fwidth = filterwidthp(Pshad);

  /* Compute some fractional Brownian motion */
  disp = fBm(Pshad, fwidth, 3, 2, 0.6);

  /* Threshold the fBm and scale it */
  disp = Km * smoothstep(trough, peak, disp);

  /* displace in shader space units */
  N = Displace(normalize(N), "shader", disp, 1);
}
