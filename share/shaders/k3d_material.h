/************************************************************************
 * material.h - Functions which compute the light response of materials.
 *
 * Author: Larry Gritz (gritzl@acm.org)
 *
 * Reference:
 *   _Advanced RenderMan: Creating CGI for Motion Picture_, 
 *   by Anthony A. Apodaca and Larry Gritz, Morgan Kaufmann, 1999.
 *
 * $Revision: 1.1 $    $Date: 2004/05/19 18:15:19 $
 *
 ************************************************************************/

#ifndef MATERIAL_H
#define MATERIAL_H


#include "k3d_locillum.h"
#include "k3d_reflections.h"



/* Compute the color of the surface using a simple plastic-like BRDF.
 * Typical values are Ka=1, Kd=0.8, Ks=0.5, roughness=0.1.
 */
color MaterialPlastic (normal Nf;  color basecolor;
                       float Ka, Kd, Ks, roughness;)
{
    extern vector I;
    return basecolor * (Ka*ambient() + Kd*diffuse(Nf))
           + Ks*specular(Nf,-normalize(I),roughness);
}


/* Compute the color of the surface using a simple Lambertian BRDF. */
color MaterialMatte (normal Nf;  color basecolor;  float Ka, Kd;)
{
    return basecolor * (Ka*ambient() + Kd*diffuse(Nf));
}


/* Compute the color of the surface using a simple metal-like BRDF.  To
 * give a metallic appearance, both diffuse and specular components are 
 * scaled by the color of the metal.  It is recommended that Kd < 0.1,
 * Ks > 0.5, and roughness > 0.15 to give a believable metallic appearance.
 */
color MaterialRoughMetal (normal Nf;  color basecolor;
                          float Ka, Kd, Ks, roughness;)
{
    extern vector I;
    return basecolor * (Ka*ambient() + Kd*diffuse(Nf) +
                        Ks*specular(Nf,-normalize(I),roughness));
}


/* Compute the color of the surface using a simple thin plastic-like BRDF.
 * We call it _thin_ because it includes a transmisison component to allow 
 * light from the _back_ of the surface to affect the appearance.  Typical
 * values are Ka=1, Kd=0.8, Kt = 0.2, Ks=0.5, roughness=0.1.
 */
color MaterialThinPlastic (normal Nf;  vector V;  color basecolor; 
                           float Ka, Kd, Kt, Ks, roughness;)
{
    return basecolor * (Ka*ambient() + Kd*diffuse(Nf) + Kt*diffuse(-Nf))
           + Ks*specular(Nf,V,roughness);
}



/* Compute the color of the surface using a simple plastic-like BRDF, with
 * fresnel-attenuated coherent reflections.
 * If twosided is nonzero, both sides are shiny, but if you know that
 * the object has N facing outward, twosided==0 is a good optimization
 * (especially when using the ray server).
 */
color
MaterialShinyPlastic (normal Nf; color basecolor;
                      float Ka, Kd, Ks, roughness, Kr, blur, ior;
		      uniform float twosided;
                      DECLARE_ENVPARAMS; )
{
    extern vector I;
    extern point P;
    extern normal N;
    vector IN = normalize(I), V = -IN;
    float fkr, fkt;  vector R, T;
    fresnel (IN, Nf, 1/ior, fkr, fkt, R, T);
    fkt = 1-fkr;
    if (twosided == 0 && N.I > 0)
	fkr = 0;
    return  fkt * basecolor * (Ka*ambient() + Kd*diffuse(Nf))
          + (Ks) * specular(Nf,V,roughness)
	  + SampleEnvironment (P, R, fkr*Kr, blur, ENVPARAMS);
}



/* Compute the color of the surface using a simple metal-like BRDF.  To
 * give a metallic appearance, both diffuse and specular components are 
 * scaled by the color of the metal.  It is recommended that Kd < 0.1,
 * Ks > 0.5, and roughness > 0.15 to give a believable metallic appearance.
 * If twosided is nonzero, both sides are shiny, but if you know that
 * the object has N facing outward, twosided==0 is a good optimization
 * (especially when using the ray server).
 */
