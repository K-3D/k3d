/* renamed shader to SIG2k_srf_deformation to be consistent with RMR 
 *    -- tal@SpamSucks_renderman.org
 */


/* 
   deformation surface shader
   
   projects a texture through the camera onto the Pref
   object and deforms it to the P position
          
   additionally, calculates the changes to shading on the surface 
   measured by the change in diffuse lighting from the Pref to P.
   
   additional contrast and color controls are left as an exersize
   for the user.
   
   by Rob Bredow and Scott Stokdyk 
*/

color
fnc_mydiffuse (color Cl; 
	       vector L; 
	       normal N) 
{
    normal Nn; 
    vector Ln;

    Nn = normalize(N);
    Ln = normalize(L);
    return Cl * max(Ln.Nn,0);   
}


void 
fnc_projectCurrentCamera(point P;
			 output float X, Y;)
{
    point Pndc = transform("NDC", P);

    X = xcomp(Pndc);
    Y = ycomp(Pndc);
}


surface 
k3d_srfdeformation(
    string texname = "";        /* Texture to project */
    float debug = 0;            /* 0 = deformed lit image
				   1 = texture deformed with no lighting
				   2 = output lighting of the P object
				   3 = output lighting of the Pref object
				*/
    float Kd=1;                 /* Surface Kd for lighting calculations */
    
    varying point Pref = point "shader" (0,0,0);
    )
{
    float x, y;
    color Ci0, Ci1, Ci2;
    normal N1, N2;
    float illum_width = 180;

    point Porig = Pref;

    fnc_projectCurrentCamera(Pref, x, y);

    if (texname != "") {
	Ci0 = texture(texname, x, y);
    }
    else Ci0 = 0.5;
    
    /* Calculate shading difference between P and Porig*/

    N = normalize(calculatenormal(P));
    N1 = faceforward(normalize(N),I);
    N = normalize(calculatenormal(Porig));
    N2 = faceforward(normalize(N),I);

    Ci1 = 0.0; Ci2 = 0.0;

    /* These lighting loops can be enhanced to calculate
       specular or reflection maps if needed */
    
    illuminance(P, N1, radians(illum_width)) {
	Ci1 += Kd * fnc_mydiffuse(Cl,L,N1);
    }

    illuminance(Porig, N2, radians(illum_width)) {
	Ci2 += Kd * fnc_mydiffuse(Cl,L,N2);
    }

    /* Difference in lighting acts as brightness control*/

    Ci = Ci0 * (1+(Ci1-Ci2));
    Oi = 1.0;

    if (debug == 1) {  /* output the texture - no lighting */
	Ci = Ci0;
    } else if (debug == 2) { /* output texture with P's lighting */
	Ci = Ci1;
    } else if (debug == 3) { /* output texture with Pref's lighting */
	Ci = Ci2;
    }
}
