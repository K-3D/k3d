/*******************************************************************************
 * JBUMcraters.sl -- Lunar-like surface - Jim Bumgardner jbum@jbum.com
 *
 * Voronoi/Worley noise is combined using 1/f scaling to put craters on a surface.
 * Similar tricks can be used to make cork, swiss-cheese, bread, etc.
 ******************************************************************************/


#include "k3d_material.h"
#include "k3d_noises.h"
#include "k3d_patterns.h"


surface
k3d_craters ( float Ka = 1, Kd = 0.7, roughness = 0.1;
   float Km = 0.03;	/* Depth of crater displacement */
   float swidth=.6;      /* Size of largest crater relative to s/t mapping */
   float pitfactor = .7; /* chance of crater occuring in a cell */
   float octaves=9;      /* number of scales used - use lower numbers for cartoony effects */
   float lac=1.91341;    /* diameter spacing between successive octaves */
   float amp=1.91341;    /* amplitude reduction between successive octaves */
   float jitter=1.0;     /* irregularity of crater spacing */
  
   /* you'll get artifacts if you go too much higher than 1 */
   /* but this can be avoided by looping i,j from -2 to 2 instead of -1 to 1 */
   float distortamp = 0.0005; /* Crater distortion.  You'll get artifacts if it goes too high. */
)
{
    normal Nf;
    color		Ct;

    float ss, tt, angle, r, rim, uu, a, i, j, sc, asc, lev;
    float sctr, tctr, scell, tcell,scellctr,tcellctr;
    float pert=0,rad,pert2;
    float rmax = swidth/2; /* was /2 */
    float	rmax1=1000000,rmax2=1000000,rrad1=0,rrad2=0,cellsizescale,ilac;

    ilac = 1/lac;

    /* Add craters */

    for (lev = 0; lev < octaves; lev += 1)
      {
	sc = pow(lac, lev);
	asc = 1 / pow(amp, lev);
	scellctr = floor(s*sc/swidth);
	tcellctr = floor(t*sc/swidth);
	cellsizescale = swidth/sc;
	rrad1 = 0;
	rrad2 = 0;
	for (i = -1; i <= 1; i += 1) 
	  {
	    scell = scellctr + i;
	    for (j = -1; j <= 1; j += 1) 
	      {
		tcell = tcellctr + j;
		if ( cellnoise(7*scell-9.5, 7*tcell+7.5) < pitfactor)
		  /* xp = (sc*7*scell-9.5,sc*7*tcell+7.5,.5); */
		  /* if ( fBm_default(xp) < pitfactor)  *//* I like this distribution better - pitf = -1 -> 1 */
		  {
		    sctr = cellsizescale * (scell + 0.5 + jitter * cellnoise(scell+0.5, tcell+0.5));
		    tctr = cellsizescale * (tcell + 0.5 + jitter * cellnoise(scell+3.5, tcell+8.5));
		    ss = s - sctr;
		    tt = t - tctr;
		    r = ss*ss + tt*tt; /* r is distance from center squared */
		    if (r < rmax1) {
		      /* rad is size of crater squared */
		      rad = rmax*ilac;
		      rad /= sc;
		      rad *= rad;
		      rmax2 = rmax1;
		      rrad2 = rrad1;
		      rmax1 = r;
		      rrad1 = rad;
		    }
		    else if (r < rmax2) {
		      /* rad is size of crater squared */
		      rad = rmax*ilac;
		      rad /= sc;
		      rad *= rad;
		      rmax2 = r;
		      rrad2 = rad;
		    }
		  }
	      }
	  }
	/* Optional Crater Distortion... */
	if (distortamp != 0)
	  {
	    rrad1 += fBm_default(P*sc)*asc*distortamp; /* add crater roughness */
	    rrad2 += fBm_default(P*sc)*asc*distortamp; 
	  }
	
	/* Seem crater perturbations for each scale */
	if (rrad1 > 0 && rmax1 > 0) 
	  {
	    r = smoothstep(0,rrad1,rmax1);
	    r = r*(2*r-pow(r,3))+(1-r)*pow(r,3);
	    pert += sin(r*PI/2)*asc-asc;
	  }
	if (rrad2 > 0 && rmax2 > 0) 
	  {
	    r = smoothstep(0,rrad2,rmax2);
	    r = r*(2*r-pow(r,3))+(1-r)*pow(r,3);			
	    pert += sin(r*PI/2)*asc-asc;
	  }
      }
    
    P += Km*pert*normalize(N);
    N = calculatenormal(P);	
    Nf = faceforward (normalize(N),I);
    
    Ct = Cs;
    Ci = MaterialClay (Nf, Ct, Ka, Kd, roughness);
    Oi = Os;  Ci *= Oi;
}
