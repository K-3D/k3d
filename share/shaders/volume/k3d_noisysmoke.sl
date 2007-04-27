#include "k3d_noises.h"


/* Here is where we define the GADD. */
#define GADD(PP,PW,li,g)                                                    \
         if (use_lighting > 0) {                                            \
	     li = 0;                                                        \
	     illuminance (PW) { li += Cl; }                                 \
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







volume k3d_noisysmoke(float density = 60;
		      float integstart = 0, integend = 100;
		      float stepsize = 0.1; float debug = 0;
		      float use_lighting = 1; float use_noise = 1;
		      color scatter = 1;	/* for sky, try (1, 2.25, 21) */
		      float octaves = 3, freq = 1, smokevary = 1;
		      float lightscale = 15;
  )
{
#if 0
  /* PRMan and BMRT used to have I reverse of each other, conflict in spec */
  point Worigin = P + I;
  vector incident = vtransform("shader", -I);
#else
  /* But with BMRT 2.3.6 and later, they're the same... */
  point Worigin = P - I;
  vector incident = vtransform("shader", I);
#endif
  point origin = transform("shader", Worigin);
  vector IN, WIN;
  float d, tau;
  color Cv = 0, Ov = 0;		/* net color & opacity of volume */
  color dC, dO;			/* differential color & opacity */
  float ss, dtau, last_dtau, end;
  float nsteps = 0;		/* record number of integration steps */
  color li, last_li, lighttau;
  point PP, PW, Psmoke;
  color scat;
  float f, smoke;
  uniform float i;

  end = min(length(incident), integend) - 0.0001;

  /* Integrate forwards from the start point */
  d = integstart + random() * stepsize;
  if(d < end)
    {
      IN = normalize(incident);
      WIN = vtransform("shader", "current", IN);
      PP = origin + d * IN;
      PW = Worigin + d * WIN;
      GADD(PP, PW, li, dtau) ss = min(stepsize, end - d);
      d += ss;
      nsteps += 1;

      while(d <= end)
	{
	  last_dtau = dtau;
	  last_li = li;
	  PP = origin + d * IN;
	  PW = Worigin + d * WIN;
	  GADD(PP, PW, li, dtau)
	    /* Our goal now is to find dC and dO, the color and opacity
	     * of the portion of the volume covered by this step.
	     */
	    tau = .5 * ss * (dtau + last_dtau);
	  lighttau = .5 * ss * (li * dtau + last_li * last_dtau);

	  scat = -tau * scatter;
	  dO =
	    1 - color(exp(comp(scat, 0)), exp(comp(scat, 1)),
		      exp(comp(scat, 2)));
	  dC = lighttau * dO;

	  /* Now we adjust Cv/Ov to account for dC and dO */
	  Cv += (1 - Ov) * dC;
	  Ov += (1 - Ov) * dO;

	  ss = max(min(ss, end - d), 0.005);
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
  Ci = lightscale * Cv + (1 - Ov) * Ci;
  Oi = Ov + (1 - Ov) * Oi;

  if(debug > 0)
    {
      printf("nsteps = %f, t1 = %f, end = %f\n", nsteps, integstart, end);
      printf("   Cv = %c, Ov = %c\n", Cv, Ov);
    }
}
