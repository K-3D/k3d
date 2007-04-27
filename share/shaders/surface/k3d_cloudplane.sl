/* I took wave's lead and renamed cloudplane to DPCloudplane.sl -- tal@SpamSucks_cs.caltech.edu */

/* 
 * cloudplane.sl
 *
 * AUTHOR: Darwyn Peachy
 *
 * REFERENCES:
 *    _Texturing and Modeling: A Procedural Approach_, by David S. Ebert, ed.,
 *    F. Kenton Musgrave, Darwyn Peachey, Ken Perlin, and Steven Worley.
 *    Academic Press, 1994.  ISBN 0-12-228760-6.
 */
 
#include "k3d_proctext.h"

#define NTERMS 5

surface
k3d_cloudplane(
    color cloudcolor = color (1,1,1);
    )
{
    color Ct;
    point Psh;
    float i, amplitude, f;
    float x, fx, xfreq, xphase;
    float y, fy, yfreq, yphase;
    uniform float offset = 0.5;
    uniform float xoffset = 13;
    uniform float yoffset = 96;
    
    Psh = transform("shader", P);
    x = xcomp(Psh) + xoffset;
    y = ycomp(Psh) + yoffset;

    xphase = 0.9; /* arbitrary */
    yphase = 0.7; /* arbitrary */
    xfreq = 2 * PI * 0.023;
    yfreq = 2 * PI * 0.021;
    amplitude = 0.3;
    f = 0;
    for (i = 0; i < NTERMS; i += 1) {
        fx = amplitude *
            (offset + cos(xfreq * (x + xphase)));
        fy = amplitude *
            (offset + cos(yfreq * (y + yphase)));
        f += fx * fy;
        xphase = PI/2 * 0.9 * cos(yfreq * y);
        yphase = PI/2 * 1.1 * cos(xfreq * x);
    
        xfreq *= 1.9 + i * 0.1; /* approximately 2 */
        yfreq *= 2.2 - i * 0.08; /* approximately 2 */
        amplitude *= 0.707;
    }
    f = clamp(f, 0, 1);

    Ct = mix(Cs, cloudcolor, f);
    Oi = Os;
    Ci = Os * Ct;
}
