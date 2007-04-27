/* Renamed shader to PQCrayon for RMR -- talrmr@SpamSucks_pacbell.net */

/* crayon.sl - a surface shader making crayon like marks

	DESCRIPTION

	This shader makes the surface look as if it had been shaded with a pastel crayon.
	It makes an attempt at antaliasing.
	
	PARAMETERS
	
	Ka, Kd, Ks, roughness, specularcolor - work as in the plastic shader
	txtscale  - an overall scaling factor
	width - the width of the crayon strokes - this is scaled by txtscale
	micro - the size of the dots that make up a crayon stroke, relative to the size of
		the stroke. By default they are about 15 times smaller
	stretch - the length of the stroke relative to its width;
	density0 - controls the amount of topcolor seen - measured as a proportion - 
		this should vary between 0 (no topcolor) to 1, .3 would give 30% topcolor;
	density1 - if different density0 this is the density when t = 1, with a smooth
		interpolation of values for density0 when t = 0, thus allowing a 
		graduation of shading from top to bottom of the object
	color topcolor, basecolor - the color of the crayon strokes and the color of the ground
	
	AUTHOR
	Peter Quint - Revised Monday, January 10, 2000 

 */
 
 
#include "k3d_filterwidth.h"
#include "k3d_noises.h"

float
aanoise(float sp, tp, width)
{
	/* an antaliased noise function, which returns noise of a wavelenth always greater than
	   twice the micropolygon width */
	float f, mag, ns;
	/* calculate smallest integer f for which width / f < .5 */
	f = ceil(width /.5);
	mag = max(pow(0.85, f - 1),.1);
	/*(printf("f = %f, mag = %f\n",f,mag)*/;
	ns  = mag * snoise(sp / f, tp / f) * (1 - smoothstep(0, .5, width / f))
		+ snoise(sp / (f * 1.33), tp / (f * 1.33)) * mag * .25 * smoothstep(0, .5, width / f);
	return ns;
}


surface
k3d_crayon (	float Ka = 1;
        	float Kd = .5;
         	float Ks = .5;
         	float roughness = .1;
	 		color specularcolor = 1;
			float txtscale = 1;
			float width = .05;
			float micro = 15.32;
			float stretch = 10;
			float density0 = .5;
			float density1 = .5;
			color topcolor = 1;
			color basecolor = 0;
		)
{
  color Csurf;
    float density = density0 + t * (density1 - density0);
	/* work out the density for the current t */
	float trs = spline(1 - density, 0 , -0.195997, -0.128361, -0.0738346,	-0.0316483,
			0.00807387,	0.0445915, 0.084543, 0.150693, 0.2198, 0.527474);
	/*  use a spline to read across to the appropriate noise value - this equalisation
		process is described by Steven Worley in Ch 3 of "Texturing and Modelling a
		procedural approach */ 
	normal Nf = faceforward (normalize(N),I);
	float m;
	float fw = max(filterwidth(s), filterwidth(t)); /* the size of the micropolygon */
	float smks = aanoise(txtscale * s * micro / width, txtscale * t * micro / width,
	 		txtscale * fw * micro / width);
	float lmks = (aanoise(txtscale * s / width, txtscale * t / (width * stretch), 
		txtscale * fw / width) + 1) / 2;
	smks = (smks + 1) / 2;
	lmks = lmks - smks;
	m = smoothstep(trs - .1, trs + .3, lmks);	
	m = clamp(m, 0, 1);
	Csurf = mix(basecolor, topcolor, m);
	Oi = Os;
    Ci = Os * ( Csurf * (Ka*ambient() + Kd*diffuse(Nf)) +
		specularcolor * Ks*specular(Nf,-normalize(I),roughness));
}


