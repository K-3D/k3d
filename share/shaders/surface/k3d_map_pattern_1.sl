surface k3d_map_pattern_1 (
string map1=""; 
float channel1=4; 
float swidth1=1; 
float twidth1=1; 
float samples1=1; 
float ss1=1; 
float tt1=1; 
float txtscale=1; 
color lightwood=color(0.69,0.44,0.25); 
color darkwood=color(0.35,0.22,0.08); 
float grainy=1; 
float Ka = 1;
float Kd = .5; 
float Ks = .5; 
float roughness = .1;
color specularcolor = 1;
) 
{
/** Surface main-code start **/

/** Texture_Brick_1 **/
color temt_c1;
float temp_ss1;
float temp_tt1;
if (ss1 ==1) {temp_ss1=s;} else {temp_ss1=ss1;}
if (tt1 ==1) {temp_tt1=t;} else {temp_tt1=tt1;}
if ( map1=="" ) 
{
temt_c1=0;
} 
else
{
if (channel1 < 0 || channel1 > 3)
{
temt_c1=texture(map1,temp_ss1,temp_tt1,"swidth",swidth1,"twidth",twidth1,"samples",samples1);
} 
else 
{
temt_c1=texture(map1[channel1],temp_ss1,temp_tt1,"swidth",swidth1,"twidth",twidth1,"samples",samples1);
}
}
/** ColorToFloat_Brick_3 **/
float temp_f_3=(comp(temt_c1,0)+comp(temt_c1,1)+comp(temt_c1,2))/3;
point PP, PQ;
color Ct; 
float r, r2;
float my_t;
color C_temp1;

PP = txtscale * transform ("shader", P);

my_t = zcomp(PP) / temp_f_3;
PQ = point (xcomp(PP)*8, ycomp(PP)*8, zcomp(PP));
my_t += noise (PQ) / 16;

PQ = point (xcomp(PP), my_t, ycomp(PP)+12.93);
r = temp_f_3 * noise (PQ);
r -= floor (r);
r = 0.2 + 0.8 * smoothstep(0.2, 0.55, r) * (1 - smoothstep(0.75, 0.8, r));

PQ = point (xcomp(PP)*128+5, zcomp(PP)*8-3, ycomp(PP)*128+1);
r2 = grainy * (1.3 - noise (PQ)) + (1-grainy);

Ct = mix (lightwood, darkwood, r*r2*r2);

/** Surface main-code end **/
normal Nf = faceforward(normalize(N), I);
C_temp1 = Os * (Cs * (Ka * ambient() + Kd * diffuse(Nf)) + specularcolor * Ks * specular(Nf, -normalize(I), roughness));
Ci = mix (C_temp1, Ct, 0.5);
  Oi = Os;
  Ci *= Oi;
}
