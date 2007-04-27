
/*
 * parquet_plank.sl -- another surface shader for wood.
 *
 * DESCRIPTION:
 *   Makes texture of wooden planks in s-t space.  This wood looks rather
 *   like oak plank parquet floor tiles.  The actual wood and plank pattern
 *   is based on my "planks" shader.  This shader works best if "s" and "t"
 *   units are both the same size in world space.
 *
 * PARAMETERS:
 *   Ka, Kd, Ks, specular, roughness - work just like the plastic shader
 *   txtscale - overall scaling factor for the texture
 *   plankwidth - width of each plank (in terms of s/t)
 *   plankspertile - number of planks in each parquet tile
 *   ringscale - scaling for the ring spacing
 *   grainscale - scaling for the fine grain
 *   groovewidth - width of the grooves between the planks (in terms of s/t)
 *   lightwood, darkwood - surface colors for the wood itself
 *   groovecolor - the color of the "grooves" between the planks
 *   plankvary - controls how much wood color varies from plank to plank
 *   grainy - relative graininess (0 = no fine grain)
 *   wavy - relative wavyness of the ring pattern
 *
 * ANTIALIASING: this shader does a pretty good job of antialiasing itself,
 *   even with low sampling densities.
 *
 * AUTHOR: written by Larry Gritz, the George Washington University
 *         email: gritz@SpamSucks_seas.gwu.edu
 *         snail: Dept. of EE & CS
 *                801  22nd St. NW, Rm. T-624-G
 *                Washington, DC 20052
 *
 * HISTORY:
 *    10 Feb 1995 - written by Larry Gritz, based on my "plank" shader.
 *    10 Feb 1995 - modified by wave to change the name
 *
 * last modified 10 Feb 1995 by wave
 */

#include "k3d_noises.h"
#include "k3d_rmannotes.h"



surface
k3d_parquet_plank2 (float Ka = 1, Kd = 0.75, Ks = .15, roughness = .025;
	       color specularcolor = 1;
	       float ringscale = 15, grainscale = 60;
	       float txtscale = 1;
	       float plankspertile = 4;
	       color lightwood = color (0.57, 0.292, 0.125);
	       color darkwood  = color (0.275, 0.15, 0.06);
	       color groovecolor  = color (.05, .04, .015);
	       float plankwidth = .05, groovewidth = 0.001;
	       float plankvary = 0.8;
	       float grainy = 1, wavy = 0.08; )
{

  float r, r2;
  point Nf;
  float whichrow, whichplank;
  float swidth, twidth, fwidth, ss, tt, w, h, fade, ttt;
  color Ct, woodcolor;
  float groovy;
  float PGWIDTH, PGHEIGHT, GWF, GHF;
  float tilewidth, whichtile, tmp, planklength;

  PGWIDTH = plankwidth+groovewidth;
  planklength = PGWIDTH * plankspertile - groovewidth;
  PGHEIGHT = planklength+groovewidth;
  GWF = groovewidth*0.5/PGWIDTH;
  GHF = groovewidth*0.5/PGHEIGHT;

  /* Determine how wide in s-t space one pixel projects to */
  swidth = (max (abs(Du(s)*du) + abs(Dv(s)*dv), MINFILTERWIDTH) / PGWIDTH) * txtscale;
  twidth = (max (abs(Du(t)*du) + abs(Dv(t)*dv), MINFILTERWIDTH) / PGHEIGHT) * txtscale;
  fwidth = max(swidth,twidth);

  Nf = faceforward (normalize(N),I);

  ss = (txtscale * s) / PGWIDTH;
  whichrow = floor (ss);
  tt = (txtscale * t) / PGHEIGHT;
  whichplank = floor(tt);
  if (mod (whichrow/plankspertile + whichplank, 2) >= 1) {
      ss = txtscale * t / PGWIDTH;
      whichrow = floor (ss);
      tt = txtscale * s / PGHEIGHT;
      whichplank = floor(tt);
      tmp = swidth;  swidth = twidth;  twidth = tmp;
    } 
  ss -= whichrow;
  tt -= whichplank;
  whichplank += 20*(whichrow+10);

  /*
   * Figure out where the grooves are.  The value groovy is 0 where there
   * are grooves, 1 where the wood grain is visible.  Do some simple
   * antialiasing.
   */
  if (swidth >= 1)
      w = 1 - 2*GWF;
  else w = clamp (boxstep(GWF-swidth,GWF,ss), max(1-GWF/swidth,0), 1)
	 - clamp (boxstep(1-GWF-swidth,1-GWF,ss), 0, 2*GWF/swidth);
  if (twidth >= 1)
      h = 1 - 2*GHF;
  else h = clamp (boxstep(GHF-twidth,GHF,tt), max(1-GHF/twidth,0),1)
	 - clamp (boxstep(1-GHF-twidth,1-GHF,tt), 0, 2*GHF/twidth);
  /* This would be the non-antialiased version:
   * w = step (GWF,ss) - step(1-GWF,ss);
   * h = step (GHF,tt) - step(1-GHF,tt);
   */
  groovy = w*h;


  /*
   * Add the ring patterns
   */
  fade = smoothstep (1/ringscale, 8/ringscale, fwidth);
  if (fade < 0.999) {
      ttt = tt/4+whichplank/28.38 + wavy * noise (8*ss, tt/4);
      r = ringscale * noise (ss-whichplank, ttt);
      r -= floor (r);
      r = 0.3 + 0.7 * smoothstep(0.2, 0.55, r) * (1 - smoothstep(0.75, 0.8, r));
      r = (1-fade)*r + 0.65*fade;

      /*
       * Multiply the ring pattern by the fine grain
       */
      fade = smoothstep (2/grainscale, 8/grainscale, fwidth);
      if (fade < 0.999) {
	  r2 = 1.3 - noise (ss*grainscale, (tt*grainscale/4));
	  r2 = grainy * r2*r2 + (1-grainy);
	  r *= (1-fade)*r2 + (0.75*fade);
        }
      else r *= 0.75;
    }
  else r = 0.4875;
  

  /* Mix the light and dark wood according to the grain pattern */
  woodcolor = mix (lightwood, darkwood, r);

  /* Add plank-to-plank variation in overall color */
  woodcolor *= (1-plankvary/2 + plankvary * noise (whichplank+0.5));

  Ct = mix (groovecolor, woodcolor, groovy);

  /* Use the plastic illumination model */
  Oi = Os;
  Ci = Os * ( Ct * (Ka*ambient() + Kd*diffuse(Nf)) +
	      specularcolor * Ks*specular(Nf,-normalize(I),roughness));
}
