/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.384 */
/* Listing 16.37  Pencil-labeling surface shader*/

/*
 * sdixon(): Paint the body of a pencil.
 */
surface
k3d_sdixon(
	float	Ka		= 1.0, 
		Kd		= 1.0, 
		Ks		= 1.0,
	 	roughness 	= .25;
	color	green 		= color(0, .2, 0), 
		yellow 		= color(.56, .23, 0);
	string	texturename	= "")
{
	point Nf = faceforward(normalize(N),I);
	point V = normalize(-I);
	float ink;
	color cout;

	/* This shader uses a single-channel texture map to apply a 
	 *   metallic-green ink to a matte-yellow background. */

	/* Get the amount of ink from texture file. */
	ink = texture(texturename, s, t);

 	/* Use ink to mix yellow and green */
	cout = mix(yellow, green, ink); 					 

	/* Compute the output color. Notice that as ink goes from zero to
	   one, the diffuse component goes to zero and the specular 
	   component is increased. This has the effect of transitioning
	   from a matte surface to a metallic one as ink is added. */
	Oi = Os;
	Ci = Os * cout * ( Ka*ambient() + (1-ink)*Kd*diffuse(Nf) +
					ink*Ks*specular(Nf,V,roughness) ) ;
}
