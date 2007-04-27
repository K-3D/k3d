/*
 * arealight.sl - light source shader appropriate for an area light source.
 *
 * DESCRIPTION:
 *   Makes light with cosine falloff from the normal of the light source
 *   surface.  This is perfect for using with area light sources.
 *
 * PARAMETERS
 *   intensity, lightcolor - same meanings as pointlight
 *
 */

light k3d_arealight(
	float intensity = 1;
	color lightcolor = 1;
	)
{
	N = normalize(N);

	illuminate(P, N, 1.5707963 /* PI/2 */)
	{
		Cl = intensity * lightcolor * (N.normalize(L)) / (L . L);
	}
}

