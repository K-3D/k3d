/****************************************************************************
 * oakplank.sl
 *
 * Description: makes procedural varnished wood planks.  The planks
 *    are projected onto the x-y plane, with the length aligned with
 *    the y axis.  The subpattern within each individual plank is just
 *    a shifted version of the oaktexture function from oak.h.
 *   
 * Parameters for the coordinate mapping: 
 *   shadingspace - space in which the pattern is laid out
 *   shadingfreq - overall scaling factor for the pattern
 *   Pref - if supplied, gives the reference pose
 *
 * Parameters for the pattern of the plank structure: 
 *   plankwidth, planklength - size of the planks
 *   groovewidth, grooveheight - width of the grooves between planks
 *   Cgroove - color of the grooves between the planks
 *   groovedepth - how far down do the grooves displace?
 *   edgewidth - how close to the plank border does the wood start to curl?
 *   varyhue, varysat, varylum - control plank-to-plank color variation
 * 
 * Parameters for the color and pattern of the wood grain: 
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
 *   Kr, blur, eta - reflection parameters for the tile
 *   envname, envspace, envrad - environment mapping controls
 *   rayjitter, raysamples - ray tracing controls
 *   varnishlump, arnishlumpfreq - amp & freq of lumpiness in the varnish
 *
 ***************************************************************************
 *
 * Author: Larry Gritz, 1999
 *
 * Contacts:  lg@pixar.com
 *
 * $Revision: 1.2 $    $Date: 2006/03/08 16:01:22 $
 *
 ****************************************************************************/


#include "k3d_project.h"
#include "k3d_pshad.h"
#include "k3d_material.h"
#include "k3d_noises.h"
#include "k3d_displace.h"
#include "k3d_patterns.h"

#include "k3d_oak.h"


/* Given 2-D texture coordinates ss,tt, filter widths ds, dt, and the
 * width and height of the grooves between tiles, figure out which
 * (integer indexed) plank we are on and what coordinates within our
 * individual plank we are shading.
 */
float plankpattern(float ss, tt, ds, dt;
		   float plankwidth, planklength;
		   float groovewidth, grooveheight;
		   output float swhichplank, twhichplank;
		   output float splank, tplank;)
{
  /* Find which s plank we're on and our s coordinate within it */
  swhichplank = floor(ss / plankwidth);
  splank = ss - swhichplank * plankwidth;
  /* Shift in t a random amount for each plank column */
  float newt = tt + planklength * cellnoise(swhichplank);
  /* Find which t plank we're on and our t coordinate within it */
  twhichplank = floor(newt / planklength);
  tplank = newt - twhichplank * planklength;
  /* Calculate our "in-plank" value by multiplying two perpendicular
   * filtered pulsetrain functions.
   */
  return filteredpulsetrain(groovewidth, plankwidth, ss + groovewidth / 2,
			    ds) * filteredpulsetrain(grooveheight,
						     planklength,
						     newt + grooveheight / 2,
						     dt);
}



surface k3d_oakplank(float Ka = 1, Kd = 1, Ks = .75, roughness = 0.1;
		     float Kr = 1, blur = 0, eta = 1.5;
		     DECLARE_DEFAULTED_ENVPARAMS;
		     DEFAULT_PSHAD_PARAMS;
		     float ringfreq = 8, ringunevenness = 0.5;
		     float ringnoise = 0.02, ringnoisefreq = 1;
		     float grainfreq = 25;
		     float trunkwobble = 0.15, trunkwobblefreq = 0.025;
		     float angularwobble = 1, angularwobblefreq = 1.5;
		     float divotdepth = 0.012, truedisp = 0;
		     color Clightwood = color(.5, .2, .067);	/* light wood color */
		     color Cdarkwood = color(0.15, 0.077, 0.028);
		     color Cgroove = color(0.02, 0.02, 0.02);
		     float ringy = 1, grainy = 1;
		     float plankwidth = 2, planklength = 30;
		     float groovewidth = 0.05, grooveheight = 0.05;
		     float varyhue = 0.015, varysat = 0.1, varylum = 0.5;
		     float groovedepth = 0.03, edgewidth = 0.1;
		     float varnishlump = 0.01, varnishlumpfreq = 0.5;
  )
{
  GET_PSHAD;
  float ss = xcomp(Pshad), tt = ycomp(Pshad), height = zcomp(Pshad);
  float dss = filterwidth(ss), dtt = filterwidth(tt);

  /*
   * Find out where in the pattern we are: which plank we're on, and
   * the (splank,tplank) coordinates (both on [0,1]) within our tile.
   */
  float swhichplank, twhichplank, splank, tplank;
  float inplank = plankpattern(ss, tt, dss, dtt, plankwidth, planklength,
			       groovewidth, grooveheight,
			       swhichplank, twhichplank, splank, tplank);
  float plankindex = swhichplank + 13 * twhichplank;
  point Ppat =
    point(splank - 0.5, height - 0.01 * tplank, tplank) + vector(1, 5,
								 10) *
    (vector cellnoise(swhichplank, twhichplank) - 0.5);

  float wood = oaktexture(Ppat, dPshad, ringfreq, ringunevenness, grainfreq,
			  ringnoise, ringnoisefreq, trunkwobble,
			  trunkwobblefreq, angularwobble,
			  angularwobblefreq, ringy, grainy);
  color Cwood = mix(Clightwood, Cdarkwood, wood);

  Cwood = varyEach(Cwood, plankindex, varyhue, varysat, varylum);
  Cwood = mix(Cgroove, Cwood, inplank);

  /* Displacement: the edges of the planks displace down a bit, as do
   * the grooves between planks. 
   */
  float edgedisp = smoothpulse(0, edgewidth, plankwidth - edgewidth,
			       plankwidth, splank);
  edgedisp *=
    smoothpulse(0, edgewidth, planklength - edgewidth, planklength, tplank);
  normal Nf = faceforward(normalize(N), I);
  float disp = -wood * divotdepth + groovedepth * (edgedisp - 1);
  disp +=
    varnishlump * filteredsnoise(Pshad * varnishlumpfreq,
				 dPshad * varnishlumpfreq);
  Nf = faceforward(Displace(Nf, "shader", disp, truedisp), I);

  /* Illumination model
   * Less specular in the grooves, more specular in the dark wood. 
   */
  float specadjusted = 1 + .3 * wood - 0.8 * (1 - inplank);
  Ci =
    MaterialShinyPlastic(Nf, Cwood, Ka, Kd, specadjusted * Ks, roughness,
			 specadjusted * Kr, blur, eta, 1, ENVPARAMS);
  Oi = Os;
  Ci *= Oi;
}
