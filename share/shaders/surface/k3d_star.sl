/* I took wave's lead and renamed star to DPStar.sl -- tal@SpamSucks_cs.caltech.edu */

/* 
 * star.sl
 *
 * AUTHOR: Darwyn Peachy
 *
 * REFERENCES:
 *    _Texturing and Modeling: A Procedural Approach_, by David S. Ebert, ed.,
 *    F. Kenton Musgrave, Darwyn Peachey, Ken Perlin, and Steven Worley.
 *    Academic Press, 1994.  ISBN 0-12-228760-6.
 */
 
#include "k3d_proctext.h"

surface
k3d_star(
    uniform float Ka = 1;
    uniform float Kd = 1;
    uniform color starcolor = color (1.0000,0.5161,0.0000);
    uniform float npoints = 5;
    uniform float sctr = 0.5;
    uniform float tctr = 0.5;
     )
{
    point Nf = normalize(faceforward(N, I));
    color Ct;
    float ss, tt, angle, r, a, in_out;
    uniform float rmin = 0.07, rmax = 0.2;
    uniform float starangle = 2*PI/npoints;
    uniform point p0 = rmax*(cos(0),sin(0),0);
    uniform point p1 = rmin*
        (cos(starangle/2),sin(starangle/2),0);
    uniform point d0 = p1 - p0;
    point d1;
    
    ss = s - sctr; tt = t - tctr;
    angle = atan(ss, tt) + PI;
    r = sqrt(ss*ss + tt*tt);
    a = mod(angle, starangle)/starangle;
    
    if (a >= 0.5)
        a = 1 - a;
    d1 = r*(cos(a), sin(a),0) - p0;
    in_out = step(0, zcomp(d0^d1));
    Ct = mix(Cs, starcolor, in_out);
    
    /* diffuse ("matte") shading model */
    Oi = Os;
    Ci = Os * Ct * (Ka * ambient() + Kd * diffuse(Nf));
}
