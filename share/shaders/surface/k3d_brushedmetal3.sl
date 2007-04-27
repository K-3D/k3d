/* Renamed to LGbrushedmetal for RMR -- tal@SpamSucks_cs.caltech.edu */

/*
 * Greg Ward Larson's anisotropic specular local illumination model.
 * The derivation and formulae can be found in:  Ward, Gregory J.
 * "Measuring and Modeling Anisotropic Reflection," ACM Computer
 * Graphics 26(2) (Proceedings of Siggraph '92), pp. 265-272, July, 1992.
 * Inputs:
 *   N - unit surface normal
 *   V - unit viewing direction (from P toward the camera)
 *   xdir - a unit tangent of the surface which defines the reference
 *          direction for the anisotropy.
 *   xroughness - the apparent roughness of the surface in xdir.
 *   yroughness - the roughness for the direction of the surface
 *          tangent which is perpendicular to xdir.
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
        /* Must declare because extern L & Cl because we're in a function */
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



surface
k3d_brushedmetal3 ( float Ka = 1, Kd = 0.1, Ks = .9;
		float uroughness = 0.35, vroughness = 0.2; 
		color basecolor = color (.5, .5, .5);
)
{
    normal Nf = faceforward (normalize(N), I);
    vector xdir = normalize (dPdu);

    color spec = LocIllumWardAnisotropic (Nf, -normalize(I),
                                          xdir, uroughness, vroughness);
    Ci = basecolor * (Ka*ambient() + Kd*diffuse(Nf) + Ks*spec);
    Oi = Os;  Ci *= Oi;
}

