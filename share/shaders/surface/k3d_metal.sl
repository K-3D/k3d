/* metal.sl - Standard metal surface for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

surface k3d_metal(float Ka = 1;
		  float Ks = 1;
		  float roughness = .1;)
{
  normal Nf = faceforward(normalize(N), I);
  vector V = -normalize(I);
  Oi = Os;
  Ci = Os * Cs * (Ka * ambient() + Ks * specular(Nf, V, roughness));
}