color MaterialShinyMetal (normal Nf;  color basecolor;
                          float Ka, Kd, Ks, roughness, Kr, blur;
			  uniform float twosided;
                          DECLARE_ENVPARAMS;)
{
    extern point P;
    extern vector I;
    extern normal N;
    float kr = Kr;
    if (twosided == 0 && N.I > 0)
	kr = 0;
    vector IN = normalize(I), V = -IN;
    vector R = reflect (IN, Nf);
    return basecolor * (Ka*ambient() + Kd*diffuse(Nf) +
                        Ks*specular(Nf,V,roughness) + 
                        SampleEnvironment (P, R, kr, blur, ENVPARAMS));
}



/* Compute the color of the surface of a very rough, totally nonspecular
 * material like clay.  Use an Oren/Nayar BRDF for the diffuse term.
 */
color MaterialClay (normal Nf;  color basecolor;
                    float Ka, Kd, roughness;)
{
    extern vector I;
    return basecolor * (Ka*ambient() + 
			Kd*LocIllumOrenNayar(Nf,-normalize(I),roughness));
}


/* Compute the color of the surface using an anisotropic BRDF.  To
 * give a metallic appearance, both diffuse and specular components are 
 * scaled by the color of the metal.  It is recommended that Kd < 0.1,
 * Ks > 0.5, and roughness > 0.15 to give a believable metallic appearance.
 */
color MaterialBrushedMetal (normal Nf;  color basecolor;
                            float Ka, Kd, Ks;
                            vector xdir;  float uroughness, vroughness;)
{
    extern vector I;
    color spec = LocIllumWardAnisotropic (Nf, -normalize(I), 
					  xdir, uroughness, vroughness);
    return basecolor * (Ka*ambient() + Kd*diffuse(Nf) + Ks*spec);
}


/* Compute the color of a ceramic object.  Like plastic, but use a
 * "glossy" specular term.
 */
color MaterialCeramic (normal Nf;  color basecolor;
                       float Ka, Kd, Ks, roughness, specsharpness;)
{
    extern vector I;
    vector V = -normalize(I);
    return basecolor * (Ka*ambient() + Kd*diffuse(Nf))
              + Ks * LocIllumGlossy (Nf, V, roughness/10, specsharpness);
}



/* Compute the color of a glass-like surface with coherent reflections
 * and refractions.
 */
color MaterialGlass (normal Nf;  color basecolor;
                     float Ka, Kd, Ks, roughness, Kr, reflblur;
		     float Kt, refrblur, eta;
		     color transmitcolor;
		     uniform float refrrayjitter, refrraysamples;
		     DECLARE_ENVPARAMS;)
{
    extern point P;
    extern vector I;
    extern normal N;

    vector IN = normalize (I);

    /* Compute the reflection & refraction directions and amounts */
    vector Rfldir, Rfrdir;   /* Smooth reflection/refraction directions */
    float kr, kt;
    fresnel (IN, Nf, (I.N < 0) ? 1.0/eta : eta, kr, kt, Rfldir, Rfrdir);
    kt = 1-kr;   /* Physically incorrect, but portable */
    kr *= Kr;
    kt *= Kt;
#if (defined(BMRT) || defined(RAYSERVER_H))
    /* Speedup -- at deep ray levels, reflection is unimportant */
    if (raylevel() > 0)
	kr = 0;
#endif
#ifndef BMRT
    /* Speedup for PRMan -- don't shade back sides, but you HAVE to be sure
     * that normals correctly face outward.
     */
    if (N.I > 0)
	kr = kt = 0;
#endif

    /* Calculate the reflection & refraction color */
    color Crefl = SampleEnvironment (P, normalize(Rfldir), kr, reflblur,
				     ENVPARAMS);
    color Crefr = SampleEnvironment (P, normalize(Rfrdir), kt, refrblur,
				     envname, envspace, envrad, 
				     refrrayjitter, refrraysamples);
    return (basecolor * (Ka*ambient() + Kd*diffuse(Nf)) +
	    (Crefl + Ks*LocIllumGlossy(Nf,-IN,roughness,0.5)) +
	    transmitcolor * Crefr);
}



#endif /* defined(MATERIAL_H) */

