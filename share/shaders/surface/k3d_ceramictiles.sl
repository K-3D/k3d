/****************************************************************************
 * ceramictiles.sl
 *
 * Description: Ceramic tiles (like you'd find in a bathroom)
 *   
 * Parameters for pattern placement and size:
 *   projection, textureprojspace, mx -  define the projection used to
 *     establish a basic 2-D coordinate system for the pattern.
 *   stilespacing, ttilespacing - tile-to-tile spacing (separate controls
 *     for s and t directions)
 *   groovewidth, grooveheight - width of the spacing between tiles,
 *     expressed as a fraction of the tile-to-tile spacing.
 *   groovedepth - displacement amount for the grooves (expressed in
 *     shader space units)
 *   truedisp - 1 for true displacement, 0 for bump mapping
 *
 * Parameters for tile color and pattern:
 *   Cbase, Cmottle - base color and mottle color of the tile
 *   mottlefreq - frequency of the mottling between Cbase & Cmottle
 *   Cedge - separate edge color for the tiles
 *   Cspeck - color of the occasional specks in the tiles
 *   edgevary, mottling, speckly - individual scalar controls over
 *      edge variation, mottling, and speckles.  Setting any to zero will
 *      turn that feature off.
 *   varyhue, varysat, varylum - individual controls for the per-tile
 *      color variation (0 means don't vary in that way, larger values
 *      cause more tile-to-tile variation).
 *
 * Parameters for illumination model:
 *   Ka - the usual meaning
 *   Kdmortar - Kd for the mortar between tiles
 *   mortarcolor - base color of the mortar
 *   Kdtile - diffuse component weighting of the tile
 *   Ks, roughness, specsharpness - glossy specular controls of the tile
 *   Kr, blur, eta - reflection parameters for the tile
 *   envname, envspace, envrad - environment mapping controls
 *   rayjitter, raysamples - ray tracing controls
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
#include "k3d_material.h"
#include "k3d_noises.h"
#include "k3d_displace.h"
#include "k3d_patterns.h"

/* Given 2-D texture coordinates ss,tt and their filter widths ds, dt,
 * and the width and height of the grooves between tiles (assuming that
 * tile spacing is 1.0), figure out which (integer indexed) tile we are
 * on and what coordinates (on [0,1]) within our individual tile we are
 * shading.
 */
float tilepattern(float ss, tt, ds, dt;
		  float groovewidth, grooveheight;
		  output float swhichtile, twhichtile;
		  output float stile, ttile;)
{
  swhichtile = floor(ss);
  twhichtile = floor(tt);
  stile = ss - swhichtile;
  ttile = tt - twhichtile;

  return filteredpulsetrain(groovewidth, 1, ss + groovewidth / 2,
			    ds) * filteredpulsetrain(grooveheight, 1,
						     tt + grooveheight / 2,
						     dt);
}



/* Given coordinates (stile,ttile) and derivatives (ds,dt) *within* a
 * single tile, calculate the color of the tile at that point.  Major
 * features include: (1) mottling of the color; (2) darkening or shifting
 * to a different color near the border of the tile (with a ragged edge
 * to the color transition); (3) occasional dark specks.
 */
color tiletexture(float tileindex;
		  float stile, ttile, ds, dt;
		  float edgevary, mottling, speckly; float mottlefreq;
		  color Cbase, Cedge, Cmottle, Cspeck)
{
  color C = Cbase;
  float dst = max(ds, dt);
  if(mottling > 0)
    {
      point noisep = mottlefreq * point(stile, ttile, tileindex);
      float mottle =
	.2 + .6 * fBm(noisep, mottlefreq * max(ds, dt), 4, 2, 0.65);
      C = mix(C, Cmottle, clamp(mottling * mottle, 0, 1));
    }
  if(edgevary > 0)
    {
      float sedgeoffset =
	.05 * fBm(point(stile * 10, ttile * 10, tileindex + 10),
		  10 * dst, 2, 2, 0.5);
      float tedgeoffset =
	.05 * fBm(point(stile * 10, ttile * 10, tileindex - 3),
		  10 * dst, 2, 2, 0.5);
      float edgy =
	1 -
	(smoothpulse(.05, .15, .85, .95, stile + sedgeoffset) *
	 smoothpulse(.05, .15, .85, .95, ttile + tedgeoffset));
      C = mix(C, Cedge, edgevary * edgy);
    }
  if(speckly > 0)
    {
      float speckfreq = 7;
      point noisep =
	point(stile * speckfreq, ttile * speckfreq, tileindex + 8);
      float specky = filteredsnoise(noisep, speckfreq * dst);
      specky = smoothstep(0.55, 0.7, specky);
      C = mix(C, Cspeck, speckly * specky);
    }
  return C;
}




/* Compute the color of a ceramic object.  Like plastic, but use a
 * "glossy" specular term.  We're actually blending between a purely
 * diffuse model for the mortar, and a ceramic model for the tiles,
 * depending on the variable intile.  When in the mortar area, we turn
 * off highlights and reflections.
 */
