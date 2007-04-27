/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.368 */
/* Listing 16.25  Surface shader to make a cylinder look like a filament */

/*
 * filament(): map a filament-like spiral onto the surface of a cylinder.
 */
surface 
k3d_filament ( 
	float	frequency	= 5.0,
		phase 		= 0.0, 
		width		= 0.3 )
{
	/* Calculate the distance of (s,t) from a spiral as a fraction [0,1] */
	float offset = mod((t*frequency + s + phase), 1.0);

	/* Threshold the fraction against the fractional filament width */
	if (offset < width) {
		Ci = Cs;
		Oi = 1.0;
	} else {
		Ci = 0.0;
		Oi = 0.0;
	}
}

