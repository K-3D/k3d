/*  IDbrushedmetal.sl written 9/99 by Ivan DeWolf
 *  ivan@SpamSucks_martian-tools.com
 *  feel free to copy, distribute, hack and/or abuse this code 
 *  in any way you see fit, but please leave my name near the top
 *
 *  yet another brushedmetal. This one computes a specular 
 *  highlight, and also can use a reflection map.
 *  stochasticly spreads the map, so it looks a little grainy.
 *  based somewhat on LGbrushedmetal.sl
 *
 *  if you set Kr = 0 and specwidth = 10
 *  then assume specspread is roughness, then you have something 
 *  very similar to plastic.sl...
 *
 *  PARAMETERS:
 *  Ks			- coefficient of specular
 *  Kd			- coefficient of diffuse
 *  Ka			- coefficient of ambient
 *  Kr			- coefficient of reflectivity
 *  specwidth		- the width of the specular stripe
 * 			  maxes out at 10
 *  specspread		- the spread of the specular stripe
 *			  maxes out at .5
 *  mapspread		- the spread of the image streaking
 *			  maxes out at .5
 *  twist		- allows you to twist the direction of anisotropy
 *			  angle in radians (i.e. PI*.5 = 90 degrees)
 *  mapname		- name of the environment map
 *  specularcolor	- color of the specular hilight
 */

color
anisospecular (vector VA; float specspread; float specwidth)
{

    extern vector I;
    extern normal N;
    extern point P;

    float nonspec;
    vector V = normalize(-I);
    normal NN = normalize(N);
    normal Nf = faceforward(NN,-V);
    color pixbrdf, C = 0;
    point zro = 0;

    illuminance (P, Nf, PI*.5) {
        extern vector L;  
	extern color Cl;

        nonspec = 0;
        lightsource ("__nonspecular", nonspec);
        if (nonspec < 1) {
            vector LN = normalize (L);
	    vector H = normalize (V + LN);

	    pixbrdf  = specularbrdf(LN, Nf,V,specspread);
            C += Cl * pixbrdf * pow( 1-abs(VA.H), 1/specwidth );
        }
    }
    return C;
}

surface
k3d_brushedmetal2(
	float	Ks		=  1,
		Kd		= .01,
		Ka		= .001,
		Kr		= .6,
		specwidth	= .5,
		specspread 	= .5,
		mapspread 	= .3,
		twist		=  0;
	string	mapname 	= "";
	color	specularcolor	=  1)
{
	point Po = transform("object",P);
	color ev = 0;
	vector D, V= normalize(-I);
	vector Nf, Ntmp;
        point zro = 0;
	float i, numsamples = 20;
	float angle, jitter;
	float Jspread = PI*(1/numsamples)*mapspread;
	vector VA = rotate(normalize(dPdu), twist, zro, normalize(N));
	

	Nf = faceforward(normalize(N), -I);
        Ntmp = Nf;

	if( mapname != "" ) {
	    for(i=0;i<=numsamples;i=i+1){
	      jitter = (random()-.5)*Jspread;
	      angle = PI*((i/numsamples)-.5)*mapspread;
	      Ntmp = rotate(Nf,angle+jitter,zro,VA);
	      D = reflect(-V, Ntmp);
	      D = vtransform("world", D);
	      ev += environment(mapname, D)*(.5-abs((i/numsamples)-.5))*.25;
	    }
	} else
	    ev = 0;

	Oi = Os;
	Ci = Oi * (Cs * (Ka * ambient() + Kd * diffuse(-Nf)) + 
	specularcolor * Ks * anisospecular(VA, specspread*2, specwidth*.1)) +
	ev * Kr;
}














