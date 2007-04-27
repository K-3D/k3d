/* 2006-03-13 dan@3-e.net */ 
#include "k3d_rmannotes.h" 

 normal
 Displace (normal dir; float amp; float truedisp;)
 {
 extern point P;
 float spacescale = length(vtransform("shader", dir));
 vector Ndisp = dir * (amp / max(spacescale,1e-6));
 P += truedisp * Ndisp;
 return normalize (calculatenormal (P + (1-truedisp)*Ndisp));
 }

 displacement k3d_grids_disp (
float freq0=22.2; 
float rotation0=26.5; 
float fuzz0=0.136; 
float linewidth0=0.368; 
color SurfaceColor=color(1,1,1); 
color GridColor=color(0,0,0); 
float freq1=44.9; 
float rotation1=61.8; 
float fuzz1=0.269; 
float linewidth1=0.122; )
 {
 
float ss0, tt0;

rotate2d(s, t, radians(rotation0), 0.5, 0.5, ss0, tt0);
ss0 = repeat(ss0, freq0);
tt0 = repeat(tt0, freq0);

color layer_opac0 = pulse(linewidth0, 1-linewidth0, fuzz0, tt0);
color Ct0 = blend(SurfaceColor, GridColor, layer_opac0);

float ss1, tt1;

rotate2d(s, t, radians(rotation1), 0.5, 0.5, ss1, tt1);
ss1 = repeat(ss1, freq1);
tt1 = repeat(tt1, freq1);

color layer_opac1 = pulse(linewidth1, 1-linewidth1, fuzz1, tt1);
color Ct1 = blend(SurfaceColor, GridColor, layer_opac1);
float x=0.5; 
/** ColorToFloat_Brick_2 **/
float temp_f_2=(comp((x*Ct0+(1-x)*Ct1),0)+comp((x*Ct0+(1-x)*Ct1),1)+comp((x*Ct0+(1-x)*Ct1),2))/3;
 N = Displace(normalize(N),0.05*temp_f_2,0);
 }

