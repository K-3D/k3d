/* defaultsurface.sl - just color the surface, don't even look at lights
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

surface
k3d_defaultsurface(float Kd = .8, Ka = .2)
{
  float d = normalize(I).normalize(N);
  Ci = Os * Cs * (Ka + Kd * d * d);
  Oi = Os;
}
