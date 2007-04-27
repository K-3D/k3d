/* renamed LG_orennayar.sl -- tal@SpamSucks_cs.caltech.edu */

/*
 * orennayar.sl - rough diffuse surface
 *
 * 
 * DESCRIPTION:
 *   Makes a rough surface using a BRDF which is more accurate than
 *   Lambert.  Based on Oren & Nayar's model (see Proc. SIGGRAPH 94).
 *
 *   Lambertian (isotropic) BRDF is a simple approximation, but not
 *   an especially accurate one for rough surfaces.  Truly rough surfacs
 *   tend to act more like retroreflectors than like isotropic scatterers.
 * 
 * PARAMETERS:
 *   Ka, Kd - just like matte.sl
 *   sigma - roughness (0 is lambertian, larger values are rougher)
 *
 * AUTHOR:  Larry Gritz
 *
 * REFERENCES:
 *   Oren, Michael and Shree K. Nayar.  "Generalization of Lambert's
 *         Reflectance Model," Computer Graphics Annual Conference
 *         Series 1994 (Proceedings of SIGGRAPH '94), pp. 239-246.
 *
 * NOTES:
 *   1. Note that this is really just an illuminance loop that gathers
 *      light from the sources and applies Oren & Nayar's local reflectance
 *      model to the result.  It could easily be packaged up as a macro
 *      or a function and used in any other shader, in place of diffuse().
 *   2. Examination of why rough surfaces are not Lambertian will lead
 *      you to the solution to the famous "flat full moon" problem.
 *
 * HISTORY:
 *   14 June 1994 -- written by Larry Gritz
 *
 */


surface
k3d_orennayar (float Ka = 1;
	   float Kd = .5;
	   float sigma = 0.0; )
{
    point Nf, IN, Eye, LN;
    color lightC = 0;
    color L1, L2;
    float C1, C2, C3;
    float theta_r, theta_i, cos_theta_i;
    float alpha, beta, sigma2, cos_phi_diff;

    Nf = faceforward (normalize(N),I);
    IN = normalize (I);
    Eye = -IN;
    theta_r = acos (Eye . Nf);
    sigma2 = sigma*sigma;

    illuminance (P, Nf, PI/2) {
	LN = normalize(L);
	cos_theta_i = LN . Nf;
	cos_phi_diff = normalize(Eye-Nf*(Eye.Nf)) . normalize(LN - Nf*(LN.Nf));
	theta_i = acos (cos_theta_i);
	alpha = max (theta_i, theta_r);
	beta = min (theta_i, theta_r);
	C1 = 1 - 0.5 * sigma2/(sigma2+0.33);
	C2 = 0.45 * sigma2 / (sigma2 + 0.09);
	if (cos_phi_diff >= 0)
	    C2 *= sin(alpha);
	else C2 *= (sin(alpha) - pow(2*beta/PI,3));
	C3 = 0.125 * sigma2 / (sigma2+0.09) * pow ((4*alpha*beta)/(PI*PI),2);
	L1 = Cs * (cos_theta_i * (C1 + cos_phi_diff * C2 * tan(beta) +
				  (1 - abs(cos_phi_diff)) * C3 * tan((alpha+beta)/2)));
	L2 = (Cs * Cs) * (0.17 * cos_theta_i * sigma2/(sigma2+0.13) *
			  (1 - cos_phi_diff*(4*beta*beta)/(PI*PI)));
	lightC += (L1 + L2) * Cl;
    }

    Oi = Os;
    Ci = Os * (Cs * (Ka*ambient()) + Kd*lightC);
}
