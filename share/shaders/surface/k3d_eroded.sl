/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.353 */
/* Listing 16.17  Surface shader eroding the surface of an object */

/*
 * eroded(): Simulate a metallic surface eaten away with acid
 */
surface
k3d_eroded (
	float Ks = 0.4,
	      Ka = 0.1,
		  Km = 0.3,
		  roughness = 0.25)
{
	float size = 4.0,
	      magnitude = 0.0,
		  i;
	point Nf,
	      W = transform ("object", P);
    point x = (1, 0, 0);
	point V = normalize(-I);
	
	for (i = 0; i < 6.0; i += 1.0) {
		/* Calculate a simple fractal 1/f noise function */
		magnitude += 4.0 * abs (.5 - noise (W * size)) / size;
		size *= 2.0;
	}
	
	/* sharpen peaks */
	magnitude = magnitude * magnitude * magnitude * Km;
	
	N = calculatenormal (P - magnitude * normalize(N));
	
	Nf = faceforward (normalize (N), I);
	Oi = smoothstep (0.0001, 0.003, magnitude);
	Ci = Oi * Cs * (Ka * ambient() + Ks * specular (Nf, V, roughness) );
}
