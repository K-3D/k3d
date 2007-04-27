/* I took wave's lead and renamed wallpaper to DPWallpaper.sl -- tal@SpamSucks_cs.caltech.edu */

/* 
 * wallpaper.sl
 *
 * AUTHOR: Darwyn Peachy
 *
 * REFERENCES:
 *    _Texturing and Modeling: A Procedural Approach_, by David S. Ebert, ed.,
 *    F. Kenton Musgrave, Darwyn Peachey, Ken Perlin, and Steven Worley.
 *    Academic Press, 1994.  ISBN 0-12-228760-6.
 */
 #include "k3d_noises.h"
 
#define NCELLS 10
#define CELLSIZE (1/NCELLS)

surface
k3d_wallpaper(
    uniform float Ka = 1;
    uniform float Kd = 1;
    uniform color starcolor = color (1.0000,0.5161,0.0000);
    uniform float npoints = 5;
     )
{
    color Ct;
    point Nf;
    float ss, tt, angle, r, a, in_out;
    float sctr, tctr, scell, tcell;
    float scellctr, tcellctr;
    float i, j;
    uniform float rmin = 0.01, rmax = 0.03;
    uniform float starangle = 2*PI/npoints;
    uniform point p0 = rmax*(cos(0),sin(0),0);
    uniform point p1 = rmin*
        (cos(starangle/2),sin(starangle/2),0);
    uniform point d0 = p1 - p0;
    point d1;

    scellctr = floor(s*NCELLS);
    tcellctr = floor(t*NCELLS);
    in_out = 0;

    for (i = -1; i <= 1; i += 1) {
        for (j = -1; j <= 1; j += 1) {
	    scell = scellctr + i;
	    tcell = tcellctr + j;
	    if (float noise(3*scell-9.5,7*tcell+7.5) < 0.55) {
                sctr = CELLSIZE * (scell + 0.5
                     + 0.6 * snoisexy(scell+0.5, tcell+0.5));
                tctr = CELLSIZE * (tcell + 0.5
                     + 0.6 * snoisexy(scell+3.5, tcell+8.5));
                ss = s - sctr;
                tt = t - tctr;
            
                angle = atan(ss, tt) + PI;
                r = sqrt(ss*ss + tt*tt);
                a = mod(angle, starangle)/starangle;
            
                if (a >= 0.5)
                    a = 1 - a;
                d1 = r*(cos(a), sin(a),0) - p0;
                in_out += step(0, zcomp(d0^d1));
            }
        }
    }
    Ct = mix(Cs, starcolor, step(0.5,in_out));

    /* "matte" reflection model */
    Nf = normalize(faceforward(N, I));
    Oi = Os;
    Ci = Os * Ct * (Ka * ambient() + Kd * diffuse(Nf));
}
