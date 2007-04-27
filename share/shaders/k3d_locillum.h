/************************************************************************
 * locillum.h - Functions that compute the light response of locillums.
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


#ifndef LOCILLUM_H
#define LOCILLUM_H




/*
 * Oren and Nayar's generalization of Lambert's reflection model.
 * The roughness parameter gives the standard deviation of angle
 * orientations of the presumed surface grooves.  When roughness=0,
 * the model is identical to Lambertian reflection.
 */
color
LocIllumOrenNayar (normal N;  vector V;  float roughness;)
{
    /* Surface roughness coefficients for Oren/Nayar's formula */
    float sigma2 = roughness * roughness;
    float A = 1 - 0.5 * sigma2 / (sigma2 + 0.33);
    float B = 0.45 * sigma2 / (sigma2 + 0.09);
    /* Useful precomputed quantities */
    float  theta_r = acos (V . N);        /* Angle between V and N */
    vector V_perp_N = normalize(V-N*(V.N)); /* Part of V perpendicular to N */

    /* Accumulate incoming radiance from lights in C */
    color  C = 0;
    extern point P;
    illuminance (P, N, PI/2) {
	/* Must declare extern L & Cl because we're in a function */
	extern vector L;  extern color Cl;
	float nondiff = 0;
	lightsource ("__nondiffuse", nondiff);
	if (nondiff < 1) {
	    vector LN = normalize(L);
	    float cos_theta_i = LN . N;
	    float cos_phi_diff = V_perp_N . normalize(LN - N*cos_theta_i);
	    float theta_i = acos (cos_theta_i);
	    float alpha = max (theta_i, theta_r);
	    float beta = min (theta_i, theta_r);
	    C += (1-nondiff) * Cl * cos_theta_i * 
		(A + B * max(0,cos_phi_diff) * sin(alpha) * tan(beta));
	}
    }
    return C;

}



/*
 * Greg Ward Larson's anisotropic specular local illumination model.
 * The derivation and formulae can be found in:  Ward, Gregory J.
 * "Measuring and Modeling Anisotropic Reflection," ACM Computer 
 * Graphics 26(2) (Proceedings of Siggraph '92), pp. 265-272, July, 1992.
 * Notice that compared to the paper, the implementation below appears
 * to be missing a factor of 1/pi, and to have an extra L.N term.
 * This is not an error!  It is because the paper's formula is for the
 * BRDF, which is only part of the kernel of the light integral, whereas
 * shaders must compute the result of the integral.
 *
 * Inputs:
 *   N - unit surface normal
 *   V - unit viewing direction (from P toward the camera)
 *   xdir - a unit tangent of the surface defining the reference
 *          direction for the anisotropy.
 *   xroughness - the apparent roughness of the surface in xdir.
 *   yroughness - the roughness for the direction of the surface 
 *          tangent perpendicular to xdir.
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
}



/*
 * LocIllumGlossy - a possible replacement for specular(), with a
 * more uniformly bright core and a sharper falloff.  It's a nice
 * specular function to use for something made of glass or liquid.
 * Inputs:
 *  roughness - related to the size of the highlight, larger is bigger
 *  sharpness - 1 is infinitely sharp, 0 is very dull
 */
color LocIllumGlossy ( normal N;  vector V;
                       float roughness, sharpness; )
{
    color C = 0;
    float w = .18 * (1-sharpness);
    extern point P;
    illuminance (P, N, PI/2) {
	/* Must declare extern L & Cl because we're in a function */
	extern vector L;  extern color Cl; 
	float nonspec = 0;
	lightsource ("__nonspecular", nonspec);
	if (nonspec < 1) {
	    vector H = normalize(normalize(L)+V);
	    C += Cl * ((1-nonspec) * 
		       smoothstep (.72-w, .72+w,
				   pow(max(0,N.H), 1/roughness)));
	}
    }
    return C;
}

#endif /* defined(LOCILLUM_H) */

