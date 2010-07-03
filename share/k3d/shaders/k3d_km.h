/* 
 * km.h
 * 
 * Copyright (C) 2001, Matt Pharr <mmp@Exluna.com> 
 * 
 * This software is placed in the public domain and is provided as is 
 * without express or implied warranty. 
 *
 * Basic implementation of the Kubelka-Munk model for the composition
 * of layered surfaces.  See SIGGRAPH 2001 course notes, "Advanced 
 * RenderMan 3: Render Harder," for notes and background information.
 */

void sinhcosh(float t; output float sinh, cosh;) {
    if (t > 10) t = 10.;
    float ex = exp(t);
    float invex = 1. / ex;

    sinh = 0.5 * (ex - invex);
    cosh = 0.5 * (ex + invex);
}


void KMEstimateCoeffs(color Rinf; output color sigma_a, sigma_s;) {
    if (Rinf == color 0.) {
        sigma_a = color 1.;
        sigma_s = color 0.;
    }
    else {
        color a_over_s = (1. - Rinf) * (1. - Rinf) / (2. * Rinf);
        sigma_a = color 1.;
        sigma_s = sigma_a / a_over_s;
    }
}

void KM(color sigma_a, sigma_s; float thickness;
        output color R, T;) {
    float i;
    for (i = 0; i < ncomps; i += 1) {
        float s = comp(sigma_s, i), a = comp(sigma_a, i);
        float aa = (s+a)/s;
        float b = sqrt(max(aa*aa - 1., 0.));

        float sh, ch;
        sinhcosh(b*s*thickness, sh, ch);

        setcomp(R, i, sh / (aa * sh + b * ch));
        setcomp(T, i, b  / (aa * sh + b * ch));
    }
}

color KMInfinite(color sigma_a, sigma_s;) {
    float i;
    color R = 0;
    for (i = 0; i < ncomps; i += 1) {
        float a = comp(sigma_a, i) / comp(sigma_s, i);
        float r = 1. + a - sqrt(a*a + 2.*a);
        setcomp(R, i, r);
    }
    return R;
}

color KMComposeR(color R1, T1, R2, T2) {
    return R1 + T1*R2*T1 / (color 1. - R1*R2);
}

color KMComposeT(color R1, T1, R2, T2) {
    return T1*T2 / (color 1. - R1*R2);
}

color KMOverGlossy(normal Nf; color sigma_a, sigma_s, Kd, Ks;
                   float thickness, roughness;) {
    color R1, T1;
    KM(sigma_a, sigma_s, thickness, R1, T1);

    color Kdnew = KMComposeR(R1, T1, Kd, color 0.);
    color Ksnew = KMComposeR(0., T1, Ks, color 0.);

    float KsRatio = comp(Ks, 0) / comp(Ksnew, 0);
    float roughnew = roughness * KsRatio;

    extern vector I;
    return Kdnew*diffuse(Nf) + Ksnew*specular(Nf, -normalize(I), roughnew);
}
