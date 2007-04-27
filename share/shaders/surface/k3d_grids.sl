/** Grid shader copyright 2005 Daniel Scott Matthews dan@3-e.net **/

#include "k3d_rmannotes.h"
#include "k3d_noises.h"

surface k3d_grids (

float K_ambient=0.993; 
float K_diffuse=0.381; 
float K_specular=0.269; 
float roughness=0.0373; 
color specularcolor=color(1, 1, 1); 
color ambientcolor=color(0.5,0.5,0.5); 

float G1L1_freq=33.2; 
float G1L1_rotation=46.3; 
float G1L1_fuzz=0.453; 
float G1L1_linewidth=0.579; 
float G1L1_Randomness=0;
color G1L1_SurfaceColor=color(1,1,1); 
color G1L1_GridColor=color(0,0,0); 

float G1L2_freq=6.87; 
float G1L2_rotation=42.4; 
float G1L2_fuzz=0.363; 
float G1L2_linewidth=0.209; 
float G1L2_Randomness=0;
color G1L2_SurfaceColor=color(1,1,1); 
color G1L2_GridColor=color(0,0,0); 

float G2L1_freq=10.4; 
float G2L1_rotation=0.162; 
float G2L1_fuzz=0.0655; 
float G2L1_linewidth=0.281; 
float G2L1_Randomness=0;
color G2L1_SurfaceColor=color(1,1,1); 
color G2L1_GridColor=color(0,0,0); 

float G2L2_freq=16.7; 
float G2L2_rotation=44.2; 
float G2L2_fuzz=0.306; 
float G2L2_linewidth=0.149; 
float G2L2_Randomness=0;
color G2L2_SurfaceColor=color(1,1,1); 
color G2L2_GridColor=color(0,0,0); 
) 
{
/** Surface main-code start **/

float G1L1_ss, G1L1_tt;
float G1L2_ss, G1L2_tt;
float G2L1_ss, G2L1_tt;
float G2L2_ss, G2L2_tt;
float blend_val=0.5; 

/** Grid 1 Color **/
/** line set 1 **/
rotate2d(s, t, radians(G1L1_rotation), 0.5, 0.5, G1L1_ss, G1L1_tt);
G1L1_ss = repeat(G1L1_ss, G1L1_freq);
G1L1_tt = repeat(G1L1_tt, G1L1_freq);

color G1L1_layer_opac = pulse(G1L1_linewidth, 1-G1L1_linewidth, G1L1_fuzz, G1L1_tt);
color G1L1_Ct = blend(G1L1_SurfaceColor, G1L1_GridColor, G1L1_layer_opac);

/** line set 2 **/
rotate2d(s, t, radians(G1L2_rotation), 0.5, 0.5, G1L2_ss, G1L2_tt);
G1L2_ss = repeat(G1L2_ss, G1L2_freq);
G1L2_tt = repeat(G1L2_tt, G1L2_freq);

color G1L2_layer_opac = pulse(G1L2_linewidth, 1-G1L2_linewidth, G1L2_fuzz, G1L2_tt);
color G1L2_Ct = blend(G1L2_SurfaceColor, G1L2_GridColor, G1L2_layer_opac);
normal G1_Nf = faceforward(normalize(N), I);
vector G1_V = -normalize(I);

/** Grid 2 Opacity **/
/** line set 3 **/
rotate2d(s, t, radians(G2L1_rotation), 0.5, 0.5, G2L1_ss, G2L1_tt);
G2L1_ss = repeat(G2L1_ss, G2L1_freq);
G2L1_tt = repeat(G2L1_tt, G2L1_freq);

color G2L1_layer_opac = pulse(G2L1_linewidth, 1-G2L1_linewidth, G2L1_fuzz, G2L1_tt);
color G2L1_Ct = blend(G2L1_SurfaceColor, G2L1_GridColor, G2L1_layer_opac);

/** line set 4 **/
rotate2d(s, t, radians(G2L2_rotation), 0.5, 0.5, G2L2_ss, G2L2_tt);
G2L2_ss = repeat(G2L2_ss, G2L2_freq);
G2L2_tt = repeat(G2L2_tt, G2L2_freq);

color G2L2_layer_opac = pulse(G2L2_linewidth, 1-G2L2_linewidth, G2L2_fuzz, G2L2_tt);
color G2L2_Ct = blend(G2L2_SurfaceColor, G2L2_GridColor, G2L2_layer_opac);

/** Surface main-code end **/

Ci = (blend_val*G1L1_Ct+(1-blend_val)*G1L2_Ct) * (ambientcolor * K_ambient * ambient() + K_diffuse * diffuse(G1_Nf)) + 
specularcolor * K_specular * phong(G1_Nf, G1_V, 1/roughness);
Oi = (blend_val*G2L1_Ct+(1-blend_val)*G2L2_Ct);
}
