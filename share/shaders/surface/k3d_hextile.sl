/*
 * hextile.sl -- surface shader for hexagonal tiles in st space
 *
 * DESCRIPTION
 *       This surface shader operates in s-t space and gives a pattern of
 *    hexagonal tiles, similar to that found as floor patterns in public
 *    places and such.
 *       The basic pattern is a hexagonal tiling, with a little bit of
 *    color variation from tile to tile.  On top of that is some staining
 *    (presumably due to water or something), which darkens the tile or
 *    mortar underneath it.  Finally, there is scuffing due to people's
 *    shoes, which really only affects the tile part not the mortar part.
 *
 *
 * PARAMTERS
 *    Ka, Kd, Ks, roughness, specularcolor - work just like plastic
 *    tilecolor - the color of the tiles
 *    mortarcolor - the color of the mortar (space between the tiles)
 *    tileradius - the "radius" (in s-t units) of a single tile
 *    mortarwidth - the width of the mortar (in s-t units)
 *    tilevary - the color variance from tile to tile
 *
 * ANTIALIASING
 *    Some rudimentary antialiasing is performed on the borders between
 *    tile and mortar.
 *
 * HINTS & APPLICATIONS
 *    If all of the default parameters are used, the tiles look just like
 *    the floors in the public areas of the Washington DC subway system.
 *
 * AUTHOR: written by Larry Gritz, 1994
 *
 * HISTORY:
 *    15 Feb 1994 -- written by lg
 *
 * last modified 15 Feb 94 by Larry Gritz
 */


#include "k3d_noises.h"

surface
k3d_hextile (float Ka = .5;
         float Kd = .5;
         float Ks = .2;
         float roughness = .1;
	 color specularcolor = 1;
	 color tilecolor = color(.55,0,0);
	 color mortarcolor = color(.5,.5,.5);
	 float tileradius = 0.2;
	 float mortarwidth = 0.02;
	 float tilevary = 0.15;
	 float scuffing = 0.5;
	 float stains = 0.4;
         float stainfrequency = 2;
         float scufffrequency = 4;
	 color scuffcolor = color (.05,.05,.05))
{
  point Nf;
  color Ct, Ctile;
  float tilewidth;
  float ss, tt;
  float ttile, stile;
  float x, y;
  float mortar;
  float swidth, twidth, sfuzz, tfuzz, fuzzmax;
  float mw2;
  float tileindex;
  float stain, scuff;
  float ks;

  /* Determine how wide in s-t space one pixel projects to */
  swidth = abs(Du(s)*du) + abs(Dv(s)*dv);
  twidth = abs(Du(t)*du) + abs(Dv(t)*dv);
  sfuzz = 0.5 * swidth;
  tfuzz = 0.5 * twidth;
  fuzzmax = max (sfuzz, tfuzz);

  tilewidth = tileradius * 1.7320508;  /* sqrt(3) */
  tt = mod (t, 1.5*tileradius);
  ttile = floor (t/(1.5*tileradius));
  if (mod (ttile/2, 1) == 0.5)
       ss = s + tilewidth/2;
  else ss = s;
  stile = floor (ss / tilewidth);
  ss = mod (ss, tilewidth);
  mortar = 0;
  mw2 = mortarwidth/2;
  if (tt < tileradius) {
      mortar =  1 - (smoothstep(mw2,mw2+sfuzz,ss) *
		     (1 - smoothstep(tilewidth-mw2-sfuzz,tilewidth-mw2,ss)));
    }
  else {
      x = tilewidth/2 - abs (ss - tilewidth/2);
      y = 1.7320508 * (tt - tileradius);
      if (y > x) {
	  if (mod (ttile/2, 1) == 0.5)
	      stile -= 1;
	  ttile += 1;
	  if (ss > tilewidth/2)
	      stile += 1;
	}
      mortar = (smoothstep (x-1.73*mw2-tfuzz, x-1.73*mw2, y) *
		(1 - smoothstep (x+1.73*mw2, x+1.73*mw2+tfuzz, y)));
    }

  tileindex = stile+41*ttile;
  Ctile = tilecolor * (1 + tilevary * snoise(tileindex+0.5));

  stain = stains * smoothstep (.5,1, noise(s*stainfrequency,t*stainfrequency));

  scuff = scuffing * smoothstep (.6,1, noise(t*scufffrequency-90.26,
					     s*scufffrequency+123.82));

  ks = Ks * (1-scuff/2);
  Ct = (1-stain) * mix (mix (Ctile, scuffcolor, scuff), mortarcolor, mortar);
  Oi = Os;
  Nf = faceforward (normalize(N),I);
  Ci = Os * ( Ct * (Ka*ambient() + Kd*diffuse(Nf)) +
	      specularcolor * ks*specular(Nf,-normalize(I),roughness));
}

