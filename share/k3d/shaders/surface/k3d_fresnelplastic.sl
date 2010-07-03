/* 
 * TLFresnelPlastic.sl -- simple shader illustrating fresnel().
 *
 * DESCRIPTION:
 *    Simple Fresnel shader using plastic illuminance.
 *
 * PARAMETERS
 *
 * AUTHOR: Tal Lancaster  tal@SpamSucks_renderman.org
 *
 * HISTORY:
 *    Jan 26 2002  Created
 */

/* From Advanced RenderMan */
#include "k3d_reflections.h"

#ifdef PRMAN  /* as of prman10 */
#define CHKTX(tx) ((tx != "" && textureinfo (tx, "exists", 0) != 0)? 1: 0)
#else
#define CHKTX(tx) \
    ((tx != "")? 1: 0)
#endif

surface k3d_fresnelplastic (
   color Csurf = 0.5; /* cat Color 
			 desc {Base color for surface if 
			 not over-written by "colorMap } */
   string colorMap = ""; /* cat Color
			    desc {Optional color map.  Replaces 'Csurf'.} */
   
   float ior = 0;
   /* desc { Index of Refraction.  Used to get Fresnel falloff.
      0: turns off fresnel and gives plastic look. Typical values
      gt 1.0 and lt 2.2}  range {1.01 2.2 .01} */
   float Ka= 1;       /* desc { Usual } */
   float Kd= 0.5;     /* desc { Usual } */
   float Ks= 0.5;     /* desc { Usual } */
   float roughness= 0.1;  /* desc {Specular roughness} */
   color Cspec = 1;       /* desc {Specular color} */
   float Kr = 0;  /* cat Reflection desc {Strength of reflection } */
   string reflectMap = ""; 
   /* cat Reflection desc {Name of reflection map. } type texture */
   float reflectBlur = 0; 
   /* cat Reflection desc {Percentage amount to blur map} */
   string reflectSpace = "world";  /* cat Reflection 
   desc {Space the reflection calculations are performed in. } */
   float reflectDist = 1e10;
   /* cat Reflection desc {The size of the room to base the reflection
      lookup off of.} */

   float flipS = 0; /* cat ST type switch desc {Flip S on texture lookups.} */
   float flipT = 0; /* cat ST type switch desc {Flip S on texture lookups.} */
   float MtorFlip = 0; /* cat ST type switch def 1
			  desc {Swap S/T access to overcome Maya/MtoR
			  NURBs differences.} */
)
{
    float ss, tt;
    float fKt, fKr;
    color Ct;

    normal Nf = normalize (faceforward( normalize(N), I ));
    vector V = -normalize(I);
    vector R = normalize (reflect (I, Nf));

    /* Parametric space adjustments */
    if (MtorFlip == 1) {
	ss = t;
	tt = s;
    }
    else {
	ss = s;
	tt = t;
    }
    if (flipS == 1)
	ss = 1 - ss;
    if (flipT == 1)
	tt = 1 - tt;

    /* Calculate fresnel index of refraction */
    if (ior != 0) {
      fresnel (normalize (I), Nf, (I.Nf > 0)? ior: 1/ior,
	       fKr, fKt);
      /* hack */
      fKt = 1 - fKr;
    }
    else {
      /* fresnel turned off */
      fKr = fKt = 1;
    }

    /* base color */
    /*Ct = (CHKTX (colorMap))? texture (colorMap, ss, tt): Csurf; */
    if (CHKTX (colorMap) == 1)
      Ct = texture (colorMap, ss, tt);
    else
      Ct = Csurf;

    /* Environment */
    color Cr = 0;
    float reflOpacity = 1;
    if (reflectMap != "")
      Cr = Environment (reflectMap, reflectSpace, reflectDist, 
			P, R, reflectBlur, reflOpacity);

    /* lighting */
    Oi = Os;
    Ci = Os * ( fKt * Ct * (Ka*ambient() + Kd*diffuse(Nf)) +
                fKr * Cspec * Ks * specular(Nf,V,roughness) +
		Kr * Cr * fKr);
}
