/* shinymetal.sl - Standard metal with environment mapping for
 * RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

surface k3d_shinymetal(float Ka = 1;
		       float Ks = 1; float Kr = 1; float roughness = .1;
		       string texturename = "";)
{
  normal Nf;
  vector V, D;
  color env;

  V = normalize(I);
  Nf = faceforward(normalize(N), V);
  D = reflect(V, Nf);
  D = vtransform("world", D);

  if(texturename != "")
    env = Kr * color environment(texturename, D);
  else
    env = 0;

  Oi = Os;
  Ci = Os * Cs * (Ka * ambient() + Ks * specular(Nf, -V, roughness) + env);
}
