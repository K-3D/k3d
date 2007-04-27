/****************************************************************************
* FGHexaTile_bump.sl -- "Simple shader to do "Hexagon" displacement"
*
* Description:
* A simple bump shader that tile a "Hexagon" pattern
*
*
* Parameters:
* 	string Name 	= "FGHexaTile_bump"	---	Name of the shader "do nothing".
*	float Edge1	=.2 			---	Minvalue of Edge.
*	float Edge2	=.5 			---	Maxvalue of Edge.
*	float ScaleSS	= 3 			---	Scale the "s" direction.
*	float ScaleTT	= 3 			---	Scale the "t" direction.
*	float Rotate	= 180 			---	Rotate the tile default "Z-direction"
*	point P1	= point(0,0,0) 		---	Center point for Rotation.
*	point P2	= point(0,0,1) 		---	The x,y,z direction for Rotate.
*	string Projection = "planar" 		---	Projection "preoject.h"
*	string Space	  = "shader" 		---	Space for projection.
*	float mx[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}  --- 16 float for scale,rotate,translate.
*	float C1	= 1 			---	Multiply of f1 the closes future point.
*	float C2	= 2 			---	Multiply of f2 the second closes future point.
*	float Km	= -.015 		---	Diplacement amplitude.
*	float Truedisp	= 1 			---	True Displacement 0 = off, 1 = on.
*
*
***************************************************************************
*
* Author: Fredrik Gustafsson, gusten@SpamSucks_algonet.se
*
* Reference:
* Larry Gritz "noises.h", "project.h", "patterns.h"
* Arman "Advanced Renderman-"Creating CGI for Motion Picture.
*
* Revision:1.0 Date: 2002/02/25
*
****************************************************************************/

#include "k3d_project.h"
#include "k3d_patterns.h"

void
Worley2D_f1f2 (float ss, tt; float jitter, seed, c1, c2, hexagon, scale_ss, scale_tt; 
		output float f1; output float spos1, tpos1; output float f2; output float spos2, tpos2;)
{

float sthiscell = floor(ss)+0.5, tthiscell = floor(tt)+0.5;

f1 = f2 = 1000;

uniform float i, j;
	for (i = -1; i <= 1; i += 1) {
		float stestcell = sthiscell + i;
			for (j = -1; j <= 1; j += 1) {
				float ttestcell = tthiscell + j;

/* Hexagon code */
if ((hexagon != 0) && abs(mod(stestcell, 2)) < 1)
	ttestcell += 0.5;

	float spos = stestcell + jitter * (float cellnoise(stestcell,seed+ttestcell) - 0.5);
	float tpos = ttestcell + jitter * (float cellnoise(stestcell+23,seed+ttestcell-87) - 0.5);
	float soffset = (spos - ss) * scale_ss;
	float toffset = (tpos - tt) * scale_tt;
	float dist = soffset*soffset + toffset*toffset;

if (dist < f1) {
	f2 = f1; spos2 = spos1; tpos2 = tpos1;
	f1 = dist; spos1 = spos; tpos1 = tpos;
} else if (dist < f2) {
	f2 = dist;
	spos2 = spos; tpos2 = tpos;
	}
  }
}
f1 = f1 * c1; f2 = f2 * c2;
}

displacement k3d_hexatile_bump (
	string Name 	= "FGHexaTile_bump";
	float Edge1	=.2; 
	float Edge2	=.5; 
	float ScaleSS	= 3; 
	float ScaleTT	= 3; 
	float Rotate	= 180; 
	point P1	= point(0,0,0); 
	point P2	= point(0,0,1); 
	string Projection = "planar"; 
	string Space	  = "shader"; 
	float mx[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}; 
	float C1	= 1; 
	float C2	= 2; 
	float Km	= -.015; 
	float Truedisp	= 1; ) 
{ 

float SS,TT,Ds,Dt;
ProjectTo2D (Projection,rotate(P,radians(Rotate),P1,P2),Space,array_to_mx(mx),SS,TT,Ds,Dt);

float f1_out_15 = 0,f2_out_15 = 0,spos1_out_15 = 0,tpos1_out_15 = 0,spos2_out_15 = 0,tpos2_out_15 = 0;

float scale1 = ScaleSS * SS;
float scale2 = ScaleTT * TT;

Worley2D_f1f2 (scale1,scale2,/* Jitter */ 0,/* Seed */ 0,C1,C2,/* Hexagon */ 1,/* TileScaleSS */ 1,
			/* TileScaleTT */ 1,/* f1_result */ f1_out_15,spos1_out_15, tpos1_out_15,
				/* f2_result */ f2_out_15,spos2_out_15,tpos2_out_15);

/*** ----- Exponential "sqrt(float X)" ----- ***/
float SQRT_f2 = sqrt(f2_out_15);

/*** ----- SmoothPulse macro ----- ***/
float smoothp_20 = smoothpulse (0,Edge1,Edge2,1,SQRT_f2);

/*** ----- Exponential "sqrt(float X)" ----- ***/
float SQRT_f1 = sqrt(f1_out_15);

/*** ----- SmoothPulse macro ----- ***/
float smoothp_22 = smoothpulse (0,Edge1,Edge2,1,SQRT_f1);

/*** ----- True Displacement and bump ----- ***/
normal 
Displace (normal dir; vector space; float amp; float truedisp;)
{
	extern point P;
	float spacescale = length(space);
	vector Ndisp = dir * (amp / max(spacescale,1e-6));
P += truedisp * Ndisp;
return normalize (calculatenormal (P + (1-truedisp)*Ndisp));
}

vector VShd = vtransform("shader", vector(normalize(N)));

float disp = clamp(smoothp_20*smoothp_22,0,0.95);

N = Displace(normalize(N),VShd,Km*disp,Truedisp); 
}

