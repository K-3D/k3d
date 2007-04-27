/* Based on paintedplastic.sl - Standard texture map surface for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 *
 * DESCRIPTION:
 *    Apply a texture map to a plastic surface, indexing the texture
 *    by the s,t parameters of the surface.
 *    Has optional parameters to change texture coordinate bounds.
 *    
 *
 * PARAMETERS:
 *    Ka, Kd, Ks, roughness, specularcolor - the usual meaning.
 *    texturename - the name of the texture file.
 *    s0, s1, t0, t1 - texture coordinate bounds.
 *    Uses alpha channel value to blend texture.
 */

surface k3d_paintedplastic_alpha(
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float roughness = 0.1;
	color specularcolor = 1.0;
	string texturename = "";
	float s0 = 0.0;
	float s1 = 1.0;
	float t0 = 0.0;
	float t1 = 1.0;
	)
{
	color Ct = color(1, 1, 1);
	float Ot = 1;
	
	if(texturename != "")
		{
			float ss = mix(s0, s1, s);
			float tt = mix(t0, t1, t);
			
			Ct = color texture(texturename, ss, tt);
			Ot = float texture(texturename[3], ss, tt);
		}

	normal Nf = faceforward(normalize(N), I);
	vector V = -normalize(I);
	Oi = Os * Ot;
	Ci = Oi * Cs * Ct * (Ka * ambient() + Kd * diffuse(Nf)) + specularcolor * Ks * specular(Nf, V, roughness);
}