color MaterialCeramicTiles(normal Nf;
			   color Cmortar, Ctile;
			   float intile;
			   float Ka, Kdmortar, Kdtile, Ks;
			   float roughness, specsharpness, Kr, blur, eta;
			   DECLARE_ENVPARAMS)
{
  extern vector I;
  extern point P;
  color basecolor = mix(Cmortar, Ctile, intile);
  float ks = Ks * intile;
  float kd = mix(Kdmortar, Kdtile, intile);
  vector IN = normalize(I), V = -IN;
  float fkr, fkt;
  vector R, T;
  fresnel(IN, Nf, 1 / eta, fkr, fkt, R, T);
  fkt = 1 - fkr;
  float kr = fkr * Kr * intile;
  return fkt * basecolor * (Ka * ambient() + kd * diffuse(Nf)) +
    ks * LocIllumGlossy(Nf, V, roughness / 10,
			specsharpness) + SampleEnvironment(P, R, kr, blur,
							   ENVPARAMS);
}



surface k3d_ceramictiles(float Ka = 1, Ks = .75, roughness =
			 0.1, specsharpness = 0.5;
			 float Kr = 1, blur = 0, eta = 1.5;
			 float Kdtile = 0.5;
			 float Kdmortar = 0.8;
			 color mortarcolor = color(.5, .5, .5);
			 DECLARE_DEFAULTED_ENVPARAMS;
			 float stilespacing = 10, ttilespacing = 10;
			 float groovewidth = 0.06, grooveheight = 0.06;
			 float groovedepth = 0.2, truedisp = 0;
			 string projection = "st";
			 string textureprojspace = "shader";
			 float mx[16] =
			 {
			 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
			 float edgevary = 1, mottling = 1, speckly = 1;
			 float mottlefreq = 7;
			 color Cbase = color(.05, .075, .6);
			 color Cedge = color(.025, .025, .2);
			 color Cmottle = color(.2, .2, .7);
			 color Cspeck = color(.015, .015, .015);
			 float varyhue = 0.025, varysat = 0.4, varylum = 0.5;)
{
  /*
   * Get a 2-D texture coordinates for the texturing, then
   * Normalize everything so that the tiles are 1x1 units
   */
  float ss, tt, dss, dtt;
  ProjectTo2D(projection, P, textureprojspace, array_to_mx(mx), ss, tt, dss,
	      dtt);
  ss /= stilespacing;
  dss /= stilespacing;
  tt /= ttilespacing;
  dtt /= ttilespacing;

  /*
   * Find out where in the pattern we are: which tile we're on, and
   * the (stile,ttile) coordinates (both on [0,1]) within our tile.
   */
  float swhichtile, twhichtile, stile, ttile;
  float intile = tilepattern(ss, tt, dss, dtt,
			     groovewidth, grooveheight,
			     swhichtile, twhichtile, stile, ttile);
  float tileindex = swhichtile + 13 * twhichtile;

  /*
   * Displacement: the edges of the tile displace down a bit, as do
   * the grooves between tiles.  Also, add just a little bit of
   * per-tile normal variation to break up reflections.
   */
  float tiledisp = smoothpulse(0, .075, 0.925, 1, stile);
  tiledisp *= smoothpulse(0, .075, 0.925, 1, ttile);
  normal Nf = faceforward(normalize(N), I);
  normal Ntile =
    Displace(Nf, "shader", groovedepth * (tiledisp - 1), truedisp);

  Ntile += 0.05 * (vector cellnoise(tileindex + 5) - 0.5);
  Nf = normalize(mix(Nf, Ntile, intile));

  /* 
   * Here's the exciting part -- calculate the color of the spot we're
   * in within the tile.  Then use the tile index to vary its color
   * so every tile looks a little different.
   */
  color Ctile = tiletexture(tileindex, stile, ttile, dss, dtt,
			    edgevary, mottling, speckly,
			    mottlefreq,
			    Cbase, Cedge, Cmottle, Cspeck);
  Ctile = varyEach(Ctile, tileindex, varyhue, varysat, varylum);

  /*
   * Set the color of the mortar between tiles, make it look good by
   * scaling it by some high frequency fBm.
   */
  color Cmortar = mortarcolor;
  point Q = 20 * point(ss, tt, 0);
  float dQ = filterwidthp(Q);
  if(intile < 1.0)
    Cmortar *= smoothstep(0, 1, (.5 + .4 * fBm(Q, dQ, 3, 2, .6)));

  /*
   * Illumination model
   */
  Ci =
    MaterialCeramicTiles(Nf, Cmortar, Ctile, intile, Ka, Kdmortar, Kdtile, Ks,
			 roughness, specsharpness, Kr, blur, eta, ENVPARAMS);
  Oi = Os;
  Ci *= Oi;
}
