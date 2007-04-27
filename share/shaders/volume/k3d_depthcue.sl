/* depthcue.sl - Standard depthcue volume shader for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

volume k3d_depthcue(float mindistance = 0, maxdistance = 1;
		    color background = 0;)
{
  float d;

  d = clamp((depth(P) - mindistance) / (maxdistance - mindistance), 0, 1);
  Ci = mix(Ci, background, d);
}
