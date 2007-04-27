/*
 * rustymetal.sl -- metal with specks of rust
 *
 * DESCRIPTION:
 *   A rough metal surface with controllable rust spots.  The rust pattern
 *   is basically thresholded turbulence (summed abs(snoise)).  Where it's
 *   rusty, shade like rust colored matte, and also make it bumpy (like
 *   the corrosion is kind of grainy).  Where there is no rust, shade like
 *   regular metal.  All computations are done in shader space.
 *
 * PARAMETERS
 *   metalKa, metalKs, metalroughness - control the appearance of the metal.
 *   rustKa, rustKd, rustcolor - control the appearance of the rust.
 *   txtscale - overall scaling factor of the rust pattern.
 *   rusty - 0=no rust, larger for more rust, 1=completely rusty
 *   rustbump - controls the "bumpiness" of the rusty areas.
 *
 * ANTIALIASING:
 *   The fractal sum used to determine the rust pattern chooses a number of
 *   octaves to sum based on the shader sampling rate.  This helps to keep
 *   aliasing under control.
 *
 * AUTHOR: Larry Gritz, gritz@SpamSucks_seas.gwu.edu
 *         The George Washington University
 *
 * HISTORY:
 *   19 Jan 1995 - gritz - created
 *
 *   last modified 19 Jan 95 
 */



#include "k3d_noises.h"

/* Maximum number of octaves */
#define MAXOCTAVES 8




surface
k3d_rustymetal (float metalKa = 1, metalKs = 1, metalroughness = .1;
	    float rustKa = 1, rustKd = 1;
	    color rustcolor = color (.437, .084, 0);
	    float txtscale = 1;
	    float rusty = 0.2;
	    float rustbump = 0.035;
    )
{
  point Nf, V;                 /* normal and view vector used for shading */
  point Nrust;                 /* perturbed normal for the rusty areas */
  point PP;                    /* shade space point */
  float i, sum = 0, a = 1;     /* Loop control for fractal sum */
  float alimit;                /* Limit sum to do simple antialiasing */
  float rustiness;             /* Result: how rusty is this point? */
  color Cmetal = 0, Crust = 0; /* Computed colors of metal & rust */

  /* Sum several octaves of abs(snoise), i.e. turbulence.  Limit the
   * number of octaves by the estimated change in PP between adjacent
   * shading samples.
   */
  PP = txtscale * transform ("shader", P);
  alimit = sqrt (area(PP));
  for (i = 0;  i < MAXOCTAVES  &&  a > alimit;  i += 1) {
      sum += a * abs(snoise(PP));
      PP *= 2;
      a /= 2;
    }
  /* If it's rusty, also add a high frequency bumpiness to the normal */
  Nrust = calculatenormal (P + rustbump * snoise(PP) * normalize(N));

  /* Scale the rust appropriately, modulate it by another noise 
   * computation, then sharpen it by squaring its value.
   */
  rustiness = step (1-rusty, clamp (sum,0,1));
  rustiness *= clamp (abs(snoise(PP)), 0, .08) / 0.08;
  rustiness *= rustiness;

  /* If we have any rust, calculate the color of the rust, taking into
   * account the perturbed normal and shading like matte.
   */
  if (rustiness > 0) {
      Nf = faceforward (normalize(Nrust),I);
      Crust = rustcolor * (rustKa*ambient() + rustKd*diffuse(Nf));
    }
  /* If we have any metal, calculate the color of the metal, using the
   * original (smooth) normal and the usual metal illumination model.
   */
  if (rustiness < 1) {
      Nf = faceforward (normalize(N),I);
      V = -normalize(I);
      Cmetal = Cs * (metalKa*ambient() + metalKs*specular(Nf,V,metalroughness));
    }

  /* Now blend the metal and rust colors depending on the computed value
   * of the rustiness.
   */
  Oi = Os;
  Ci = Oi * mix (Cmetal, Crust, rustiness);
}
