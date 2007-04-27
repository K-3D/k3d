/* 
 * skin.sl 
 * 
 * Copyright (C) 2000-2001, Matt Pharr <mmp@SpamSucks_Exluna.com> 
 * 
 * This software is placed in the public domain and is provided as is 
 * without express or implied warranty. 
 * 
 * Surface shader that implements a shading model that should have a visual 
 * appearence generall similar to that of skin.  Based on phenomenological 
 * information about skin reflectance from Hanrahan and Krueger, 
 * "Reflection from layered surfaces due to subsurface scattering", 
 * proceedings of Siggraph 1993. 
 *
 * See SIGGRAPH 2001 course notes, "Advanced RenderMan 3: Render Harder,"
 * for notes and background information.
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
   color for an oily surface sheen, the ratio of the indices of 
   refraction of the incoming ray (typically ~1 for air) to the index
   of refraction for the transmitted ray (say something like 1.4 for
   skin), and the overall thickness of the skin layer.  Then loops
   over light sources with illuminance() and computes the reflected
   skin color.
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
}

/* Basic surface shader that uses the skin reflection model implemented
   above.  Uses Cs for the basic color of the skin (.8, .5, .5) works
   reasonably well for Caucasian skin.
*/
surface k3d_skin2(color Ka = .5; color sheenColor = 1.;
             float eta = 1./1.4, thickness = .5) {
	normal Nn = faceforward(normalize(N), I);
	vector Vf = -normalize(I);

	Oi = Os;
	Ci = Os * subsurfaceSkin(Vf, Nn, Cs, sheenColor, eta, thickness);
}
