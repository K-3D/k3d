/*
 * dented.sl -- displacement shader for dents
 *
 * DESCRIPTION:
 *   Makes a surface appear worn and dented.
 * 
 * PARAMETERS:
 *   Km 	   	the amplitude of the dents
 *   power	   	controls the fractal dimension of the dents (1 looks
 *                         like previously crumpled paper or cloth, 3 looks
 *                         like worn metal).
 *   frequency  	the frequency of the dents
 *
 *
 * AUTHOR: written by Larry Gritz, based on the "dented" shader in
 *         RenderMan Companion, but with more control parameters.
 *
 * HISTORY:
 *      Dec 1992 -- written by lg for "Timbre Trees Examples" (jingle)
 *
 * last modified  12 Jan 1994 by Larry Gritz
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */



displacement k3d_dented( float Km = 1; float power = 3; float frequency = 1; float maxoctaves = 6; )
{
	float size;
	float magnitude = 0;
	float i;
	point PP;

	PP = transform ("shader", P);
	size = frequency;
	for(i = 0;  i < maxoctaves;  i += 1)
		{
			magnitude += abs(0.5 - noise(PP*size)) / size;
			size *= 2;
		}
	P = P - (Km * pow(magnitude, power)) * normalize(N);
	N = calculatenormal(P);
}

