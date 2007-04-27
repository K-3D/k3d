/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.352 */
/* Listing 16.18  Surface shader for granite-like surface*/

/* 
 * granite(): Provide a diffuse granite-like surface texture. 
 */
surface 
k3d_granite(
	float	Kd	= .8,
		Ka	= .2 )
{
	float sum = 0;
	float i, freq = 1.0; /* Try other values for example, 7.0 */

	for (i = 0; i < 6; i = i + 1) {
		sum = sum + abs(.5 - noise( 4 * freq * I))/freq ;
		freq *= 2;
	}
	Ci = Cs * sum * (Ka + Kd * diffuse(faceforward( normalize(N), I )) ) ;
}
