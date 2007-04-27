/* screen.sl - RenderMan compatible shader for a metalic screen.
 * (c) Pixar (appears in _RenderMan Companion_)
 *
 *
 * DESCRIPTION:
 *   Makes a surface that looks like a metal screen.  Strips of metal run
 *   parallel to lines of s and t.  You can adjust the Ka, Kd, Ks, etc.
 *   to change the material appearance.
 * 
 * PARAMETERS:
 *   Ka, Kd, Ks, roughness, specularcolor - work just like the plastic shader
 *   frequency - how many cycles of screen in st space
 *   density - how much of each cycle is opaque?
 *
 * WARNINGS:
 *   No antialiasing is performed here.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 *
 */

surface k3d_screen(float Ka = 1, Kd = 0.75, Ks = 0.4, roughness = 0.1;
		   color specularcolor = 1;
		   float density = 0.25, frequency = 20;)
{
  normal Nf;

  if(mod(s * frequency, 1) < density || mod(t * frequency, 1) < density)
    {
      Oi = 1;
      Nf = faceforward(normalize(N), I);
      Ci =
	Os * (Cs * (Ka * ambient() + Kd * diffuse(Nf)) +
	      specularcolor * Ks * specular(Nf, -normalize(I), roughness));
    }
  else
    {
      Oi = 0;
      Ci = 0;
    }
}
