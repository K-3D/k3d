/*  tooledsteel.sl written 9/99 by Ivan DeWolf
 *  ivan@SpamSucks_martian-tools.com
 *  feel free to copy, distribute, hack and/or abuse this code 
 *  in any way you see fit, but please leave my name near the top
 *
 *  a combination of IDbrushedmetal.sl and the Worley-esque
 *  F1 from bubbly.sl adapted to happen in parameter space.
 *  A vector is computed from the gradient of the distance function,
 *  and then used as the vector of anisotropy. works best on parametric
 *  surfaces where you don't care about the seams. 
 *  (like that'll ever happen...)
 *  thanks to Jos Stam whose animated cube motivated this thing.
 *
 *
 *  PARAMETERS:
 *  mult    		- multiples of swirls per unit
 *  Nzscale 		- scale of the noise that randomizes the location of 
 *          		  the swirls. set this to zero for a perfect grid.
 *  Ks			- coefficient of specular
 *  Kd			- coefficient of diffuse
 *  Ka			- coefficient of ambient
 *  Kr			- coefficient of reflectivity
 *  specwidth		- the width of the specular stripe
 * 			  maxes out at 10
 *  specspread		- the spread of the specular stripe
 *			  maxes out at .5
 *  mapspread		- the spread of the reflection map streaking
 *			  maxes out at .5
 *  twist		- allows you to twist the direction of anisotropy
 *			  angle in radians (i.e. PI*.5 = 90 degrees)
 *  mapname		- name of the environment map
 *  specularcolor	- color of the specular hilight
 */

vector
getvec(float  mult; float Nzscale)
{
  extern point P;
  extern vector dPdu;
  extern vector dPdv;
  extern float s;
  extern float t;
  vector udir = vector(1,0,0);
  vector vdir = vector(0,1,0);

  float a,b,c;
  float dist, shortest=10000;
  float valu,valv;
  point Po = point(s*mult,t*mult,0);
  point Pou = Po +(udir*.01);
  point Pov = Po +(vdir*.01);

  /*true cell center, surrounding cell centers, noised cell center*/
  point trucell, surrcell, nzcell;
  vector offset, nzoff, out;
  float chu, chv;
  
  float ncells = floor(mult);
  float cellsize = 1/ncells;
  
  setxcomp(trucell,floor(xcomp(Po))+.5);
  setycomp(trucell,floor(ycomp(Po))+.5);
  setzcomp(trucell,0);
  c = 0;
			     
  /*what is the shortest distance to a noised cell center?*/
  for(a = -1; a<= 1; a+=1){
    for(b = -1; b<=1; b += 1){
	offset = vector(a,b,c);
	surrcell = trucell+offset;
	nzoff = ((vector cellnoise(surrcell)-.5)*Nzscale);
	setzcomp(nzoff,0);
	nzcell = surrcell + nzoff;
	dist = distance(Po,nzcell);
	if(dist<shortest){
	  shortest = dist;
	  valu = distance(Pou,nzcell);
	  valv = distance(Pov,nzcell);
	}
    }
  }
  chu = valu - shortest;
  chv = valv - shortest;
  out  = normalize((udir*chu)+(vdir*chv));
  return out;
}

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
k3d_tooledsteel(
	float	mult		=  3,
		Nzscale		=  1.5,
		Ks		=  1,
		Kd		= .01,
		Ka		= .001,
		Kr		= .6,
		specwidth	= .5,
		specspread 	= .5,
		mapspread 	= .2,
		twist		=  0;
	string	mapname 	= "";
	color	specularcolor	=  1)
{
        vector VA = getvec(mult,Nzscale);
	point Po = transform("object",P);
	color ev = 0;
	vector D, V= normalize(-I);
	vector Nf, Ntmp;
        point zro = 0;
	float i, numsamples = 20;
	float angle, jitter;
	float Jspread = PI*(1/numsamples)*mapspread;

	VA = rotate(VA, twist, zro, normalize(N));
	

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
