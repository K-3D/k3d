/*
 * 2006-03-13 dan@3-e.net
 *
 */

#include "k3d_rmannotes.h"

surface k3d_toonmap (

float Ka=0.327; 
float Kd=0.336; 
float Ks=0.929; 
float roughness=0.107; 
string Highlight_Map="";
color Highlight_Color=(1, 1, 1); 
float channel4=0; 
float swidth4=1; 
float twidth4=1; 
float samples4=1; 
float ss4=1; 
float tt4=1; 
string Paint_Map="";
color Paint_Color=(0.5, 0.5, 0.5); 
float channel3=0; 
float swidth3=1; 
float twidth3=1; 
float samples3=1; 
float ss3=1; 
float tt3=1; 
float paint_spec=0.0888; 
float paint_trans=0.539; 
float paint_fuzz=0.274; 
float ink_thresh=0.389; 
float ink_fuzz=0.765; 
string Ink_Map=""; 
color Ink_Color=(0, 0, 0);
float channel2=0; 
float swidth2=1; 
float twidth2=1; 
float samples2=1; 
float ss2=1; 
float tt2=1; ) 
{
/** Surface main-code start **/

/** Texture_Highlight_Map **/
color temt_c4;
float temp_ss4;
float temp_tt4;
if (ss4 ==1) {temp_ss4=s;} else {temp_ss4=ss4;}
if (tt4 ==1) {temp_tt4=t;} else {temp_tt4=tt4;}
if ( Highlight_Map=="" ) 
{
temt_c4=Highlight_Color;
} 
else
{
if (channel4 < 0 || channel4 > 3)
{
temt_c4=texture(Highlight_Map,temp_ss4,temp_tt4,"swidth",swidth4,"twidth",twidth4,"samples",samples4);
} 
else 
{
temt_c4=texture(Highlight_Map[channel4],temp_ss4,temp_tt4,"swidth",swidth4,"twidth",twidth4,"samples",samples4);
}
}
/** Texture_Paint_Map **/
color temt_c3;
float temp_ss3;
float temp_tt3;
if (ss3 ==1) {temp_ss3=s;} else {temp_ss3=ss3;}
if (tt3 ==1) {temp_tt3=t;} else {temp_tt3=tt3;}
if ( Paint_Map=="" ) 
{
temt_c3=Paint_Color;
} 
else
{
if (channel3 < 0 || channel3 > 3)
{
temt_c3=texture(Paint_Map,temp_ss3,temp_tt3,"swidth",swidth3,"twidth",twidth3,"samples",samples3);
} 
else 
{
temt_c3=texture(Paint_Map[channel3],temp_ss3,temp_tt3,"swidth",swidth3,"twidth",twidth3,"samples",samples3);
}
}
/** Texture_Ink_Map **/
color temt_c2;
float temp_ss2;
float temp_tt2;
if (ss2 ==1) {temp_ss2=s;} else {temp_ss2=ss2;}
if (tt2 ==1) {temp_tt2=t;} else {temp_tt2=tt2;}
if ( Ink_Map=="" ) 
{
temt_c2=Ink_Color;
} 
else
{
if (channel2 < 0 || channel2 > 3)
{
temt_c2=texture(Ink_Map,temp_ss2,temp_tt2,"swidth",swidth2,"twidth",twidth2,"samples",samples2);
} 
else 
{
temt_c2=texture(Ink_Map[channel2],temp_ss2,temp_tt2,"swidth",swidth2,"twidth",twidth2,"samples",samples2);
}
}
#define blend(a,b,x) ((a) * (1 - (x)) + (b) * (x))
#define union(a,b) ((a) + (b) - (a) * (b))

float
toonspec(vector N, V; float roughness)
{
float C = 0;
vector H;

illuminance(P, N, PI/2) {
H = normalize(normalize(L)+V);
C += pow(N.H, 1/roughness);
}
return C;
}

float cos_here;
float diff, spec;
normal Nf;
vector normI, half;

color toon_color, layer_color;
color toon_opac, layer_opac;

Nf = faceforward(normalize(N), I);
normI = normalize(I);

/* background layer (layer 0) */
toon_color = color (0, 0, 0);
toon_opac = Os;

/* toon paint (layer 1) */

/* only care about percentage illumination */
diff = 0;
illuminance(P, Nf, PI/2)
diff += normalize(L).Nf; 

/* antialias the transition */
diff = smoothstep(paint_trans - paint_fuzz/2, paint_trans + paint_fuzz/2, diff);

/* specular -- transition also antialiased */ 
spec = toonspec(Nf, -normalize(I), roughness);
spec = smoothstep(paint_spec - paint_fuzz/2, paint_spec + paint_fuzz/2, spec);

layer_color = Cs*(Kd*diff*temt_c3 + Ka*ambient()) + temt_c4*Ks*spec;
layer_opac = Os;

toon_color = blend(toon_color, layer_color, layer_opac);
toon_opac = union(toon_opac, layer_opac);

/* toon ink (layer 3) */

cos_here = normalize(Nf).normI;

/* antialias ink lines */
layer_opac = 1 - smoothstep(ink_thresh - ink_fuzz/2, ink_thresh + ink_fuzz/2, abs(cos_here));

if (abs(cos_here) < ink_thresh)
layer_color = temt_c2;
else
layer_color = 0;

toon_color = blend(toon_color, layer_color, layer_opac);
toon_opac = union(toon_opac, layer_opac);

color toonOi = toon_opac;
color toonCi = toon_opac * toon_color;

/** Surface main-code end **/

Ci = toonCi;
Oi = toon_opac;
}
