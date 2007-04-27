/* pointlight.sl - Standard point light source for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

light k3d_pointlight(float intensity = 3000;
		     color lightcolor = 1;
		     point from = point "shader"(0, 0, 0);)
{
  illuminate(from) Cl = intensity * lightcolor / (L.L);
}
