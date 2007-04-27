/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.380 */
/* Listing 16.33  Spotlight using shadow map*/

/*
 *  shadowspot(): spotlight with an optional shadow map
 */
light k3d_shadowspot( 
	float intensity	= 1;
	color lightcolor = 1;
	point from = point "shader" (0, 0, 0);
	point to = point "shader" (0, 0, 1);
	float coneangle	= radians(30);
	float conedeltaangle = radians(5);
	float beamdistribution = 2;
	string shadowfile = "";
	float samples = 16;
	float blur = 0.01;
	float bias = 0.01; )
{
	point A = (to - from) / length(to - from); /* direction */
	float	cosoutside= cos(coneangle),
			cosinside = cos(coneangle-conedeltaangle);
	float	attenuation, 	/* falloff from center of illumination cone */
		cosangle;	/* cosine of angle wrt center of cone */

	illuminate( from, A, coneangle ) {
		cosangle = L.A / length(L);	/* A is already normalized */
		attenuation = pow(cosangle, beamdistribution) / (L.L);
		attenuation *= smoothstep( cosoutside, cosinside, cosangle );
		if( shadowfile != "" )
		    attenuation *= (1.0 - shadow( shadowfile, Ps, "samples", samples, "blur", blur, "bias", bias ));
		Cl = attenuation * intensity * lightcolor;
	}
}
