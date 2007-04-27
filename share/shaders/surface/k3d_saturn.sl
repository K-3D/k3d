/* This was terran.sl from Larry Gritz and Ken Musgrave.  But there isn't much of it left.
*/

/*
 * TLSaturn.sl - surface for an Saturn-like planet.
 *
 *
 * DESCRIPTION:
 *      When put on a sphere, sets the color to look like relatively
 *   Saturn-like.  The shader works by using a variety of fractal 
 *   turbulence techniques.
 *
 *
 * PARAMETERS:
 *    Ka, Kd - the usual meaning
 *    dist_scale - scaling for multifractal distortion
 *    lat_scale,  map_exp - control scaling of 
 *               terrain type by latitude
 *
 *
 * HINTS:
 *   Assumes being used on a sphere.  Haven't really tried it by using other
 *   than the default arguments.
 *
 * AUTHOR: Ken Musgrave.
 *    Conversion to Shading Language and minor modifications by Larry Gritz.
 *
 *
 * REFERENCES:
 *
 *
 * HISTORY:
 *    This started out as L. Gritz's terran.sl shader.  But I think he may be
 *        the only one that would recognize that it was.
 *    23 May 1995 - Changed name from terran.sl to TLSaturn.sl and
 *        Did major hacking to make it a Saturn-like shader by Tal Lancaster 
 *        tal@SpamSucks_cs.caltech.edu
 *
 * last modified 23 May 1995 by Tal
 */

surface
k3d_saturn (float Ka = .5, Kd = .7;
	  float dist_scale = .2;
	  float offset = 0;
	  float lat_scale = 0.95;
	  float map_exp = 0;)
{
  point PP;
  point PtN;
  float latitude;
  color Ct;
  point Ptexture;

  /* Do all shading in shader space */
  Ptexture = transform ("shader", P);
  PtN = normalize (Ptexture);      /* Version of Ptexture with radius 1 */

  /************************************************************************
   * Step 2: Assign a climite type, roughly by latitude.
   ************************************************************************/

  /* make climate symmetric about equator */
  latitude = abs (zcomp (PtN));
	
  if (map_exp > 0)
       latitude = lat_scale * pow(latitude,map_exp);
  else latitude *= lat_scale;

  	/* Color map for Saturn */
      Ct = spline (latitude,
		color (.73, .598, .266),
		color (.73, .598, .266),
		color (1, .63, .332),
		color (.598, .531, .266),
		color (.863, .664, .332),
		color (.797, .598, .332),
		color (.73, .598, .266),
		color (.461, .461, .461),
		color (.332, .199, .266),
		color (.332, .199, .266),
		color (.332, .199, .266));

  /* Shade using matte model */
  Oi = 1;
  Ci = Os * Ct * (Ka * ambient() + Kd * diffuse(faceforward(normalize(N),I)));
}
