/**************************************************************************
 * TLSmoke.sl -- This is really Larry's noisysmoke.sl.  I only took out the
 * random() as it made my animation look to funky -- tal 9/4/96
 *
 * Description:
 *    This is a volume shader for smoke.  Trapezoidal integration is
 *    used to integrate the GADD to find scattering and extinction.
 *
 * Parameters:
 *   density - overall smoke density control
 *   integstart, integend - bounds along the viewing ray direction of the
 *          integration of atmospheric effects.
 *   stepsize - step size for integration
 *   use_lighting - if nonzero, light visibility along the ray will be taken
 *          into account.
 *   use_noise - makes the smoke noisy (nonuniform) when nonzero
 *   freq, octaves, smokevary - control the fBm of the noisy smoke
 *   lightscale - multiplier for light scattered toward viewer in volume
 *   debug - if nonzero, copious output will be sent to stderr.
 *
 * Author: Larry Gritz
 *
 * History:
 *   9/4/96 tal -- took out random call and renamed to TLSmoke
 *
 * $Revision: 1.3 $     $Date: 2006/03/14 02:20:56 $
 *
 * $Log: k3d_smoke2.sl,v $
 * Revision 1.3  2006/03/14 02:20:56  dsmatthews
 * ENH: Removed all duplicate #define and added correct #include moved some #define to new  .h files
 *
 * Revision 1.2  2006/03/08 16:01:23  dsmatthews
 * PERF: removed all #ifdef BMRT blocks from shaders.
 *
 * Revision 1.1  2006/02/25 20:11:56  tshead
 * ENH: Moved shaders into separate directories based on type
 * ENH: Shader paths are stored in user options based on type
 * ENH: Path properties can specify file filters, shaders filter on *.sl
 * ENH: Updated user options to work with newer versions of Pixie
 *
 * Revision 1.1  2004/05/19 18:15:20  tshead
 * * Moved scripts & shaders to the share directory
 *
 * Revision 1.1  2003/02/11 14:32:43  blackburst
 * * Initial checkin
 *
 * Revision 1.3  1996-03-01 17:07:40-08  lg
 * Eliminated duplicate local variable declarations
 *
 * Revision 1.2  1996-02-29 18:04:26-08  lg
 * Compute only one octave of noise when not lit (big speedup)
 *
 * Revision 1.1  1996-02-05 11:03:45-08  lg
 * Initial RCS revision
 *
 **************************************************************************/


#include "k3d_noises.h"


/* Here is where we define the GADD. */
#define GADD(PP,PW,li,g)                                                    \
         if (use_lighting > 0) {                                            \
	     li = 0;                                                        \
	     illuminance (PW, point(0,0,1), PI) { li += Cl; }               \
	 } else { li = 1; }                                                 \
         if (use_noise != 0) {                                              \
             Psmoke = PP*freq;                                              \
             smoke = snoise (Psmoke);                                       \
             /* Optimize: one octave only if not lit */                     \
	     if (comp(li,0)+comp(li,1)+comp(li,2) > 0.01) {                 \
                 f=1;                                                       \
                 for (i=1;  i<octaves;  i+=1) {                             \
                      f *= 0.5;  Psmoke *= 2;                               \
                      smoke += f*snoise(Psmoke);                            \
                 }                                                          \
             }                                                              \
             g = density * smoothstep(-1,1,smokevary*smoke);                \
         } else g = density;                                                \







volume
k3d_smoke2 (float density = 60;
	    float integstart = 0, integend = 100;
	    float stepsize = 0.1;
	    float debug = 0;
	    float use_lighting = 1;
	    float use_noise = 1;
	    color scatter = 1;   /* for sky, try (1, 2.25, 21) */
	    float octaves = 3, freq = 1, smokevary = 1;
	    float lightscale = 15;
	    )
{

  point Worigin = P - I;
  point incident = vtransform ("shader", I);

  point origin = transform ("shader", Worigin);
  point IN, WIN;
  float d, sigma, tau;
  color Cv = 0, Ov = 0;           /* net color & opacity of volume */
  color dC, dO;                   /* differential color & opacity */
  float ss, dtau, last_dtau, end;
  float nsteps = 0;          /* record number of integration steps */
  color li, last_li, lighttau;
  point PP, PW, Psmoke;
  color scat;
  float f, i, smoke;

  end = min (length (incident), integend) - 0.0001;

  /* Integrate forwards from the start point */
  d = integstart + /*random()* */ stepsize;
  if (d < end) {
      IN = normalize (incident);
      WIN = vtransform ("shader", "current", IN);
      PP = origin + d * IN;
      PW = Worigin + d * WIN;
      GADD (PP, PW, li, dtau)
      ss = min (stepsize, end-d);
      d += ss;
      nsteps += 1;

      while (d <= end) {
	  last_dtau = dtau;
	  last_li = li;
	  PP = origin + d*IN;
	  PW = Worigin + d*WIN;
	  GADD (PP, PW, li, dtau)
	  /* Our goal now is to find dC and dO, the color and opacity
	   * of the portion of the volume covered by this step.
	   */
	  tau = .5 * ss * (dtau + last_dtau);
	  lighttau = .5 * ss * (li*dtau + last_li*last_dtau);

	  scat = -tau * scatter;
	  dO = 1 - color (exp(comp(scat,0)), exp(comp(scat,1)), exp(comp(scat,2)));
	  dC = lighttau * dO;

	  /* Now we adjust Cv/Ov to account for dC and dO */
	  Cv += (1-Ov)*dC;
	  Ov += (1-Ov)*dO;

	  ss = max (min (ss, end-d), 0.005);
	  d += ss;
	  nsteps += 1;
        }
    }

  /* Ci & Oi are the color (premultiplied by opacity) and opacity of 
   * the background element.
   * Now Cv is the light contributed by the volume itself, and Ov is the
   * opacity of the volume, i.e. (1-Ov)*Ci is the light from the background
   * which makes it through the volume.
   */
  Ci = lightscale*Cv + (1-Ov)*Ci; 
  Oi = Ov + (1-Ov)*Oi;
  
  if (debug > 0) {
      printf ("nsteps = %f, t1 = %f, end = %f\n", nsteps, integstart, end);
      printf ("   Cv = %c, Ov = %c\n", Cv, Ov);
    }
}
