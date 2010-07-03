/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.385 */
/* Listing 16.38  Rubber surface shader*/

/*
 * rubber(): This shader generates a rubber surface. It is a matte shader that
 * adds in a little white dust to mimic the dust on a new eraser.
 */
surface
k3d_rubber( 
	float	Ka		= 1.0, 
		Kd		= 1.0, 
		txtscale	= 1.5 )
{
	point	Nf = faceforward(normalize(N),I), 
		Ploc = transform("shader", P);  /* Move to shader space */
	color	cout, white=1;

	/* Mix in some white dust. */
	cout = mix(Cs, white, .05* (float noise(txtscale*Ploc)));

	/* Compute a matte surface. */
	Oi = Os;
	Ci = Os * cout * ( Ka*ambient() + Kd*diffuse(Nf) ) ;
}
