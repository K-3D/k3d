/* skymetal shader 
 * from p. 103 of Siggraph 1991 Course 21
 * The RenderMan Interface and Shading Language
 *
 * NOTE: This shader was created at a time when the shading lanuage
 *   didn't support vector the vector type.  
 *
 * A more general version with some explainations can be found in my
 * TLSkymetal shader.  Actually, I never could get this oneto work right,
 * anyways.
 *
 * tal@SpamSucks_cs.caltech.edu
 */

surface k3d_skymetal (
  float Ka = 1, Ks = 1, Kr = .2;
  float roughness = .1;
    point up = normalize (point "world" (0, 1, 0) - point "world"  (0, 0, 0)); 
             /* actually should be vector and vtransform if using 3.5 or 
			    greater */

)
{
	point Nf;
	color refl;
	float costheta;
	
	color sky_zenith = color (0.125, 0.162, 0.5);
	color sky_horiz = color (.4, .45, .8);
	color land_horiz = color (.0281, 0.0287, 0.0220);
	color land_zenith = color (0, 0, 0);
	
	Nf = normalize (faceforward (N, I));

	costheta = normalize (reflect (I, Nf)) . up;

	if (costheta >= 0.0)
		refl = mix (sky_horiz, sky_zenith, costheta);
	else 
		refl = mix (land_horiz, land_zenith, -costheta);
		
	Oi = Os;	
	Ci = Os * Cs * (Kr*refl + (1-Kr) * (Ka * ambient () +
		Ks * specular (Nf, -I, roughness)));
}
