/* renamed shader to SIG2k_srf_contact to be consistent with RMR 
 *    -- tal@SpamSucks_renderman.org
 */

/*  
**  
** Render a contact shadow based on depth data derived from a light  
** placed onto the surface which catches the contact shadow  
**  
** by Rob Engle and Jim Berney  
*/  

surface 
k3d_contactshadow (  
    string shadowname = "";     /* the name of the texture file */  
    float samples = 10;         /* how many samples to take per Z lookup */  
    float influence = 1.0;      /* world space distance in which effect is visible */  
    float gamma = 0.5;          /* controls ramp on of effect over distance */  
    float maxdist = 10000;      /* how far is considered infinity */  
    )
{  
    /* get a matrix which transforms from current space to the  
       camera space used when rendering the shadow map */  
    uniform matrix matNl;  
    textureinfo(shadowname, "viewingmatrix", matNl);  
    
    /* get a matrix which transforms from current space to the  
       screen space (-1..1) used when rendering the shadow map */  
    uniform matrix matNP;  
    textureinfo(shadowname, "projectionmatrix", matNP);  

    /* transform the ground plane point into texture coordinates  
       needed to look up the point in the shadow map */  
    point screenP = transform(matNP, P);  
    float ss = (xcomp(screenP) + 1) * 0.5;  
    float tt = (1 - ycomp(screenP)) * 0.5;  

    if (ss < 0 || ss > 1 || tt < 0 || tt > 1) {  
	/* point being shaded is outside the region of the depth map */  
	Ci = color(0);  
    }  
    else {  
	/* get the distance from the shadow camera to the closest
	   object as recorded in the shadow map */
	float mapdist = float texture(shadowname, ss, tt, "samples", samples);
	
	/* transform the point on the ground plane into the shadow
	   camera space in order to get the distance from the shadow
	   camera to the ground plane */
	point cameraP = transform(matNl, P);  
	
	/* the difference between the two distances is used to calculate the  
	   contact shadow effect */  
	float distance = mapdist - zcomp(cameraP);  
	
	distance = smoothstep(0, 1, distance/influence);  
	distance = pow(distance, gamma);  

	/* convert into a color (white=shadow) */  
	Ci = (1.0-distance);  
    }  
}
