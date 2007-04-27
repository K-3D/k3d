/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.363 */
/* Listing 16.22  Surface shader for use without a light source*/

/*
 * easysurface(): orientation-sensitive surface shading without a light source
 */
surface
k3d_easysurface(
	float	Kd	= .8, 
		Ka	= .2, 
		falloff	= 2.0 )
{
	float diffuse ;
	point II = normalize(I);
	point NN = normalize(N);

	diffuse = II.NN / (II.II * NN.NN);
	diffuse = pow(diffuse, falloff);

	Ci = Cs * (Ka + Kd * diffuse ) ;
}
