/*
 * TLd_cell1
 *
 * Simple displacement shader using Voronoi cell noise
 * Uses noises.h from ARMAN
 *
 * Tal Lancater 12/15/01
 *
 * History
 *   01/12/15  Created
 *
 */

#include "k3d_noises.h"

displacement k3d_celld (
        float Kvoro = 0;        /* desc {amount to displace. } */
	float voro_freq = 1;    /* desc {Feature size } */
	float voro_step = 0.05; /* desc {Step value size.  If the difference
				   between f2 and f1 is less then this value
				   then the result is 0. } */
	float voro_jitter = 0;  /* desc { Amount to perturb the voroni 
				   function } */
	string PSpace = "shader";  /* desc {Space to transform P for shading calculations } */
	float usePref = 0;   /* type switch */
	varying point __Pref = point (1000, 0, -1000); /* vis hidden */
   	output varying float VoroVal = 0; /* vis hidden */
)
{
	point objP = transform (PSpace, P);
	point noiseP = (usePref != 0)? transform (PSpace, __Pref): objP;
	normal Nn = ntransform (PSpace, N);
	Nn = normalize (Nn);

	float voro_f1 = 0, voro_f2 = 0;
	point voro_pos1 = 0, voro_pos2 = 0;

	voronoi_f1f2_3d (noiseP*voro_freq, voro_jitter, 
			 voro_f1, voro_pos1, voro_f2,
			 voro_pos2);

	float voro_dist = 1 - step (voro_step, voro_f2 - voro_f1);
#if 0
	if (acos(N.I) >= 90)
		voro_dist = 0;
#endif
	VoroVal = voro_dist;
	objP += Kvoro * voro_dist * Nn;

#pragma nolint 1
	P = transform (PSpace, "current", objP);
	N = calculatenormal (P);
}
