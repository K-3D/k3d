/*
 * Renamed to JMredapple.sl for RMR -- talrmr@SpamSucks_pacbell.net  7/18/99
 *
 * redapple.sl - A nice shader for reddy-green apples.
 *
 * Copyright (C) Jonathan Merritt, 1999.
 * Feel free to use this shader to create apples anywhere and
 * everywhere.
 *
 * This shader creates a nice skin for red-green apples. It _is_ tuned
 * for specific geometry, but can very easily be adjusted to fit any
 * apple you care to lay it on. Apples are requred to have the "s"
 * texture parameter running equatorialy around them (like latitude
 * lines), and "t" running from pole to pole (like longditude). Apples
 * look best from the side, but work ok from the top, so long as you
 * don't mind the simple lerping I used to fade to green at the poles.
 *
 * Look out for the "txtscale" parameter: some noise calculations are
 * done using shader space, instead of "s" and "t" parameters (I think
 * my apple's parameters are stretched a bit?), so if you use
 * different size apples, make sure to change txtscale appropriately.
 *
 * I think this shader is really cool. If you think so too, you can
 * email me your praises, notification of my credits in big motion
 * pictures, job offers at Pixar, etc... to:  jmerritt@SpamSucks_warpax.com   :-)
 *
 * Have FUN!!!   :-)
 */

#include "k3d_noises.h"

/*#define USE_LG_RAYTRACE */

#ifdef USE_LG_RAYTRACE
#include "rayserver.h"
#include "raytrace.h"
#endif


#define TSCALE .05

