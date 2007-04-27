/************************************************************************
 * project.h - Routines for texture projection
 *
 * Author: Larry Gritz (gritzl@acm.org)
 *
 * $Revision: 1.1 $    $Date: 2004/05/19 18:15:20 $
 *
 ************************************************************************/



#ifndef PROJECT_H
#define PROJECT_H 1


#ifndef FILTERWIDTH_H
#include "k3d_filterwidth.h"
#endif


/* Project 3-D points onto a unit sphere centered at the origin */
void
spherical_projection (point p; output float ss, tt, ds, dt;)
{
    extern float du, dv;    /* Used by the filterwidth macro */
    vector V = normalize(vector p);
    ss = (-atan (ycomp(V), xcomp(V)) + PI) / (2*PI);
    tt = 0.5 - acos(zcomp(V)) / PI;
    ds = filterwidth (ss);
    if (ds > 0.5)
	ds = max (1-ds, MINFILTWIDTH);
    dt = filterwidth (tt);
    if (dt > 0.5)
	dt = max (1-dt, MINFILTWIDTH);
}
    


/* Project 3-D points onto a cylinder about the z-axis between z=0 and z=1 */
void
cylindrical_projection (point p; output float ss, tt, ds, dt;)
{
    extern float du, dv;    /* Used by the filterwidth macro */
    vector V = normalize(vector p);
    ss = (-atan (ycomp(V), xcomp(V)) + PI) / (2*PI);
    tt = zcomp(p);
    ds = filterwidth (ss);
    if (ds > 0.5)
	ds = max (1-ds, MINFILTWIDTH);
    dt = filterwidth (tt);
}




void
ProjectTo2D (string projection;
	     point P; string whichspace;
	     matrix xform;
	     output float ss, tt;
	     output float ds, dt; )
{
    point Pproj;
    extern float du, dv;    /* Used by the filterwidth macro */
    if (projection == "st") {
	extern float s, t;
	Pproj = point (s, t, 0);
    } else {
        Pproj = transform (whichspace, P);
    }
#pragma nolint 2
    Pproj = transform (xform, Pproj);
    if (projection == "planar" || projection == "st") {
	ss = xcomp(Pproj);
	tt = ycomp(Pproj);
	ds = filterwidth (ss);
	dt = filterwidth (tt);
    }
    else if (projection == "perspective") {
	float z = max (zcomp(Pproj), 1.0e-6);  /* avoid zero division */
	ss = xcomp(Pproj) / z;
	tt = ycomp(Pproj) / z;
	ds = filterwidth (ss);
	dt = filterwidth (tt);
    }
    /* Special cases for the projections that may wrap */
    else if (projection == "spherical")
	spherical_projection (Pproj, ss, tt, ds, dt);
    else if (projection == "cylindrical")
	cylindrical_projection (Pproj, ss, tt, ds, dt);
}



color
GetColorTextureAndAlpha (string texturename;
			 string projection;
			 point P; string whichspace;
			 matrix xform;
			 float blur;
			 uniform float alphachannel;
			 output float alpha;
    )
{
    float ss, tt, ds, dt;
    ProjectTo2D (projection, P, whichspace, xform, ss, tt, ds, dt);
    ds *= 0.5;  dt *= 0.5;
    color Ct = color texture (texturename, ss-ds, tt-dt, ss+ds, tt-dt,
			      ss-ds, tt+dt, ss+ds, tt+dt, "blur", blur);
    alpha = float texture (texturename[alphachannel], ss-ds, tt-dt,
			   ss+ds, tt-dt, ss-ds, tt+dt, ss+ds, tt+dt,
			   "blur", blur, "fill", 1);
    return Ct;
}



float
GetFloatTextureAndAlpha (string texturename;
			 string projection;
			 point P; string whichspace;
			 matrix xform;
			 float blur;
			 uniform float alphachannel;
			 output float alpha;
    )
{
    float ss, tt, ds, dt;
    ProjectTo2D (projection, P, whichspace, xform, ss, tt, ds, dt);
    ds *= 0.5;  dt *= 0.5;
    float val = float texture (texturename, ss-ds, tt-dt, ss+ds, tt-dt,
			       ss-ds, tt+dt, ss+ds, tt+dt, "blur", blur);
    alpha = float texture (texturename[alphachannel], ss-ds, tt-dt,
			   ss+ds, tt-dt, ss-ds, tt+dt, ss+ds, tt+dt,
			   "blur", blur, "fill", 1);
    return val;
}



color
ApplyColorTextureOver (color basecolor;
		       string texturename;
		       string projection;
		       point P; string whichspace;
		       matrix xform;
		       float blur;
    )
{
    float alpha;
    color Ct = GetColorTextureAndAlpha (texturename, projection, P, whichspace,
					xform, blur, 3, alpha);
    return Ct + (1-alpha)*basecolor;
}



float
ApplyFloatTextureOver (float baseval;
		       string texturename;
		       string projection;
		       point P; string whichspace;
		       matrix xform;
		       float blur;
    )
{
    float alpha;
    float x = GetFloatTextureAndAlpha (texturename, projection, P, whichspace,
					xform, blur, 3, alpha);
    return x + (1-alpha)*baseval;
}


/* Helper function to convert arrays of 16 floats to matrices */
matrix array_to_mx (float mx[16])
{
    return matrix (mx[0], mx[1], mx[2], mx[3],
		   mx[4], mx[5], mx[6], mx[7],
		   mx[8], mx[9], mx[10], mx[11],
		   mx[12], mx[13], mx[14], mx[15]);
}




#endif /* PROJECT_H */
