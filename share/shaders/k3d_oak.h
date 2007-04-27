/****************************************************************************
 * oak.h - contains the oaktexture function, which is common to many
 *         shader which make wood grain patterns.  It looks very much
 *         like oak, and can also pass for ...
 *
 ***************************************************************************
 *
 * Author: Larry Gritz, 1999
 *
 * Contacts:  lg@pixar.com
 *
 * $Revision: 1.1 $    $Date: 2004/05/19 18:15:19 $
 *
 ****************************************************************************/

#include "k3d_noises.h"


/* oaktexture -- generate a pattern much like the rings and grain of
 *    oak wood.  Rings are centered on the z axis, so the caller should
 *    position the space of Pshad so that the grain goes the way you want.
 *    Note: this function takes derivatives, so it shouldn't be called 
 *    from within varying conditionals or loops.
 * Inputs:
 *    Pshad, dPshad - sample point & its derivative
 *    ringfreq - mean frequency of ring spacing
 *    ringunevenness - 0=equally spaced rings, larger is unequally spaced
 *    grainfreq - frequency of the fine grain
 *    ringnoise, ringnoisefreq - general warping of the domain
 *    trunkwobble, trunkwobblefreq - controls noise which wobbles the
 *        axis of the trunk so that it's not perfectly on the z axis.
 *    angularwobble, angularwobblefreq - warping indexed by angle about
 *        the z axis.
 *    ringy, grainy - overall scale on the degree to which rings and
 *        grain are weighted.  0 turns one off, 1 makes full effect.
 * Return value: 0 when in the "background" light wood, 1 when in the
 *    darkest part of a ring or grain.
 */
float
oaktexture (point Pshad;  float dPshad;
	    float ringfreq, ringunevenness, grainfreq;
	    float ringnoise, ringnoisefreq;
	    float trunkwobble, trunkwobblefreq;
	    float angularwobble, angularwobblefreq;
	    float ringy, grainy;)
{
    /* We shade based on Pshad, but we add several layers of warping: */
    /* Some general warping of the domain */
    vector offset = vfBm(Pshad*ringnoisefreq,dPshad*ringnoisefreq, 2, 4, 0.5);
    point Pring = Pshad + ringnoise*offset;
    /* The trunk isn't totally steady xy as you go up in z */
    Pring += trunkwobble * 
               vsnoise(zcomp(Pshad)*trunkwobblefreq) * vector(1,1,0);

    /* Calculate the radius from the center. */
    float r2 = sqr(xcomp(Pring)) + sqr(ycomp(Pring));
    float r = sqrt(r2) * ringfreq;
    /* Add some noise around the trunk */
    r += angularwobble * smoothstep(0,5,r)
	   * snoise (angularwobblefreq*(Pring)*vector(1,1,0.1));

    /* Now add some noise so all rings are not equal width */
    extern float du, dv;
    float dr = filterwidth(r);
    r += ringunevenness*filteredsnoise(r,dr);

    float inring = smoothpulsetrain (.1, .55, .7, .95, 1, r);

    point Pgrain = Pshad*grainfreq*vector(1,1,.05);
    float dPgrain = filterwidthp(Pgrain);
    float grain = 0;
    float i, amp=1;
    for (i = 0;  i < 2;  i += 1) {
	float grain1valid = 1-smoothstep(.2,.6,dPgrain);
	if (grain1valid > 0) {
	    float g = grain1valid * snoise (Pgrain);
	    g *= (0.3 + 0.7*inring);
	    g = pow(clamp(0.8 - (g),0,1),2);
	    g = grainy * smoothstep (0.5, 1, g);
	    if (i == 0)
		inring *= (1-0.4*grain1valid);
	    grain = max (grain, g);
	}
	Pgrain *= 2;
	dPgrain *= 2;
	amp *= 0.5;
    }

    return mix (inring*ringy, 1, grain);
}

