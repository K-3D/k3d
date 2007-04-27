/* matte.sl - Standard matte surface for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

surface k3d_matte(float Ka = 1;
		  float Kd = 1;)
{
  normal Nf = faceforward(normalize(N), I);
  Oi = Os;
  Ci = Os * Cs * (Ka * ambient() + Kd * diffuse(Nf));
}
