/* I took wave's lead and renamed brickant to DPBrickAnti.sl -- tal@SpamSucks_cs.caltech.edu */

/* 
 * brickant.sl
 *
 * AUTHOR: Darwyn Peachy
 *
 * REFERENCES:
 *    _Texturing and Modeling: A Procedural Approach_, by David S. Ebert, ed.,
 *    F. Kenton Musgrave, Darwyn Peachey, Ken Perlin, and Steven Worley.
 *    Academic Press, 1994.  ISBN 0-12-228760-6.
 */
 

#include "k3d_proctext.h"
#include "k3d_functions.h"

#define BRICKWIDTH      0.25
#define BRICKHEIGHT     0.08
#define MORTARTHICKNESS 0.02

#define BMWIDTH         (BRICKWIDTH+MORTARTHICKNESS)
#define BMHEIGHT        (BRICKHEIGHT+MORTARTHICKNESS)
#define MWF             (MORTARTHICKNESS*0.5/BMWIDTH)
#define MHF             (MORTARTHICKNESS*0.5/BMHEIGHT)

surface
k3d_brickanti(
    uniform float Ka = 1;
    uniform float Kd = 1;
    uniform color Cbrick = color (0.5, 0.15, 0.14);
    uniform color Cmortar = color (0.5, 0.5, 0.5);
     )
{
    color Ct;
    point Nf;
    float ss, tt, sbrick, tbrick, w, h;
    float scoord = s;
    float tcoord = t;
    float swidth, twidth;

    Nf = normalize(faceforward(N, I));

    ss = scoord / BMWIDTH;
    tt = tcoord / BMHEIGHT;

    if (mod(tt*0.5,1) > 0.5)
        ss += 0.5;  /* shift alternate rows */

    swidth = abs(Du(ss)*du) + abs(Dv(ss)*dv);
    twidth = abs(Du(tt)*du) + abs(Dv(tt)*dv);
    tbrick = floor(tt); /* which brick? */
    sbrick = floor(ss); /* which brick? */

#if 0
    /* This is the simple antialiasing with "boxstep" */
    ss -= sbrick;
    tt -= tbrick;

    w = boxstep(MWF-swidth,MWF,ss)
      - boxstep(1-MWF-swidth,1-MWF,ss);
    h = boxstep(MHF-twidth,MHF,tt)
      - boxstep(1-MHF-twidth,1-MHF,tt);

#else
    /* This is the preferred antialiasing using integrals. */
#define frac(x)        mod((x),1)
#define sintegral(ss)  (floor(ss)*(1-2*MWF) + \
                        max(0,frac(ss)-MWF))
#define tintegral(tt)  (floor(tt)*(1-2*MHF) + \
                        max(0,frac(tt)-MHF))

    w = (sintegral(ss+swidth) - sintegral(ss))/swidth;
    h = (tintegral(tt+twidth) - tintegral(tt))/twidth;
#endif

    Ct = mix(Cmortar, Cbrick, w*h);

    /* diffuse reflection model */
    Oi = Os;
    Ci = Os * Ct * (Ka * ambient() + Kd * diffuse(Nf));
}
