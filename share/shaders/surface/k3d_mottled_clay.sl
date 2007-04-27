

surface k3d_mottled_clay (

float Ka1=0.8; 
float Kd1=0.5; 
float roughness1=0.1; 
color claycol1=color(0.88,0.74,0.42); 
color claycol2=color(0.59,0.37,0.19); 
float stucco_freq3=15; 
float transp=1; ) 
{
/** Surface main-code start **/

point PP6=point(0.0,0.0,0.0); 
/** Basic fractal **/
point temp_PP6;
if (PP6 == point(0.0,0.0,0.0)) {temp_PP6=P;} else {temp_PP6=PP6;}
temp_PP6=transform("shader",P);
uniform float i6;
float size6,sum6,area6;
sum6 = 0;
size6 = 1;
area6 = sqrt(area(temp_PP6));
while(area6<size6)
{
sum6 += size6*noise(temp_PP6/size6);
size6 /= 2;
}
/*** stucco pattern code ***/ 
float magnitude3; 
point PP3; 
PP3 = transform ("shader", P); 
magnitude3 = pow (noise(PP3 * stucco_freq3), sum6);
color
LocIllumOrenNayar (normal N; vector V; float roughness;)
{
/* Surface roughness coefficients for Oren/Nayar's formula */
float sigma2 = roughness * roughness;
float A = 1 - 0.5 * sigma2 / (sigma2 + 0.33);
float B = 0.45 * sigma2 / (sigma2 + 0.09);
/* Useful precomputed quantities */
float theta_r = acos (V . N); /* Angle between V and N */
vector V_perp_N = normalize(V-N*(V.N)); /* Part of V perpendicular to N */

/* Accumulate incoming radiance from lights in C */
color C = 0;
extern point P;
illuminance (P, N, PI/2) {
/* Must declare extern L & Cl because we're in a function */
extern vector L; extern color Cl;
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

normal Nf1 = faceforward (normalize(N), I);
color ci1 = (float(magnitude3)*claycol1+(1-float(magnitude3))*claycol2) * (Ka1*ambient() + Kd1*LocIllumOrenNayar(Nf1,-normalize(I),roughness1));

/** Surface main-code end **/

Ci = ci1;
Oi = color(transp);
}
