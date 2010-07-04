/* distantlight.sl - Standard distant light source for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

light k3d_distantlight(float intensity = 1;
		       color lightcolor = 1;
		       point from = point "shader"(0, 0, 0);
		       point to = point "shader"(0, 0, 1);)
{
  solar(to - from, 0) Cl = intensity * lightcolor;
}
