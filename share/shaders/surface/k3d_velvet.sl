/* Renamed to SHW_velvet.sl -- tal@SpamSucks_cs.caltech.edu */

/*
 * velvet.sl -- velvet
 *
 * DESCRIPTION:
 *   An attempt at a velvet surface.
 *   This phenomenological model contains three compnents:
 *   - A retroreflective lobe (back toward the light source)
 *   - Scattering near the horizon, regardless of incident direction
 *   - A diffuse color
 * 
 * PARAMETERS:
 *   Ks:	controls retroreflective lobe
 *   Kd:	scales diffuse color
 *   Ka:	ambient component (affects diffuse color only)
 *   sheen:	color of retroreflective lobe and horizon scattering
 *   roughness: shininess of fabric (controls retroreflection only)
 *
 * ANTIALIASING: should antialias itself fairly well
 *
 * AUTHOR: written by Stephen H. Westin, Ford Motor Company
 *
 * HISTORY:
 * 	2001.02.01	westin@SpamSucks_graphics.cornell.edu
 *			Fixed retroreflection lobe (sign error); added
 *			"backscatter" parameter to control it; added
 *			"edginess" parameter to control horizon scatter;
 *			defined sqr()
 *
 * prev modified  28 January 1997 S. H. Westin
 */

#include "k3d_functions.h"

surface
k3d_velvet (float Ka = 0.05,
              Kd = 0.1,
              Ks = 0.1;
	    float backscatter = 0.1,
	    edginess = 10;
        color sheen = .25;
        float roughness = .1;
  )
{
  normal Nf;                     /* Normalized normal vector */
  vector V;                      /* Normalized eye vector */
  vector H;                      /* Bisector vector for Phong/Blinn */
  vector Ln;                     /* Normalized vector to light */
  color shiny;                   /* Non-diffuse components */
  float cosine, sine;            /* Components for horizon scatter */

  Nf = faceforward (normalize(N), I);
  V = -normalize (I);

  shiny = 0;
  illuminance ( P, Nf, 1.57079632679489661923 /* Hemisphere */ ) {
    Ln = normalize ( L );
    /* Retroreflective lobe */
    cosine = max ( Ln.V, 0 );
    shiny += pow ( cosine, 1.0/roughness ) * backscatter
      * Cl * sheen;
    /* Horizon scattering */
    cosine = max ( Nf.V, 0 );
    sine = sqrt (1.0-sqr(cosine));
    shiny += pow ( sine, edginess ) * Ln.Nf * Cl * sheen;
  }

  Oi = Os;
  /* Add in diffuse color */
  Ci = Os * (Ka*ambient() + Kd*diffuse(Nf)) * Cs + shiny;

}

