/* Glowing ember shader ©2002 Graeme Nattress & NOITAMINANIMATION Inc.
 *
 * email: graeme@noitaminanimation.com
 *
 * It is designed for use with particle systems where the particle is scaled
 * to represent it's life. The desired effect is that of glowing particles that
 * dim and fade as they die.
 *
 * baseSize = the basic diameter of the particle sphere before scaling
 *  
 *	
 */

surface
k3d_ember (	float baseSize = 2;
		float attenuation = 4;
		float roughness = 0.99;
       )
{
	point PP = transform ("world", P);
	point QQ = transform ("world", point "object" (0, 0, 0));
	
	float size = distance (PP, QQ) / baseSize;
	
	//size now represents the size of the sphere particle normalised into 0,1.
	
	normal Nf;
	vector V;
	normal NN, NI;
	
	NN = normalize(N);
	NI = normalize(I);
	Nf = faceforward(NN,I);
	V = vector -NI;
	
	float angle = NN . NI;
	// angle for edge opacity falloff
	
	float spcol = pow (max (0, Nf.NI * -1), 1/roughness);
	// basically, the standard specular function, but so that the highlight is
	// always in the centre of the particle sphere.
							
	Ci = color spline (spcol * size, 
		color (0, 0, 0), color (0, 0, 0),
		color (27, 0, 0),
		color (54, 0, 0),
		color (81, 0, 0),
		color (109, 0, 0),
		color (136, 0, 0),
		color (166, 5, 0),
		color (189, 30, 0),
		color (211, 60, 0),
		color (231, 91, 0),
		color (238, 128, 0),
		color (244, 162, 12),
		color (248, 187, 58),
		color (251, 209, 115),
		color (254, 236, 210),
		color (255, 241, 230),
		color (255, 241, 230)) / 255;
	// the colours for the fire effect from F. Kenton Musgrave's KMFlame.sl
	
	//only back face shade...
	if (angle < 0) {
	
		angle = pow(angle, attenuation);
		
		Ci *= angle;
		Oi = angle * size;
	
	} else {
		Oi = 0;
		Ci *= Oi;
	}
}
