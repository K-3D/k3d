/* plastic.sl - Standard plastic surface for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

surface k3d_plastic(
	float Ks = 0.5; 
	float Kd = 0.5; 
	float Ka = 1;
	float roughness = 0.1;
	color specularcolor = 1;
	)
{
	normal Nf = faceforward(normalize(N), I);
	normal V = normalize(-I);

	Oi = Os;
	Ci = Os * (Cs * (Ka * ambient() + Kd * diffuse(Nf)) + specularcolor * Ks * specular(Nf, V, roughness));
}

