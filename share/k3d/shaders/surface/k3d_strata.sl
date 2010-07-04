/*
 * strata.sl -- surface shader for sedimentary rock strata
 *
 * DESCRIPTION:
 *    Makes sedimentary rock strata, useful for rendering landscapes.
 *
 * PARAMETERS:
 *    Ka, Kd - the usual meaning
 *    txtscale - overall scaling factor for the texture
 *    zscale - scaling for the thickness of the layers
 *    turbscale - how turbulent the layers are
 *    offset - z offset for the pattern
 *    octaves - number of octaves of noise to sum for the turbulence
 *
 * ANTIALIASING:
 *    None.
 *
 *
 * AUTHOR:
 *    C language version by F. Kenton Musgrave
 *    Translation to Shading Language by Larry Gritz.
 *
 * REFERENCES:
 *    _Texturing and Modeling: A Procedural Approach_, by David S. Ebert, ed.,
 *    F. Kenton Musgrave, Darwyn Peachey, Ken Perlin, and Steven Worley.
 *    Academic Press, 1994.  ISBN 0-12-228760-6.
 *
 * HISTORY:
 *    ??? - original C language version by Ken Musgrave
 *    Apr 94 - translation to Shading Language by L. Gritz
 *
 * this file last updated 18 Apr 1994
 */

#include "k3d_noises.h"

surface k3d_strata(float Ka = 0.5, Kd = 1; float txtscale = 1;
		   float yscale = 2; float turbscale = 0.1; float offset = 0;
		   float octaves = 8;
  )
{
  color Ct;
  point PP;
  float cmap;
  float turb, i, freq;

  PP = txtscale * transform("shader", P);

  turb = 0;
  freq = 1;
  for(i = 0; i < octaves; i += 1)
    {
      turb += abs(snoise(PP * freq) / freq);
      freq *= 2;
    }

  cmap = yscale * ycomp(PP) + turbscale * turb - offset;
  Ct =
    color spline(mod(cmap, 1), color(166, 131, 70), color(166, 131, 70),
		 color(204, 178, 127), color(184, 153, 97), color(140, 114,
								  51),
		 color(159, 123, 60), color(204, 178, 127), color(230, 180,
								  80),
		 color(192, 164, 110), color(172, 139, 80), color(102, 76,
								  25),
		 color(166, 131, 70), color(201, 175, 124), color(181, 150,
								  94),
		 color(161, 125, 64), color(177, 145, 87), color(170, 136,
								 77),
		 color(197, 170, 117), color(180, 100, 50), color(175, 142,
								  84),
		 color(197, 170, 117), color(177, 145, 87), color(170, 136,
								  77),
		 color(186, 156, 100), color(166, 131, 70), color(188, 159,
								  104),
		 color(168, 134, 74), color(159, 123, 60), color(195, 167,
								 114),
		 color(175, 142, 84), color(161, 125, 64), color(197, 170,
								 117),
		 color(177, 145, 87), color(177, 145, 87)) / 255;

  /* Shade like matte, but with color scaled by cloudcolor and opacity */
  Oi = 1;
  Ci =
    Cs * Ct * (Ka * ambient() + Kd * diffuse(faceforward(normalize(N), I)));
}
