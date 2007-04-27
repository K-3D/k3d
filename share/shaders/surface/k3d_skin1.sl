/**
 * RCskin.sl
 * 
 * Copyright (C) 2002, Rudy Cortes   rcortes@hntb.com
 * created  09/16/2002
 * 
 * This software is placed in the public domain and is provided as is 
 * without express or implied warranty. 
 * 
 * Surface shader that implements a shading model that should have a visual 
 * appearence generall similar to that of skin. 
 *
 * Feel free to use this shader to create skin for any character, anywhere and
 * everywhere, Just list me on the credits under "Shading Team"
 *******************************
 * PARAMETERS
 * Kd = the amount of uniform diffusion applyied to the skin
 * skincolor, skinmap = the color of the skin. Using a map overrides original skincolor
 * color value.
 * sheencolor, shinmap = the color of the skin at grazing angles. Using a map overrides
 * original sheencolor color value.
 * blemishfreq, blemishthresh,blemhishopac = control the freqency, threshold and opacity
 * of blemishes on the skin. Use to give skin a little variation.
 * blemishmap = Use a grayscale map to control where the belmishes will be more visible
 * xdir, angle = control the direction of the specular highlights.
 * Oily,oilmap = controls how oily the skin looks. Use a grayscale map to contol
 * oiliness.---Note--- teh oiliy parameter is multiplied with the map.
 * xroughness,yroughness = how rough is the specular highlight on x and Y?
 * poresfreq, poresthresh poresdepth = control the frequency, threshold and depth of the
 * pores.
 **************************************
 *
 * You can replace the header functions by using
 * #include "locilum.h" and #include "noises.h".
 * NOTE- you must copy the entire subsurface skin function to your
 * "locIllum.h" file
 *
 * Enjoy!!
 *
 * Modification history:
 *   9/22/02 Tal Lancaster
 *     Renamed RudyCSkin for RMR.
 *     Made changes so would compile.
 *
 **/

#include "k3d_noises.h"


 /* NECESARY FUNCTIONS */


 /*---------FROM BMRT's locillum.h-----
 * Greg Ward Larson's anisotropic specular local illumination model.
 * The derivation and formulae can be found in:  Ward, Gregory J.
 * "Measuring and Modeling Anisotropic Reflection," ACM Computer 
 * Graphics 26(2) (Proceedings of Siggraph '92), pp. 265-272, July, 1992.
 */

color
LocIllumWardAnisotropic (normal N;  vector V;
                         vector xdir;  float xroughness, yroughness;)
{
    float sqr (float x) { return x*x; }

    float cos_theta_r = clamp (N.V, 0.0001, 1);
    vector X = xdir / xroughness;
    vector Y = (N ^ xdir) / yroughness;

    color C = 0;
    extern point P;
    illuminance (P, N, PI/2) {
  /* Must declare extern L & Cl because we're in a function */
  extern vector L;  extern color Cl; 
  float nonspec = 0;
  lightsource ("__nonspecular", nonspec);
  if (nonspec < 1) {
      vector LN = normalize (L);
      float cos_theta_i = LN . N;
      if (cos_theta_i > 0.0) {
    vector H = normalize (V + LN);
    float rho = exp (-2 * (sqr(X.H) + sqr(Y.H)) / (1 + H.N))
        / sqrt (cos_theta_i * cos_theta_r);
    C += Cl * ((1-nonspec) * cos_theta_i * rho);
      }
  }
    }
    return C / (4 * xroughness * yroughness);
}    /*---locillum.h ends---*/


/* ------ ADAPTED FROM Mat Pahr's Skin.sl------
 * --- subsurfaceSkin------
 * Surface shader that implements a shading model that should have a visual 
 * appearence generall similar to that of skin.  Based on phenomenological 
 * information about skin reflectance from Hanrahan and Krueger, 
 * "Reflection from layered surfaces due to subsurface scattering", 
 * proceedings of Siggraph 1993.
 */
/* Evaluate the Henyey-Greenstein phase function for two vectors with
   an asymmetry value g.  v1 and v2 should be normalized and g should 
   be in the range (-1, 1).  Negative values of g correspond to more
   back-scattering and positive values correspond to more forward scattering.
*/
float phase(vector v1, v2; float g) {
  float costheta = -v1 . v2;
  return (1. - g*g) / pow(1. + g*g - 2.*g*costheta, 1.5);
}

/* Compute a the single-scattering approximation to scattering from
   a one-dimensional volumetric surface.  Given incident and outgoing
   directions wi and wo, surface normal n, asymmetry value g (see above),
   scattering albedo (between 0 and 1 for physically-valid volumes),
   and the thickness of the volume, use the closed-form single-scattering
   equation to approximate overall scattering.
*/
float singleScatter(vector wi, wo; normal n; float g, albedo, thickness) {
    float win = abs(wi . n);
    float won = abs(wo . n);

    return albedo * phase(wo, wi, g) / (win + won) *
    (1. - exp(-(1/win + 1/won) * thickness));
}

