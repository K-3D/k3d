/*  ripple3.sl
 *  
 *  Note: I modified a given code from Renderman Interface 3.1
 *   
 *  This produced concave displacements on any surfaces.
 *
 *    by Lawrence D. Chin, cs184-bo
 */



displacement
k3d_ripple ( float amplitude = 1.0,
	       wavelength = 0.25,
	       fac = 1,
	       size = 5; )
{
  float ss, tt;
  
  P += 1 + N * amplitude * (2 - abs (sin(2*PI*(s/wavelength))));
  N = calculatenormal(P);
}
