/*
 * wallpaper_2stripe.sl -- surface shader for double striped wall paper
 *
 * DESCRIPTION:
 *   Makes a double striped pattern appropriate for wall paper.  Stripes
 *   are shaded in s-t space, and the stripes are parallel to lines of
 *   equal s.  The background color is given by the surface color.
 *
 * 
 * PARAMETERS:
 *   Ka, Kd, Ks, roughness	the usual
 *   specularcolor
 *   bgcolor, stripecolor       color of background and stripes
 *   stripewidth                width of stripes, in s coordinates
 *   stripespacing              dist between sets of stripes, in s coordinates
 *
 *
 * ANTIALIASING:  should analytically antialias itself quite well.
 *
 * AUTHOR: written by Larry Gritz (email: lg@bmrt.org)
 *
 * $Revision: 1.1 $   $Date: 2006/02/25 20:11:54 $
 */


#include "k3d_patterns.h"
#include "k3d_material.h"

surface k3d_wallpaper_2stripe(float Ka = 0.5, Kd = 0.75, Ks = 0.25;
			      float roughness = 0.1;
			      color stripecolor = color "rgb"(1, 0.5, 0.5);
			      float stripewidth = 0.05;
			      float stripespacing = 0.5;)
{
  float ss = s / stripespacing - 0.5;
  float ds = filterwidth(ss);
  float edge = (1 - stripewidth);

  float stripe =
    (filteredpulsetrain(edge, 1, ss, ds) +
     filteredpulsetrain(edge, 1, ss + 2 * stripewidth, ds));

  color Ct = mix(Cs, stripecolor, stripe);

  normal Nf = faceforward(normalize(N), I);
  Ci = MaterialPlastic(Nf, Ct, Ka, Kd, Ks, roughness);
  Oi = Os;
  Ci *= Oi;
}
