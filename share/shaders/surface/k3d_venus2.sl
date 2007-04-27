/* I renamed the shader to MBVenus.sl -- tal@SpamSucks_cs.caltech.edu */

/*
 * venus.sl - surface for a very cloudy planet like Venus.
 *
 *
 * DESCRIPTION:
 *      When put on a sphere, sets the color to look like a densely
 *   clouded planet, very much like the real Venus appears in UV.
 *      The shader works by creating a fractal turbulence function over
 *   the surface to simulate the clouds.  Strong Coriolis forces are
 *   simulated to give the twisting of clouds that is typically seen
 *   on Venus.
 *
 *
 * PARAMETERS:
 *    Ka, Kd - the usual meaning
 *    offset, scale - control the linear scaling of the cloud value.
 *    twist - controls the twisting of the clouds due to Coriolis forces.
 *    omega - controls the fractal characteristics of the clouds
 *    octaves - the number of octaves of noise to sum for the clouds.
 *    radius - radius of planet
 *
 *
 * AUTHOR: Ken Musgrave.
 *    Conversion to Shading Language and minor modifications by Larry Gritz.
 *    Planet radius param added by Mark Beckwith.
 *
 * REFERENCES:
 *    _Texturing and Modeling: A Procedural Approach_, by David S. Ebert, ed.,
 *    F. Kenton Musgrave, Darwyn Peachey, Ken Perlin, and Steven Worley.
 *    Academic Press, 1994.  ISBN 0-12-228760-6.
 *
 *
 * HISTORY:
 *    ???? - Venus texture developed by F. Ken Musgrave.
 *    Feb 1994 - Conversion to Shading Language by L. Gritz
 *    Dec 1996 - Added radius of planet as a parameter by Mark Beckwith
 *        (mark@SpamSucks_intrig.com)
 */


#include "k3d_constants.h"
#include "k3d_noises.h"


surface
k3d_venus2 (float Ka = 1, Kd = 1;
       float offset = 1;
       float scale = 0.6;
       float twist = 0.22;
       float omega = 0.65;
       float octaves = 8;
       float radius = 1)
{
  point Ptexture;           /* the shade point in texture space */
  point PtN;                /* normalized version of Ptexture */
  point PP;                 /* Point after rotation by coriolis twist */
  float rsq;                /* Used in calculation of twist */
  float angle;              /* Twist angle */
  float sine, cosine;       /* sin and cos of angle */
  float l, o, a, i;         /* Loop control for fractal sum */
  float value;              /* Fractal sum is stored here */

  /* Transform to texture coordinates and map to nit sphere */
  Ptexture = transform ("shader", P) / radius;

  /* Calculate Coriolis twist, yielding point PP */
  PtN = normalize (Ptexture);
  rsq = xcomp(PtN)*xcomp(PtN) + ycomp(PtN)*ycomp(PtN);
  angle = twist * TWOPI * rsq;
  sine = sin (angle);
  cosine = cos (angle);
  PP = point (xcomp(Ptexture)*cosine - ycomp(Ptexture)*sine,
	      xcomp(Ptexture)*sine + ycomp(Ptexture)*cosine,
	      zcomp(Ptexture));

  /* Compute VLfBm */
  l = 1;  o = 1;  a = 0;
  for (i = 0;  i < octaves;  i += 1) {
      a += o * snoise (PP * l);
      l *= 2;
      o *= omega;
    }

  value = abs (offset + scale * a);

  /* Shade like matte, but with color scaled by cloud color */
  Oi = 1;
  Ci = Os * (value * Cs) * (Ka * ambient() +
			    Kd * diffuse(faceforward(normalize(N),I)));
}
