/* ambientlight.sl - Standard ambient light source for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

light k3d_ambientlight(float intensity = 1;
		       color lightcolor = 1;
  )
{
  Cl = intensity * lightcolor;
  L = 0;
}

