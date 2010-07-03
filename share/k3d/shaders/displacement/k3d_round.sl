/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.364 */
/* Listing 16.23  Displacement shader for bevelling perpendicular bilinear patches*/

/* 
 * round(): displace the edge of a bilinear patch so that, if it is placed 
 * next to another patch at a right angle, the edge will be rounded.
 */
displacement
k3d_round (float radius = .10 )
{
	float	 uu,	/* distance in u to the nearest "vertical" edge */
		 vv,	/* distance in v to the nearest "horizontal" edge */
		 lu,	/* "real" distance to the nearest "vertical" edge */
		 lv;	/* "real" distance to the nearest "horizontal" edge */
	point	 center,/* point toward which the surface is displaced 	*/
		 dpdu,	/* dPdu pointed toward patch center line 	*/
		 dpdv;	/* dPdv pointed toward patch center line 	*/

	/* Find the distance in parameter space from the nearest edge in
	   u and in v, and the directions away from those edges. */
	if (u < .5) {
		uu = u;
		dpdu = dPdu;
	} else {
		uu = 1 - u;
		dpdu = -dPdu;
	}
	if (v < .5) {
		vv = v;
		dpdv = dPdv;
	} else {
		vv = 1 - v;
		dpdv = -dPdv;
	}

	/* Find the distances from the edges in the current space. */
	lu = length(dPdu*uu);
	lv = length(dPdv*vv);

	if (lu < radius || lv < radius) {	/* only if within radius of 
	                                                     an edge...  */
	/*
	 * Find the point towards which the surface  point will be 
	 *  moved. This center is on the center line of a cylinder, if we 
	 *  are not near the corner of the patch, or is the center of a 
	 *  sphere, if we are. We move `center' to the nearest inflection 
	 *  edge along u and/or v.
	 */
		center = point(0,0,0);
		if (lu < radius)
			center = (radius-lu) * normalize(dpdu);
		if (lv < radius)
			center += (radius-lv) * normalize(dpdv);
		/* Move center perpendicular to the surface */
		center += P - radius*normalize(N);
		/* Make P be distance 'radius' along the line 
                 * from 'center' to P */
		P = center + radius*normalize( P-center );
	}
	N = calculatenormal(P);
}
