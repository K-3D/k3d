/*
 * water.sl -- water surface, using ray tracing.
 * This was originally shiny.sl, but I change it to make it transparant.  The interesting part colors that
 * are not blue will pass thru the water.
 *
 * note : This is very similar to shiny.sl with a slight modification to give it a
 *        water like look.
 *
 * modified by Lawrence D. Chin, cs184-bo
 */

/* DESCRIPTION:
 *   Makes a smoothly polished metal, using ray tracing to calculate
 *   reflections of the environment.
 * 
 * PARAMETERS:
 *    Ka, Kd, Ks, roughness, specularcolor - The usual meaning
 *    Kr - coefficient for mirror-like reflections of environment
 *    blur - how blurry are the reflections? (0 = perfectly sharp)
 *    samples - set to higher than 1 for oversampling of blur
 *
 *
 * HISTORY:
 *      Aug 1991 -- written by lg in C
 *      25 Jan 1994 -- recoded by lg in correct shading language.
 *
 * last modified 25 Jan 1994 by Larry Gritz
 */

#include "k3d_rmannotes.h"

surface
k3d_water ( float Ka = 0, Kd = 0, Ks = 1;
	float Kr = 1, roughness = 0, blur = 0;
	color specularcolor = 1;
	float samples = 0,
        radius = 8,           /* radius of ring */
        half_width = 0.1;      /* half width of ring */
      )
{
    /* FIRST LAYER */
    
    normal Nf;               /* Forward facing normal vector */
    vector IN;               /* normalized incident vector */
    vector uoffset, voffset; /* Offsets for blur */
    color surface_color,     /* Resulting color */
          ev;                /* Color of the reflections */
    vector R, Rdir;          /* Direction to cast the ray */
    uniform float i, j;
    
    /* Construct a forward facing surface normal */
    Nf = faceforward (normalize(N), I);
    IN = normalize (I);
    ev = 0;

    /* Calculate the reflection color */
    if (Kr > 0.001) {
	/* Rdir gets the perfect reflection direction */
	Rdir = normalize (reflect (IN, Nf));
	if (blur > 0) {
	    /* Construct orthogonal components to Rdir */
	    uoffset = blur * normalize (vector (zcomp(Rdir) - ycomp(Rdir),
					        xcomp(Rdir) - zcomp(Rdir),
					        ycomp(Rdir) - xcomp(Rdir)));
	    voffset = Rdir ^ uoffset;
	    for (i = 0;  i < samples;  i += 1) {
		for (j = 0;  j < samples;  j += 1) {
		    /* Add a random offset to the smooth reflection vector */
		    R = Rdir +
			((i + float random())/samples - 0.5) * uoffset +
			((j + float random())/samples - 0.5) * voffset;
		    ev += trace (P, normalize(R));
		}
	    }
	    ev *= Kr / (samples*samples);
	} else {
	    /* No blur, just do a simple trace */
	    ev = Kr * trace (P, Rdir);
	}
    }

    surface_color = Os * ( Cs * (Ka*ambient() + Kd*diffuse(Nf)) +
		specularcolor * (ev + Ks*specular(Nf,-IN,roughness)));

    /* I added some current.  Note, it doesn't apppear horizontally. */
    color current = (1,1,1);  
   
    color layer_color = (1,1,1);
    color layer_opac = 1;
    float fuzz = 0.025;
    point center;
    float d;

    center = (0.5, 0.5, 0);  /* position of ring */
    d = distance(center, (s, t, 0));
    layer_opac = pulse(radius - half_width, radius + half_width, fuzz, d);
    surface_color = blend(surface_color, layer_color, layer_opac);

    Ci = surface_color;
     
    /* This gives the water a somewhat transparant look. 
     *   Note: This does works with radiosity.
     */

    Oi = (0, 0, 0.0000001);
    
}

