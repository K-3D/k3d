/* plastic.sl - Standard plastic surface for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

surface k3d_plastic(float Ka = 1;
		    float Kd = .5; float Ks = .5; float roughness = .1;
		    color specularcolor = 1;
  )
{
  normal Nf = faceforward(normalize(N), I);
  Oi = Os;
  Ci =
    Os * (Cs * (Ka * ambient() + Kd * diffuse(Nf)) +
	  specularcolor * Ks * specular(Nf, -normalize(I), roughness));
}
