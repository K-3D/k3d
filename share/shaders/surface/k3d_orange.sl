/*
 * Actually it is a condensed version of the mango shader from Sig '92 "Writing RenderMan Shaders"
 */

/*
 * TLOrange.sl -- create an orange
 *
 * DESCRIPTION:
 *	Creates an stippled orange surface.
 *
 * PARAMETERS:
 *	Ka, Kd, Ks, roughness - the usual
 *
 * HINTS:
 *
 * AUTHOR: Tal Lancaster
 *	tal@SpamSucks_cs.caltech.edu
 *
 * History:
 *	Created: 9/4/96
 */

#define BUMP_AMPLITUDE (1/30)
#define BUMP_FREQUENCY (35)
 
surface k3d_orange (
	float Ka = 1;
	float Kd = .8;
	float Ks = .8;
	float roughness = .2;
)
{
	point Nf, V;
	point turb, p2;
	point newP;
	
	
	setxcomp (p2, sin (2 * PI * s));
	setycomp (p2, 2*t);
	setzcomp (p2, cos (2 * PI * s));
	

	turb = noise (BUMP_FREQUENCY * p2) * BUMP_AMPLITUDE;
	newP = calculatenormal (P + turb * normalize (N));
	Nf = faceforward (normalize (newP), I);
	V = -normalize (I);
	
	Oi = 1;
	Ci = Cs * (Ka * ambient() + Kd * diffuse (Nf)) + Ks * specular (Nf, V, roughness);
}
