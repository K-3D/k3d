/************************************************************************
 * reflections.h - Functions which compute reflected light by either
 *                 ray tracing or environment mapping.
 *
 * Author: Larry Gritz (gritzl@acm.org)
 *
 * $Revision: 1.1 $    $Date: 2004/05/19 18:15:20 $
 *
 ************************************************************************/


#ifndef REFLECTIONS_H
#define REFLECTIONS_H

#include "k3d_filterwidth.h"
#include "k3d_raysphere.h"


/* ReflMap() - Use a reflection map for reflections in flat objects.
 * Inputs are:
 *    reflname - filename of reflection map
 *    P - origin of traced ray
 *    blur - amount of additional blur to add to environment map
 * Outputs are:
 *    return value - the color of incoming light
 *    alpha - opacity of reflection map lookup in the direction R.
 * Warning -  the texture call itself takes derivatives, causing
 * trouble if called inside a loop or varying conditional!  Be cautious.
 */
color ReflMap ( string reflname;  point P; float blur;
                output float alpha; )
{
    /* Transform to the space of the environment map */
    point Pndc = transform ("NDC", P);
    float x = xcomp(Pndc), y = 1-ycomp(Pndc);
    alpha = float texture (reflname[3], x, y, "blur", blur, "fill", 1);
    return color texture (reflname, x, y, "blur", blur);
}



/* Environment() - A replacement for ordinary environment() lookups, this
 * function ray traces against an environment sphere of known, finite 
 * radius.  Inputs are:
 *    envname - filename of environment map
 *    envspace - name of space environment map was made in
 *    envrad - approximate supposed radius of environment sphere
 *    P, R - position and direction of traced ray
 *    blur - amount of additional blur to add to environment map
 * Outputs are:
 *    return value - the color of incoming environment light
 *    alpha - opacity of environment map lookup in the direction R.
 * Warning -  the environment call itself takes derivatives, causing
 * trouble if called inside a loop or varying conditional!  Be cautious.
 */
color Environment ( string envname, envspace;  uniform float envrad;
                    point P;  vector R;  float blur; output float alpha;)
{
    /* Transform to the space of the environment map */
    point Psp = transform (envspace, P);
    vector Rsp = normalize (vtransform (envspace, R));
    uniform float r2 = envrad * envrad;
    /* Clamp the position to be *inside* the environment sphere */
    if ((vector Psp).(vector Psp) > r2)
        Psp = point (envrad * normalize (vector Psp));
    float t0, t1;
    if (raysphere (Psp, Rsp, envrad, 1.0e-4, t0, t1) > 0)
	Rsp = vector (Psp + t0 * Rsp);
    alpha = float environment (envname[3], Rsp, "blur", blur, "fill", 1);
    return color environment (envname, Rsp, "blur", blur);
}




/* RayTrace() - A fancy ray trace routine, particularly suitable for
 * use with the "ray server."  Tries to sample over the surface
 * element and over the varying ray spread due to surface curvature.
 * An ordinary call to trace would point sample the environment in a
 * very simplistic way.  This function takes the size of the surface
 * facet and curvature of the surface into account, and lets you
 * sample the space with multiple rays.
 * 
 * Inputs:
 *    P - surface position
 *    Rdir - the unit-length reflection direction.
 *    blur - reflection blurriness; 0 = sharp reflection
 *    jitter - when 1, fully jitter the stochastic ray directions.  Lower
 *          numbers jitter less, 0 doesn't jitter.  Lowering jitter may help
 *          alleviate "sparkling" due to animation with low nrays.
 *    nsamples - number of rays with which to sample.  Larger numbers will
 *          yield better-sampled reflections, but will be more expensive.
 *          Note that the function reduces this number for secondary rays,
 *          assuming that the distribution from primary rays will be
 *          sufficient!
 * Return value: the average of the trace calls.
 * 
 * Warning!!! This function takes derivatives to find out the ray spread!
 * This can cause trouble if RayTrace() is called inside a loop or varying
 * conditional!  Be cautious.
 */
color
RayTrace (point P;  vector Rdir;  float Kr, blur, jitter;  
          uniform float nsamples;  output float alpha;)
{
#if (defined(BMRT) || defined(RAYSERVER_H))
    float rand () {
        extern float jitter;
        return (raylevel()==0) ? (0.5 + jitter * (float random() - 0.5)) : 0.5;
    }
    extern float du, dv;
    color C, Ct;
    float hitdist; point Phit, Pmiss;  vector Nhit, Rmiss;
    float bluramt = blur + filterwidthp(Rdir);
    uniform float nrays = (raylevel() == 0 ? max(1,ceil(sqrt(nsamples))) : 1);
    vector Tu = Du(P) * (1.5 * du); /* overblur just a tad... */
    vector Tv = Dv(P) * (1.5 * dv);
    if (Kr < 0.0001) {
	C = 0;
    } else if (bluramt > 0 || nrays > 1) {
        /* Construct orthogonal components to Rdir */
        vector uoffset = blur * normalize (vector (zcomp(Rdir) - ycomp(Rdir),
                                                   xcomp(Rdir) - zcomp(Rdir),
                                                   ycomp(Rdir) - xcomp(Rdir)));
        vector voffset = Rdir ^ uoffset;
        uniform float i, j;
        C = 0;  alpha = 0;
        for (i = 0;  i < nrays;  i += 1) {
            for (j = 0;  j < nrays;  j += 1) {
                /* Add a random offset to the smooth reflection vector */
                vector R = Rdir + ((i + rand())/nrays - 0.5) * uoffset +
                                  ((j + rand())/nrays - 0.5) * voffset;
		R = normalize(R);
                point Pray = P +  ((j + rand())/nrays - 0.5) * Tu +
                                  ((i + rand())/nrays - 0.5) * Tv;
#pragma nolint 3  /* this call intentionally passes uninitialized vars */
		fulltrace (Pray, R, Ct, hitdist, Phit, Nhit, Pmiss, Rmiss);
		C += Ct;
		alpha += 1 - step(1.0e10,hitdist);
            }
        }
	uniform float totrays = nrays*nrays;
        C /= totrays;   alpha /= totrays;
    } else {
        /* No blur or curvature, just do a simple trace */
#pragma nolint 2  /* this call intentionally passes uninitialized vars */
	fulltrace (P, Rdir, C, hitdist, Phit, Nhit, Pmiss, Rmiss);
	alpha = 1 - step(1.0e10,hitdist);
    }
    return C;
#else
    return color 0;
#endif
}





#define ENVPARAMS \
        envname, envspace, envrad, rayjitter, raysamples

#define DECLARE_ENVPARAMS                           \
        string envname, envspace;                   \
        uniform float envrad, rayjitter, raysamples

#define DECLARE_DEFAULTED_ENVPARAMS                                 \
        string envname = "", envspace = "world";                    \
        uniform float envrad = 100, rayjitter = 0, raysamples = 1



color
SampleEnvironment (point P;  vector R;  float Kr, blur;  DECLARE_ENVPARAMS;)
{
    color C = 0;
    float alpha;
    if (envname != "") {
	if (envspace == "NDC")
	    C = ReflMap (envname, P, blur, alpha);
	else
	    C = Environment (envname, envspace, envrad, P, R, blur, alpha);
    }
#if (defined(BMRT) || defined(RAYSERVER_H))
    color Cray = RayTrace (P, R, Kr, sqrt(blur), rayjitter, raysamples, alpha);
    C = Cray + (1-alpha) * C;
#endif
    return Kr * C;
}




#endif /* defined(REFLECTIONS_H) */

