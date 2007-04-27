/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.370 */
/* Listing 16.27  Displacement shader with catenary droop in y*/

/*
 * droop(): a displacement shader for making a surface "sag" along t.
*/
#include "k3d_constants.h" 

displacement 
k3d_droop ( 
	float	Km = 0.05 )
{
	float droop, yDel;
 
	droop = (t-.5)*2;	/* t in [0,1] goes to droop in [-1,1] */

	yDel = -Km * ( M_E + (1/M_E) - (exp(droop)+exp(-droop)) );
	setycomp(P, ycomp(P) + yDel);
	N = calculatenormal(P);
}
