/* fog.sl - Standard fog volume shader for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

volume k3d_fog(float distance = 1; color background = 0;)
{
  float d = 1 - exp(-length(I) / distance);
  Ci = mix(Ci, background, d);
  Oi = mix(Oi, color(1, 1, 1), d);
}
