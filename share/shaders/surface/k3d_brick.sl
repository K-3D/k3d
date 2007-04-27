/*
 * brick.sl -- Surface shader for a bricks.
 *
 * DESCRIPTION:
 *   Makes a wall of bricks.  Need more be said?  OK.  It makes a good
 *   looking staggered brick masonry.  It is especially convincing when
 *   used in conjunction with the "brickbump" displacement shader (and
 *   identical parameters).  Every other row of bricks is staggered.
 *   The staggering isn't exact, however, and this variance is controlled
 *   by the "rowvary" parameter.
 * 
 * PARAMETERS:
 *    Ka, Kd			The usual
 *    brickcolor, mortarcolor	Pretty obvious (default is red bricks)
 *    brickvary                 How much does the brick color vary from
 *     				   brick to brick?
 *    brickwidth                Width of a brick (in st space)
 *    brickheight               Height of a brick (in st space)
 *    mortarthickness           Thickness of the mortar (in st space)
 *    rowvary                   How much does each row shift?
 *    jagged                    How much do bricks deviate from squares?
 *
 * AUTHOR: written by Larry Gritz, gritzl@acm.org
 */

#include "k3d_noises.h"
#include "k3d_patterns.h"
#include "k3d_functions.h"

surface k3d_brick(float Ka = 1, Kd = 1;
		  color brickcolor = color "rgb"(.6, .1, .1);
		  color mortarcolor = color "rgb"(.6, .6, .6);
		  float raggedamp = 0.04, raggedfreq = 12;
		  float jagged = 0.006, brickvary = 0.3;
		  float brickwidth = .28, brickheight = .07;
		  float mortarthickness = .014;
		  float rowvary = .5;
		  float pitting = 0.01;
		  float pockfrequency = 10, groovedepth = 0.01;)
{
  color bcolor, Ct;
  normal Nf;
  float sbrick, tbrick, w, h;
  float ss, tt;
  float swidth, twidth;
  uniform float BMWIDTH = (brickwidth + mortarthickness);
  uniform float BMHEIGHT = (brickheight + mortarthickness);
  uniform float MWF = (mortarthickness * 0.5 / BMWIDTH);
  uniform float MHF = (mortarthickness * 0.5 / BMHEIGHT);
  float whichbrick;
  float fact, disp;

  /* Determine how wide in s-t space one pixel projects to, relative
   * the the width and height of a brick.  Overestimate the filter
   * size by a bit -- it makes the transitions between brick and mortar
   * a bit smoother.
   */
  swidth = 1.5 * max(filterwidth(s), MINFILTWIDTH) / BMWIDTH;
  twidth = 1.5 * max(filterwidth(t), MINFILTWIDTH) / BMHEIGHT;

  basicbrick(s, t, BMWIDTH, BMHEIGHT, 0.5, 0.2, 1, jagged, sbrick, tbrick, ss,
	     tt);

  /* Make the edges ragged, but different for each brick */
  whichbrick = 103 * sbrick + tbrick;
  ss +=
    raggedamp * snoisexy((s + tbrick * 5.15) * raggedfreq,
			 (t + sbrick * 23.8) * raggedfreq);
  tt +=
    raggedamp * snoisexy((s + tbrick * 11.4) * raggedfreq,
			 (t + sbrick * 7.2) * raggedfreq);
  ss +=
    raggedamp / 2 * snoisexy((s + tbrick * 5.15) * raggedfreq * 2,
			     (t + sbrick * 23.8) * raggedfreq * 2);
  tt +=
    raggedamp / 2 * snoisexy((s + tbrick * 11.4) * raggedfreq * 2,
			     (t + sbrick * 7.2) * raggedfreq * 2);

  /* Choose a color for the surface */
  if(swidth >= 1)
    w = 1 - 2 * MWF;
  else
    w =
      clamp(filteredpulse(MWF, 1 - MWF, ss, swidth), max(1 - MWF / swidth, 0),
	    1);
  if(twidth >= 1)
    h = 1 - 2 * MHF;
  else
    h =
      clamp(filteredpulse(MHF, 1 - MHF, tt, twidth), max(1 - MHF / twidth, 0),
	    1);

  fact = 1;
  disp = 0;
  if(tt < MHF)
    {
      /* We're in the top horizontal groove */
      disp = groovedepth * (sqr((tt) / MHF) - 1);
    }
  else if(tt > (1.0 - MHF))
    {
      /* Bottom horizontal groove */
      disp = groovedepth * (sqr((1 - tt) / MHF) - 1);
    }
  if(ss < MWF)
    {
      disp = min(disp, 0.85 * groovedepth * (sqr(ss / MWF) - 1));
    }
  else if(ss > (1.0 - MWF))
    {
      disp = min(disp, 0.85 * groovedepth * (sqr((1 - ss) / MWF) - 1));
    }

  fact = smoothstep(0, 1.3 * MHF, tt) - smoothstep(1.0 - 1.3 * MHF, 1, tt);
  fact *= (smoothstep(0, 1.3 * MWF, ss) - smoothstep(1.0 - 1.3 * MWF, 1, ss));
  fact = pitting * (0.5 * fact + 0.5);
  disp -=
    fact *
    pow(noise
	((ss + sbrick) * pockfrequency / BMHEIGHT,
	 (tt + tbrick) * pockfrequency / BMWIDTH), 0.25);

  P += disp * normalize(N);
  N = calculatenormal(P);
  Nf = faceforward(normalize(N), I);


  /* Choose a brick color that varies from brick to brick */
  bcolor = brickcolor * (1 + (brickvary * snoise(whichbrick + 0.5)));

  Ct = mix(mortarcolor, bcolor, w * h);

  Oi = Os;
  Ci = Os * Ct * (Ka * ambient() + Kd * diffuse(Nf));
}