surface
k3d_redapple (
	float Ks = .25;           /* Specular reflection coeff.    */
	float Kd = .5;            /* Diffuse reflection coeff.     */
	float Ka = 1;             /* Ambient light coeff.          */
	float Kr = .05;           /* Mirror-like reflection coeff. */
	float roughness = .1;     /* Specular roughness param.     */
	float label = 0;          /* Non-uniformity param.         */
	float redness = 1;        /* Amount of 'redness' - higher  */
	                          /*  values give redder apples.   */
	                          /*  2 = almost complete red.     */
	float txtscale = .05;     /* Ugly kludge (see above...)    */
	color specularcolor = 1;  /* Specular reflection color     */
)
{
	/*
	 * Shader variables
	 */
	vector Nf, V, Rdir;
	point PP, newP;
	float base_turb, blotch_turb, disp_turb;
	float small_noise, blotch, speck, disp = 0, blackness;
	color cs, small_speckle, base_color, reflect;


	/*
	 * Some nice colors for our red apple
	 */
	color red   = color(0.52,0.00,0.00);
	color dred  = color(0.40,0.00,0.00);
	color green = color(0.76,0.80,0.37);
	color brown = color(0.70,0.78,0.34);
	color black = color(0.00,0.00,0.00);


	/*
	 * Standard settings
	 */
	PP = transform("shader", P);
	V = normalize(-I);


	/*
	 * Pick the base color for the apple.
	 *
	 * The base color consists of patches of pure green,
	 * and patches of finely speckled red and green. These
	 * are set up so that the poles of the apple (as "t"
	 * goes to 1 or 0) are colored more and more green.
	 *
	 * The apple geometry is such that lines of constant
	 * "t" are lines of longditude (from pole to pole).
	 * The constants T1 - T4 specify locations of the
	 * start of 'greenness' toward the poles.
	 */
	#define BASE_SF          1.2    /* s-factor for big noise          */
	#define BASE_TF          3      /* t-factor for big noise          */
	#define BASE_NF          100    /* scaling factor for small noise  */
	#define BASE_NOISE_AMP   0.2    /* small noise color mix amplitude */
	#define BASE_GRC         0.4    /* shift factor for more red       */
	#define T1               0.0    /* t < T1 is pure green            */
	#define T2               0.18   /* lerp s.t. T1->T2 => green->red  */
	#define T3               0.8    /* T2->T3 => red can exist here    */
	#define T4               1.0    /* lerp s.t. T3->T4 => red->green  */
	                                /* t > T4 is pure green            */
	base_turb = noise(BASE_SF*sin(2*PI*s) + PI + label,
	                  BASE_TF*t + label);
	base_turb = pow(base_turb, (1/redness));
	small_noise = snoise(BASE_NF*PP*txtscale/TSCALE);
	small_speckle = mix(red, green, BASE_GRC+
	                    (small_noise*BASE_NOISE_AMP));
	if (t < T1)
		base_turb = 0;
	else if (t >= T1 && t < T2)
		base_turb *= (t-T1)/(T2-T1);
	else if (t >= T3 && t < T4)
		base_turb *= (T4-t)/(T4-T3);
	else if (t >= T4)
		base_turb = 0;
	base_color = spline(base_turb, green, green,
	                    small_speckle, small_speckle,
	                    small_speckle);


	/*
	 * Pick the blotch color.
	 *
	 * The blotch color is finally determined by mixing,
	 * using the blotching coefficient "blotch". This
	 * section sets that coefficient, using a funky yet
	 * really simple noise routine.
	 */
	#define BLOTCH_SF          20   /* s-factor for blotches  */
	#define BLOTCH_TF          15   /* t-factor for blotches  */
	#define BLOTCH_TCF         2.5  /* scaling kludge         */
	#define BLOTCH_DELTA       0.1  /* 'nother scaling kludge */
	#define BLOTCH_SPECK_COEFF 0.3  /* blotch specking coeff  */
	blotch_turb = noise(BLOTCH_SF*sin(2*PI*(s+.1234)) + PI + label,
	                    BLOTCH_TF*t + label) + BLOTCH_DELTA;
	blotch = pow(blotch_turb, 1.3) * BLOTCH_TCF * 
	         pow(base_turb, 2);
	blotch = blotch * (1+small_noise) +
	         small_noise * BLOTCH_SPECK_COEFF;
	if (blotch > 1) blotch = 1;


	/*
	 * Set the brown specking.
	 *
	 * The apple has brown speckles on it, set by mixing
	 * using the speckling coefficient "speck". These
	 * specks are very dot-like in nature, and this
	 * dottiness technique is stolen from LG's starfield 
	 * shader.
	 */
	#define SPECK_NF        50    /* A noise scaling factor      */
	#define SPECK_CUTOFF    .45   /* Cutoff for 'tops' of specks */
	speck = pow(smoothstep(SPECK_CUTOFF, 1,
	            noise(SPECK_NF*PP*txtscale/TSCALE)), 3);

	/*
	 * Determine where the apple goes black at the poles.
	 * This simulates the spots where the stalk would be
	 * attached, and where the wierd bit at the very bottom
	 * is.
	 */
	#define BEDGE  .01
	#define BWIDTH .01
	blackness = 1-(smoothstep(BEDGE, BEDGE+BWIDTH, t)*
	               smoothstep(BEDGE, BEDGE+BWIDTH, 1-t));


	/*
	 * Combine what we have so far to set the surface
	 * color.
	 */
	cs = mix(base_color, dred, blotch);
	cs = mix(cs, brown, speck);
	cs = mix(cs, black, blackness);


	/*
	 * Set the shading surface normal.
	 *
	 * Here we set the surface normal to fix up the specular
	 * highlights. We'd like them perturbed by the small
	 * noise, affected a little by the brown specks, and
	 * also dented a bit (alas, no real apples are perfectly
	 * un-dented).
	 */
	#define DISP_SF     10             /* s-factor for dent noise */
	#define DISP_TF     30             /* t-factor for dent noise */
	#define DISP_DENT_AMP     (1/15)   /* dent amplitude          */
	#define DISP_SMNOISE_AMP  (1/1000) /* small noise amplitude   */
	#define DISP_SPECK_AMP    (1/40)   /* speckle disp. amplitude */
	disp += noise(DISP_SF*sin(2*PI*s) + PI + label,
	              DISP_TF*t + label) * DISP_DENT_AMP * txtscale / TSCALE;
	disp += small_noise * DISP_SMNOISE_AMP * txtscale / TSCALE;
	disp -= speck * DISP_SPECK_AMP * txtscale / TSCALE;
	newP = calculatenormal(P + disp * normalize(N));
	Nf = faceforward(normalize(newP), I);


	/*
	 * 'Mirror' reflections
	 *
	 * Here, we raytrace for the slight mirrored reflections
	 * in the surface of an apple. They don't add much, but
	 * may be needed for the 'perfect' apple :-).
	 *
	 * Note: You'll need Larry Gritz's raytrace helper files
	 * for this bit!
	 */
#ifdef USE_LG_RAYTRACE
	if (Kr > .01) {
		Rdir = normalize(reflect(normalize(I), Nf));
		reflect = RayTrace(P, Rdir, 0, 1, 1);
	} else {
		reflect = 0;
	}
#else
	reflect = 0;
#endif


	/*
	 * Combine everything to get Ci, in the standard form.
	 */
	Oi = Os;
	Ci = Os * (cs * (Ka*ambient() + Kd*diffuse(Nf)) +
	           specularcolor * (Kr * reflect +
	                            Ks * specular(Nf, V, roughness)));
		
}



