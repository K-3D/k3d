/*
 * TLShiftedD_RTile.sl -- generates a surface of alternating disks and rings
 *
 * DESCRIPTION:
 *	Will generate alternating and shifted rows of disks and rings
 * 
 *
 * PARAMETERS:
 *	Ka, Kd, Ks - the usual
 *  roughness -  Contols the specular reflection
 *	fuzz -       Amount to blur edge
 *	innerRadius - inner ring
 *	outerRadius - outer ring
 *	sfreq - # of tiles in s
 *	tfreq - # of tiles in t 
 *	cstate1 - foreground color
 *	specularcolor - color of specular highlight
 * 
 * HINTS:
 *	The center varible should really be placed in the parameter list 
 *      (remember to set the right space)
 *  Also, it would be better to set the width of the ring rather than
 *      specifing the inner and outer ring.
 *  Another thing that might be useful is to add a parameter to 
 *      choose which row to do first.
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
k3d_shifteddrtile (
	uniform float Ka = 1;
	uniform float Kd = .5;
	uniform float Ks = .5;
	uniform float roughness = .1;
	uniform float fuzz = .025;          /* amount to blur edge */
	uniform float innerRadius = 0.3;    /* inner ring */
	uniform float outerRadius = 0.45;   /* outer ring */
	uniform float sfreq = 4.0;          /* # of tiles in s */
	uniform float tfreq = 4.0;          /* # of tiles in t */
	uniform color cstate1 = color(1, 0, 0);  /* foreground color */
	uniform color specularcolor = 1;)
{
	point Nf;
	uniform point center;      /* Center of disk */
	color surfColor;   /* Color of surface */ 
	float mix_opacity; /* How much to mix between the surfaces */
	float ss, tt;      /* tiled s, t */
	float row, col;    /* used to determine which tile we are in */
	float d;           /* distance from center of current tile */
	float wasOdd;      /* True if test was odd */
	
	Nf = faceforward (normalize(N),I);

	surfColor = Cs;
	center = (0.5, 0.5, 0);  /* This should really be */
	                         /* put in the paramter list */

 	row = whichtile (t, tfreq);
	
	if (isOdd(row) == 0) {
		ss = mod (s * sfreq + 0.5, 1);
		wasOdd = 0;
	}
	else {
		ss = repeat (s, sfreq);
		wasOdd = 1;
	}
	
	tt = repeat (t, tfreq);
	d = distance (center, (ss, tt, 0));

	if (wasOdd == 1)
		/* Do ring */
		mix_opacity = smoothPulse (innerRadius, outerRadius, fuzz, d);
	else
		/* Do disk */
		mix_opacity = 1 - smoothstep (outerRadius-fuzz, outerRadius+fuzz, d);
		
	surfColor = mix (surfColor, cstate1, mix_opacity);
	
	
	Oi = Os;
	Ci = Os * (surfColor * (Ka*ambient() + Kd*diffuse(Nf)) +
	      specularcolor * Ks*specular(Nf,-normalize(I),roughness));
}
