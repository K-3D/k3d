/* indirect.sl - retrieve radiosity data from the "ray server"
 * 
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 *
 * [REVIEW]
 */

#include "k3d_rayserver.h"


light k3d_indirect(output float __nonspecular = 1;)
{
/* PRMan - make a light that looks up from the ray server */

  /* Exploit the fact that PRMan (incorrectly) leaves the surface normal
   * in N.
   */
  vector axis = normalize(N);
  solar(-axis, 0)
  {
    Cl = rs_indirect(Ps, axis);
  }
}
