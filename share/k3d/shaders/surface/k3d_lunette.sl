/* renamed JMlinette.sl for RMR.  -- tal */

/******************************************************************************
 * lunette.sl -- A pretty surface shader ideal for "ground planes".
 *
 * Description:
 *   A grid pattern overlaid upon an fBm noise.  This shader is good for
 *   surfaces placed beneath your scene which act as the ground.  The default
 *   colors are a bland pastel blue and yellow, which should complement many
 *   hero objects that you might wish to show off.  The shader is a copy of a
 *   pattern I saw in the background of another image entitled "Lunettes",
 *   which I believe has something to do with spectacles in French.  Hence the
 *   name, anyway.  The pattern is calculated in 2D u,v coordinates.
 *
 * Parameters:
 *   Ka, Kd, Ks, roughness, specularcolor - work like the plastic shader
 *   urepeats, vrepeats - number of repeats of the grid pattern in u and v
 *   ulinewidth, vlinewidth - proportional width of the grid lines
 *   colorA, colorB - colors used in the background fBm noise
 *   baselinecolor - base color of the grid lines
 *   colorBenhance - amount to enhance colorB's presence over colorA
 *   gridDensity - effective opacity of the grid lines
 *   noiseScale - scale factor for the fBm noise relative to the grid size
 *   noiseRandom - randomization factor for the fBm noise
 *   maxOctaves, lunacrity, gain - work as in the fBm function
 *
 * Antialiasing:
 *   The shader should antialias itself quite well, since it uses antialiased
 *   noise functions and an box-filter antialiased grid function.
 *
 * Author: written by Jonathan Merritt (jmerritt@warpax.com), 5th October 2002
 *
 * License: This shader is distributed under the GNU General Public License.
 *
 *****************************************************************************/

#include "k3d_patterns.h"
#include "k3d_noises.h"


/**
 * Base color for the "lunette" pattern.  The pattern is calculated in a two-
 * dimensional fashion using the coordinates ss and tt as texture / pattern
 * coordinates.
 *
 * Parameters:
 *   ss, tt - 2D pattern coordinates.
 *   ssrepeats, ttrepeats - Number of repeats of the grid pattern in both ss
 *                           and tt directions.  Also scales the noise in both
 *                           directions appropriately.
 *   sslinewidth, ttlinewidth - Width of the grid lines in ss and tt
 *                              respectively; expressed as a fraction of the
 *                              cell over which the grid repeat occurs.
 *   colorA, colorB - Two colors for the background pattern.
 *   noiseScale - Scale of the background fBm noise (relative to the size of
 *                a grid cell).  Increasing this value makes a finer, smaller
 *                noise; decreasing it makes a broader noise.
 *   noiseRandom - Randomization value for the noise.
 *   maxOctaves - Maximum number of octaves for the fBm noise.
 *   lunacrity - Lunacrity of the fBm noise.
 *   gain - Gain of the fBm noise.
 *   colorBenhance - After the noise has been calculated, there exists the
 *                   possibility to increase the amount of colorB present in
 *                   the output.  A value of colorBenhance = 1 means that
 *                   colorA and colorB are present in equal amounts.
 *                   Increasing this value puts more colorB in the output.
 *   gridDensity - Effective opacity of the grid pattern over the underlying
 *                   noise.
 *   baselinecolor - Base color of the grid lines.
 */
color lunetteColor (
	float ss;		/* ss coordinate for the lunette pattern */
	float tt;		/* tt coordinate for the lunette pattern */
	float ssrepeats;	/* number of repeats in the ss direction */
	float ttrepeats;	/* number of repeats in the tt direction */
	float sslinewidth;	/* proportional width of lines in ss */
	float ttlinewidth;	/* proportional width of lines in tt */
	color colorA;		/* first color in the pattern */
	color colorB;		/* second color in the pattern */
	float noiseScale;	/* scale of the noise relative to the grid */
	float noiseRandom;	/* randomization for the noise */
	uniform float maxOctaves;	/* maximum number of octives for the noise */
	uniform float lunacrity;	/* lunacrity of the noise */
	uniform float gain;		/* gain for the noise */
	float colorBenhance;	/* amount to enhance color B */
	float gridDensity;	/* density of the grid pattern */
	color baselinecolor;	/* base color for the lines */
) {

	/*
	 * Calculate pulsegrid, which is a variable indicating the value of
	 * the grid.  pulsegrid = 0 means that there is no grid at the point
	 * whereas pulsegrid = 1 means that there IS a grid at the point.
	 * pulsegrid varies smoothly between the two values to cope with
	 * antialising at the edges of the grid.
	 */
	float pulsess = filteredpulsetrain(
		1/ssrepeats, sslinewidth, ss, filterwidth(ss)
	);
	float pulsett = filteredpulsetrain(
		1/ttrepeats, ttlinewidth, tt, filterwidth(tt)
	);
	float pulsegrid = 1 - min(pulsess, pulsett); 

	/*
	 * Find the base color for the pattern.  The base color is a mix
	 * between colorA and colorB, created using noise and other mixing
	 * parameters.
	 */
	point noisePt = noiseScale * 
		point(ss*ssrepeats, tt*ttrepeats, noiseRandom);
	float noisefilterwidth = filterwidthp(noisePt);
	float noiseamt = (fBm(
		noisePt, noisefilterwidth, maxOctaves, lunacrity, gain
	)+1) / 2;
	noiseamt = pow(noiseamt, colorBenhance);
	color basecolor = mix(colorB, colorA, noiseamt);

	/*
	 * find the base line color for the grid pattern
	 */
	color linecolor = mix(basecolor, baselinecolor, gridDensity);

	/*
	 * return the mix between the base grid and the colorful noise
	 */
	return mix(basecolor, linecolor, pulsegrid);

}

surface k3d_lunette (
	/* Plastic illumination model parameters. */
	float Ka = 1;
	float Kd = .5;
	float Ks = .5;
	float roughness = .1;
	color specularcolor = 1;
	/* Grid pattern parameters. */	
	float urepeats = 30;		/* Number of repeats in u */
	float ulinewidth = 0.125;	/* Relative width of grid lines in u */
	float vrepeats = 30;		/* Number of repeats in v */
	float vlinewidth = 0.12;	/* Relative width of grid lines in v */
	/* Colors used. */
	color colorA = color(.36, .58, .67);
	color colorB = color(.88, .88, .71);
	color baselinecolor = color(.51, .48, .52);
	float colorBenhance = 1.6;	/* Enhancement of colorB in output */
	float gridDensity = .5;		/* Effective opacity of the grid */
	/* fBm noise parameters */
	float noiseScale = .3;
	float noiseRandom = 0;
	float maxOctaves = 5;
	float lunacrity = 1.3;
	float gain = .7;
) {
	/*
	 * fetch the lunette color
	 */
	color cc = lunetteColor(
		u, v, urepeats, vrepeats, ulinewidth, vlinewidth,
		colorA, colorB, noiseScale, noiseRandom, maxOctaves,
		lunacrity, gain, colorBenhance, gridDensity, baselinecolor
	);

	/*
	 * apply a plastic illumination model
	 */
	normal Nf = faceforward (normalize(N),I);
	Oi = Os;

	Ci = Os * ( cc * (Ka*ambient() + Kd*diffuse(Nf)) +
		specularcolor * Ks*specular(Nf,-normalize(I),roughness));

}
