/*
 * luna.sl -- surface shader for the moon
 *
 * DESCRIPTION:
 *    Makes a surface that looks sort of like Earth's moon.  It doesn't really
 *    have craters, so it isn't good for closeups.  But it's pretty good at about
 *    the scale for human naked-eye viewing from earth.
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
#include "k3d_functions.h"
#include "k3d_noises.h"
#include "k3d_constants.h"

surface k3d_luna(float Ka = .5, Kd = 1;
		 float lacunarity = 2;
		 float octaves = 8;
		 float H = .3;
		 color highland_color = .7;
		 float maria_basecolor = .7, maria_color = .1;
		 float arg22 = 1, arg23 = .3;
		 float highland_threshold = -0.2;
		 float highland_altitude = 0.001, maria_altitude = 0.0004;
		 float peak_rad = .0075, inner_rad = .01, rim_rad =
		 .02, outer_rad = .05; float peak_ht = 0.005, rim_ht = 0.003;
		 float numrays = 8;	/* arg10 */
		 float rayfade = 1;	/* arg11 */
  )
{
  float radial_dist;
  point PP, PQ;
  float l, a, o, i, omega;
  float chaos;
  color Ct;
  float temp1;
  point vv;
  float uu, ht, freq, scale;
  float lighten;
  point NN;
  float pd;			/* pole distance */
  float raydist;

  PQ = P;
  PP = transform("shader", P);
  NN = normalize(N);
  radial_dist = sqrt(xcomp(PP) * xcomp(PP) + ycomp(PP) * ycomp(PP));
  omega = pow(lacunarity, (-.5) - H);

  /* bumpy = fBm (PP, omega, lacunarity, octaves); */
  l = 1;
  o = 1;
  a = 0;
  for(i = 0; i < octaves; i += 1)
    {
      a += o * snoise(l * PP);
      l *= lacunarity;
      o *= omega;
    }
  chaos = a;

  Ct = Cs;

  /* Insure that the crater is in one of the maria */
  temp1 = radial_dist * arg22;
  if(temp1 < 1)
    chaos -= arg23 * (1 - smoothstep(0, 1, temp1));

  if(chaos > highland_threshold)
    {
      PQ += chaos * highland_altitude * NN;
      Ct += highland_color * chaos;
    }
  else
    {
      PQ += chaos * maria_altitude * NN;
      Ct *= maria_basecolor + maria_color * chaos;
    }


	/***********************************************************************/
  /* Add crater */
  /* get normalized vector "v" */
  pd = 1 - v;
  vv = point(xcomp(PP) / radial_dist, 0, zcomp(PP) / radial_dist);
  lighten = 0;
  if(pd < peak_rad)
    {				/* central peak */
      uu = 1 - pd / peak_rad;
/*      lighten = uu*uu; */
      ht = peak_ht * smoothstep(0, 1, uu);
    }
  else if(pd < inner_rad)
    {				/* crater floor */
      ht = 0;
    }
  else if(pd < rim_rad)
    {				/* inner rim */
      uu = (pd - inner_rad) / (rim_rad - inner_rad);
      lighten = .75 * uu;
      ht = rim_ht * smoothstep(0, 1, uu);
    }
  else if(pd < outer_rad)
    {				/* outer rim */
      uu = 1 - (pd - rim_rad) / (outer_rad - rim_rad);
      lighten = .75 * uu * uu;
      ht = rim_ht * smoothstep(0, 1, uu * uu);
    }
  else
    ht = 0;
  PQ += ht * NN;
  lighten *= 0.2;
  Ct += color(lighten, lighten, lighten);

  /* Add some noise */
  if(uu > 0)
    {
      if(pd < peak_rad)
	{			/* if on central peak */
	  vv = 5 * PP + 3 * vv;
	  freq = 1;
	  scale = 1;
	  ht = 0;
	  for(i = 0; i < 4; i += 1)
	    {
	      ht += scale * snoise(freq * vv);
	      freq *= 2;
	      scale *= 0.833;
	    }
/*	  ht = wrinkled (vv, 2, .833, 4); */
	  PQ += 0.0025 * uu * ht * NN;
	}
      else
	{
	  vv = 6 * PP + 3 * vv;
	  freq = 1;
	  scale = 1;
	  ht = 0;
	  for(i = 0; i < 4; i += 1)
	    {
	      ht += scale * snoise(freq * vv);
	      freq *= 2;
	      scale *= 0.833;
	    }
/*	  ht = wrinkled (vv, 2, .833, 4); */
	  if(radial_dist > rim_rad)
	    uu *= uu;
	  PQ += 0.0025 * (0.5 * uu + 0.5 * ht) * NN;
	}
    }


  /* Make crater rays (PP, arg10, arg11, arg12, arg15, arg24, arg25, radial_dist);, yielding temp1 */
  lighten = 0;
  if(pd >= rim_rad && pd < 0.4)
    {
      lighten = smoothstep(.15, .5, snoise(62 * u));
      raydist = 0.2 + 0.2 * snoise(20 * mod(u + 0.022, 1));
      lighten *= (1 - smoothstep(raydist - .2, raydist, pd));
    }
  lighten = 0.2 * clamp(lighten, 0, 1);
  Ct += color(lighten, lighten, lighten);


  /* Recalc normal since we changed P a whole bunch. */
/*  N = normalize (calculatenormal (PQ)); */

  /* Shade like matte */
  Oi = 1;
  Ci = Ct * (Ka * ambient() + Kd * diffuse(faceforward(normalize(N), I)));
}
