/*
 * TLShiftedMoonTile.sl -- shifted tile of cresent moons
 *
 * DESCRIPTION:
 *	Tile of shifted  cresent moons (disks)
 * 
 * PARAMETERS:
 *	Ka, Kd, Ks - the usual
 *  roughness - Contols the specular reflection
 *	fuzz -      Amount to blur edge
 *	radius -    Radis of disk (moon)
 *	sfreq - # of tiles in s
 *	tfreq - # of tiles in t
 *	eclipseCenter - Center of obscuring disk
 *	cstate1 - Color of disk
 *	specularcolor - color of specular highlight
 *
 * HINTS:
 *	Really should move center to the paramter list so both centers 
 *	 can be controled.
 *
 * AUTHOR: Tal Lancaster
 *	tal@SpamSucks_cs.caltech.edu
 *
 * History:
 *	Created: 8/12/95
 */ 
 
#include "k3d_rmannotes.h"
#include "k3d_functions.h"

surface
k3d_shiftedmoontile (
	uniform float Ka = 1;
	uniform float Kd = .5;
	uniform float Ks = .5;
	uniform float roughness = .1;
	uniform float fuzz = .02;
	uniform float radius = 0.45;
	uniform float sfreq = 4.0;
	uniform float tfreq = 4.0;
	uniform point eclipseCenter = point "current" (0.6, 0.5, 0);
	uniform color cstate1 = color(1, 0, 0);
	uniform color specularcolor = 1;)
{
	point Nf;
	uniform point center;            /* Center of disk */
	color surfColor;         /* Color of surface */
	float mix_opacity;       /* How much to mix between the surfaces */
	float circle1, circle2;  /* True if in circle1, circle2 */
	float ss, tt;            /* Tile coordinates */
	float row, col;          /* Location in tiles */
	float d, d2;             /* Point distance from circle1, circle2 */

	Nf = faceforward (normalize(N),I);
	surfColor = Cs;
	center = (0.5, 0.5, 0);  /* This should really be moved */
	                         /*  to the paramter list */

 	row = whichtile (t, tfreq);
	if (isOdd(row) == 0)
		ss = mod (s * sfreq + 0.5, 1);
	else
		ss = repeat (s, sfreq);
	
	tt = repeat (t, tfreq);
	
	d = distance (center, (ss, tt, 0));
	d2 = distance (eclipseCenter, (ss, tt, 0));

	circle1 = 1 - smoothstep (radius - fuzz, radius + fuzz, d);
	circle2 = 1 - smoothstep (radius - fuzz, radius + fuzz, d2);
	mix_opacity = difference (circle1, circle2);
	surfColor = mix (surfColor, cstate1, mix_opacity);
	
	Oi = Os;
	Ci = Os * (surfColor * (Ka*ambient() + Kd*diffuse(Nf)) +
	      specularcolor * Ks*specular(Nf,-normalize(I),roughness));
}
