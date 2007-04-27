/*
*
*
* Shader: Double side translucency shader
*
* Author: Xavier Matia Bernasconi - The Chimney Pot -
*
*
*
*
* Description:  It's possible to use one different color and
*		translucency texture for each side of a flat object
*		It also implement the Stephen H. Westin velvet
*		illuminance loop.	
*		
*
* Tips & Tricks: The transTx and TransTx2 values can be used,
*		 when a texture is present, as scale value for the texture.
*		 Don't use white on the translucency texture.
*		 It burns out the surface color.
*		 
* Notes: Please send me any kind of suggestions or bugs.
*	 If you use it in some production please email me the result.
*	 I'll really appriciate it.
*
* Shader birthdate: 31-08-2000
*
*
*/

#include "k3d_functions.h"

surface k3d_translucency(float Ka = 1;
			float Kd = 1;
			float Ks = 0.5;
			float roughness = 0.1;
			color specularcolor = 1;
			color sheen = 0.25;
			float blurcolorTx = 0;
			string colorTx = "";
			float blurcolorTx2 = 0;
			string colorTx2 = "";
			float colorS = 0;
			float colorT = 0;
			float colorScaleS = 1;
			float colorScaleT = 1;
			float transTx = 0.8;
			float transblurTx = 0;
			string transTex = "";
			float transTx2 = 0.8;
			float transblurTx2 = 0;
			string transTex2 = "";
			)
{
	/*XMB VARIABLE */
	
	float ss = (s - colorS) / colorScaleS;
	float tt = (t - colorT) / colorScaleT;
	
	/*Velvet illuminance loop variable*/

	vector H;
	vector ln;
	color shiny;
	float cosine, sine;



	/*XMB vector assignment*/
	
	normal Nf = faceforward(normalize(N),I);
	vector V = -normalize(I);
	
	/*Color texturing of different side*/
	
	color Ct;

	if (Nf.N>0)
	{
		if (colorTx != ""){
			float opac = float texture(colorTx[3], ss, tt);
			Ct = color texture(colorTx, ss, tt, "blur", blurcolorTx) + (1-opac)*Cs;
		} else Ct = Cs;
	}else
	{
	 	if (colorTx2 != ""){
			float opac = float texture(colorTx2[3], ss, tt);
			Ct = color texture(colorTx2, ss, tt, "blur", blurcolorTx2) + (1-opac)*Cs;
		} else Ct = Cs;
	}
	
	float Kt;
	
	/*Translucency texturing of different side*/

	if (Nf.N>0)
	{
		if (transTex != ""){
			 Kt = float texture(transTex[0], ss, tt, "blur", transblurTx);
			 Kt = Kt * transTx;
		} else Kt = transTx;
	}else
	{
		if (transTex2 != ""){
			 Kt = float texture(transTex2[0], ss, tt, "blur", transblurTx2);
			 Kt = Kt * transTx2;
		} else Kt = transTx2;
	}
	
	
	

	/*Velvet illuminance loop*/
	
	shiny = 0;
	
	illuminance (P, Nf, 1.57079632679489661923){
		ln = normalize(L);
		cosine = max (-Nf.V,0);
		shiny += pow (cosine, 1.0/roughness) / (ln.Nf) * Cl * sheen;
		cosine = max (Nf.V, 0);
		sine = sqrt (1.0-sqr(cosine));
		shiny += pow(sine, 10.0)*ln.Nf * Cl*sheen;
	}

		
	Ci = Ct * (Ka*ambient() + Kd*diffuse(Nf) + Kt*diffuse(-Nf)) + Ks*specular(Nf,V,roughness) + shiny;
	
	Oi = Os; Ci *= Oi;
}	
