/****************************************************************************
 * shiny.sl -- Shiny metal surface
 *
 * Parameters:
 *    Ka, Kd, Ks, roughness - The usual meaning
 *    Kr - coefficient for mirror-like reflections of environment
 *    blur - how blurry are the reflections? (0 = perfectly sharp)
 *    envname, envspace, envrad - controls for using environment maps
 *    rayjitter, raysamples - ray tracing controls for reflection
 *    twosided - if nonzero both sides of the surface are shiny, otherwise
 *        only the "outside" (where the surface normal points) will
 *        spawn rays.  This can be an important optimization, especially
 *        when using the "ray server."
 *
 ***************************************************************************
 *
 * Author: Larry Gritz (gritzl@acm.org).
 *
 * Reference:
 *   _Advanced RenderMan: Creating CGI for Motion Picture_, 
 *   by Anthony A. Apodaca and Larry Gritz, Morgan Kaufmann, 1999.
 *
 * $Revision: 1.2 $    $Date: 2006/03/08 16:01:22 $
 *
 ****************************************************************************/

#include "k3d_material.h"



surface k3d_shiny(float Ka = 1, Kd = 0.1, Ks = 1, roughness = 0.2;
		  float Kr = 0.8, blur = 0; DECLARE_DEFAULTED_ENVPARAMS;
		  float twosided = 0;)
{
  normal Nf = faceforward(normalize(N), I);
  Ci =
    MaterialShinyMetal(Nf, Cs, Ka, Kd, Ks, roughness, Kr, blur, twosided,
		       ENVPARAMS);
  Oi = Os;
  Ci *= Oi;
}
