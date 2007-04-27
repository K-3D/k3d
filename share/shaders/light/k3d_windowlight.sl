/****************************************************************************
 * windowlight.sl - make a window light (with crossbars)
 *
 * Description::
 *   Simulates light coming through a window.  The light doesn't
 *   diverge or falloff.
 *
 * Parameters:
 *   intensity - overall intensity scaling of the light
 *   lightcolor - overall color filtering for the light
 *   center - the spatial position of the center of the window
 *   up, in - vectors which define the orientation of the window
 *   from, to - the direction that the light falls
 *   hpanes, vpanes - number of horizontal and vertical panes
 *   panewidth, paneheight - width/height of the individual panes
 *   fuzz - controls the fading out near the edges
 *   framewidth, frameheight - how thick are the window frame "bars",
 *               as percentage of panewidth/paneheight
 *
 * Author:  Larry Gritz, with inspiration from [Upstill]
 *
 * $Revision: 1.1 $    $Date: 2006/02/25 20:11:54 $
 *
 ****************************************************************************/

#include "k3d_patterns.h"

light k3d_windowlight(float intensity = 1;
		      color lightcolor = color(1, 0.9, 0.6);
		      color darkcolor = color(.05, 0.15, 0.1);
		      point from = point "shader"(0, 0, 0);
		      point to = point "shader"(0, 1, 0);
		      point center = point "shader"(0, 0, 0);
		      vector up = vector "shader"(0, 0, 1);
		      vector in = vector "shader"(0, 1, 0);
		      float hpanes = 2, vpanes = 3;
		      float panewidth = 6, paneheight = 6;
		      float framewidth = .1, frameheight = .1;
		      float fuzz = 0.25;
  )
{
  uniform vector inv, right, upv;
  uniform vector path;
  float offset, modulus, yfract, xfract;
  float d;
  point PL;			/* point on the light */

  path = normalize(from - to);
  inv = normalize(in);
  right = (up) ^ inv;
  upv = normalize(inv ^ right);
  right = upv ^ inv;

  /* d is the depth "into" the room perpendicular to the pane plane */
  d = inv.(Ps - center);
  PL = Ps - path * (d / (path.inv));

  illuminate(PL, -path, .01)
  {
    offset = (PL - center).upv + paneheight * (vpanes / 2);
    if(offset > 0 && (offset / paneheight) < vpanes)
      {
	modulus = mod(offset, paneheight);
	yfract =
	  filteredpulse(frameheight / 2, paneheight - frameheight / 2,
			modulus, fuzz);
      }
    else
      yfract = 0;

    offset = (PL - center).right + panewidth * (hpanes / 2);
    if(offset > 0 && (offset / panewidth) < hpanes)
      {
	modulus = mod(offset, panewidth);
	xfract =
	  filteredpulse(framewidth / 2, panewidth - framewidth / 2, modulus,
			fuzz);
      }
    else
      xfract = 0;
    Cl = intensity * mix(darkcolor, lightcolor, yfract * xfract);
  }
}
