/* constant.sl - Standard constant surface for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

surface k3d_constant(float intensity = 1.0)
{
  Oi = Os;
  Ci = Os * Cs * intensity;
}
