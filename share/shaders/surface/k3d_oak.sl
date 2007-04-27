/****************************************************************************
 * oak.sl
 *
 * Description: makes procedural solid texture that looks very much like
 *    wood grain.  The rings surround the z axis, so to position the
 *    pattern, one should translate the shadingspace (which defaults to
 *    "shader").  This makes a fairly plain, unfinished wood, that looks
 *    very much like oak.
 *   
 * Parameters for the coordinate mapping: 
 *   shadingspace - space in which the pattern is laid out
 *   shadingfreq - overall scaling factor for the pattern
 *   Pref - if supplied, gives the reference pose
 *
 * Parameters for the color and pattern: 
 *   Clightwood - the light, "background" wood color
 *   Cdarkwood - the darker color in the ring/grain
 *   ringfreq - mean frequency of ring spacing
 *   ringunevenness - 0=equally spaced rings, larger is unequally spaced
 *   grainfreq - frequency of the fine grain
 *   ringnoise, ringnoisefreq - general warping of the domain
 *   trunkwobble, trunkwobblefreq - controls noise which wobbles the
 *       axis of the trunk so that it's not perfectly on the z axis.
 *   angularwobble, angularwobblefreq - warping indexed by angle about
 *       the z axis.
 *   ringy, grainy - overall scale on the degree to which rings and
 *       grain are weighted.  0 turns one off, 1 makes full effect.
 *   divotdepth - depth (in shader units) of the displacement due to
 *       ring or grain.
 *   truedisp - 1 for true displacement, 0 for bump mapping
 *
 * Parameters for illumination model:
 *   Ka, Kd, Ks, roughness - the usual meaning
 *   
 ***************************************************************************
 *
 * Author: Larry Gritz, 1999
 *
 * Contacts:  lg@pixar.com
 *
 * $Revision: 1.1 $    $Date: 2006/02/25 20:11:54 $
 *
 ****************************************************************************/

#include "k3d_project.h"
#include "k3d_pshad.h"
#include "k3d_material.h"
#include "k3d_displace.h"

#include "k3d_oak.h"



surface k3d_oak(float Ka = 1, Kd = 1, Ks = .25, roughness = 0.2;
		DEFAULT_PSHAD_PARAMS;
		float ringfreq = 8, ringunevenness = 0.5;
		float ringnoise = 0.02, ringnoisefreq = 1;
		float grainfreq = 25;
		float trunkwobble = 0.15, trunkwobblefreq = 0.025;
		float angularwobble = 1, angularwobblefreq = 1.5;
		float divotdepth = 0.05;
		color Clightwood = color(.5, .2, .067);	/* light wood color */
		color Cdarkwood = color(0.15, 0.077, 0.028);
		float ringy = 1, grainy = 1;
		float truedisp = 0;
  )
{
  GET_PSHAD;
  normal Nf = faceforward(normalize(N), I);

  float wood;
  wood =
    oaktexture(Pshad, dPshad, ringfreq, ringunevenness, grainfreq, ringnoise,
	       ringnoisefreq, trunkwobble, trunkwobblefreq, angularwobble,
	       angularwobblefreq, ringy, grainy);
  color Cwood = mix(Clightwood, Cdarkwood, wood);
  Nf = faceforward(Displace(Nf, "shader", -wood * divotdepth, truedisp), I);

  /* Illumination model - just use plastic */
  Ci = MaterialPlastic(Nf, Cwood, Ka, Kd, Ks * (1 - 0.5 * wood), roughness);
  Oi = Os;
  Ci *= Oi;
}