vector efresnel(vector II; normal NN; float eta; output float Kr, Kt;) {
    vector R, T;
    fresnel(II, NN, eta, Kr, Kt, R, T);
    Kr = smoothstep(0., .5, Kr);
    Kt = 1. - Kr;
    return normalize(T);
}

/* Implements overall skin subsurface shading model.  Takes viewing and
   surface normal information, the base color of the skin, a
   color for an oily surface sheen.
*/
color subsurfaceSkin(vector Vf; normal Nn; color skinColor, sheenColor;
                     float eta, thickness) {
  extern point P;
  float Kr, Kt, Kr2, Kt2;
  color C = 0;

  vector T = efresnel(-Vf, Nn, eta, Kr, Kt);
      
  illuminance(P, Nn, PI/2) {
      vector Ln = normalize(L);

      vector H = normalize(Ln + Vf);
      if (H . Nn > 0)
    C += Kr * sheenColor * Cl * (Ln . Nn) * pow(H . Nn, 4.);
      C += Kr * sheenColor * Cl * (Ln . Nn) * .2;

      vector T2 = efresnel(-Ln, Nn, eta, Kr2, Kt2);
      C += skinColor * Cl * (Ln . Nn) * Kt * Kt2 * 
    (singleScatter(T, T2, Nn, .8, .8, thickness) +
     singleScatter(T, T2, Nn, .3, .5, thickness) +
     singleScatter(T, T2, Nn, 0., .4, thickness));
  }
  return C;
}    /* subsurfaceSkin --ends */


/*
 *shader beggins here.
 */

surface k3d_skin1(
  float Kd = .1;
  color skincolor = color (0.7, 0.56, 0.52);
  string skinmap = "";
  color sheenColor = 1.;
  string sheenmap = "";
  float blemishfreq  = 12,
        belmishthresh = 4,
        blemishopac = 1;
  string blemishmap = "";
  float eta = 1.01,
        thickness = 1;
  varying vector xdir = dPdu;
  float angle = 180;
  float oily = 1.5;
  string oilmap = "";
  float xroughness = .3,
        yroughness = .5;
  float poresfreq = 60,
        poresthresh = 3,
        poresdepth = -0.001;)


{
  /* initialize local variables*/

  normal Nf, NN;
  vector Vf = -normalize(I);
  color sc , lc;
  float lo;
  point PP;
  float turb, f;
  float maxfreq = 8;

  color Cskin = skincolor;
  color Csheen = sheenColor;
  float oilVal = oily;


  /*--- layer 1 - add pores to the skin*/

  PP = transform ("shader",P) * poresfreq;
  turb = 0;
  for (f = 1; f< maxfreq; f *= 2)
  turb += abs(snoise(PP * f)) / f;

  turb = pow(turb,poresthresh) * poresdepth;


  /* displace normals*/
  NN = calculatenormal(P + turb * normalize(N));
  Nf = faceforward(normalize(NN),I);

  /* layer 1 .- apply a subsurface scattered BRDF to the surface.
   * you can use an image map for the color of the skin and the sheen.
   * If the map is not provided use defined colors*/

  if (skinmap != "")
      Cskin = color texture(skinmap);

  if ( sheenmap != "")
      Csheen = color texture(sheenmap);

  sc = Cskin;

  /* layer 2 - create small skin blemishes over the skin. Use a map to control
   * where the blemishes are visible */

  PP = transform ("shader",P) * blemishfreq;
  turb = 0;
  for (f = 1; f< maxfreq; f *= 2)
  turb += abs(snoise(PP * f)) / f;

  turb = pow(turb, belmishthresh );

  color red01 = Cskin + 0.05,
        red02 = red01 - 0.05,
        darkred01 = Cskin - 0.05,
        darkred02 = darkred01 - 0.05;


  color blemishcol = color spline ( turb,
                                    red01,
                                    red02,
                                    darkred01,
                                    red01,
                                    darkred01,
                                    red02,
                                    darkred01,
                                    darkred02,
                                    darkred01,
                                    darkred02,
                                    red01);

  lc =  subsurfaceSkin(Vf, Nf, blemishcol, Csheen, 1/eta, thickness);
  lo = 1 * (blemishopac) ;

  if (blemishmap != "")
      lo *= float texture (blemishmap);

  sc = mix (sc,lc,lo);

  /* --Layer 3-- apply an Anisotropic BRDF to simulate the oil layer that lies
  * outside the skin. Oilyness is controlled by the oily, xrougness and yroughness.
  * */

  if (oily > 0.01){
  vector anisoDir = xdir;
  if (angle !=0)
  {
    matrix rot = rotate(matrix 1, radians(angle),Nf);
    anisoDir = vtransform(rot,anisoDir);
    }
  lc = LocIllumWardAnisotropic(Nf,Vf,anisoDir,xroughness,yroughness);
 
  if (oilmap != "")
      oilVal *= float texture(oilmap);

  sc += (lc * (oilVal * 0.1) );
  }



  /* output*/

  Oi = Os;
  Ci = sc + Kd * diffuse(Nf) ;
}



