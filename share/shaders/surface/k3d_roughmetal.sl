/*
 * roughmetal.sl
 *
 * Description:
 *   Rough metal without coherent reflections
 *
 * Parameters:
 *   Ka, Kd, Ks - ambient, diffuse, specular weights
 *   roughness - highlight width
 * 
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

#include "k3d_material.h"


surface k3d_roughmetal(float Ka = 1, Kd = 0.1, Ks = .9, roughness = 0.4;)
{
  normal Nf = faceforward(normalize(N), I);
  Ci = MaterialRoughMetal(Nf, Cs, Ka, Kd, Ks, roughness);
  Oi = Os;
  Ci *= Oi;
}
