/*
 *	P			surface position
 *	N			surface geometric normal
 *	s, t		surface texture coordinates
 *	Ng			surface geometric normal
 *	u,v			surface parameters
 *	du, dv		change in u, v across the surface
 *	dPdu,dPdv	change in position with u and v
 *	I			direction of ray stricking a surface point (from the camera)
 *	E			position of the camera
 */

displacement k3d_waterdisplacement (float Km = 1.0; string bumpmap = "")
{
	float	hump;
	point	norm;
	
	/* STEP 1 - Make a copy of the surface normal */
	norm = normalize (N);
	
	/* STEP 2 - Calculate an appropriate value for the displacement */
	if (bumpmap != "")
		hump = texture(bumpmap, s, t);	
	
	/* STEP 3 - Calculate a new position of the surface point, "P" */
	P = P - norm * hump * Km;
	
	/* STEP 4 - Recalculate the surface normal */
	N = calculatenormal (P);
}


