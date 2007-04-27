/****************************************************************************
 * lensflare.sl
 *
 * Description: This shader, when placed on a piece of geometry 
 *   immediately in front of the camera, simulates lens flare.
 *   These effects happen in real cameras when the camera points toward
 *   a bright light source, resulting in interreflections within the
 *   optical elements of the lens system itself.  Real lens flare is
 *   pretty plain looking and uninteresting; this shader takes some
 *   liberties but looks pretty good.
 *   
 * Parameters:
 *   intensity - overall scale of intensity of all lens flare effects
 *   bloomintensity - overall intensity of the "bloom" effect.  Setting
 *          this to 0 removes the bloom effect altogether.
 *   bloomradius, bloomfalloff - control the size & shape of the bloom
 *   bloomstarry, bloomnpoints - control the "starry" appearance of the 
 *          bloom effect (bloomstarry=0 means perfectly round bloom)
 *   starburstintensity - overall intensity of starburst effect (0=none)
 *   starburstradius, starburstnpoints, starburstfalloff - control the
 *          size and shape of the starburst effect
 *   rainbowintensity - intensity of rainbow effect (0=none)
 *   rainbowradius, rainbowwidth - size of the rainbow
 *   nspots - number of "spots" splayed out on the axis joining the
 *          image center with the light position
 *   disky, ringy, blotty, bloony - give the relative proportions of
 *          the 4 different kinds of spots.
 *   spotintensity - overall intensity scale for the spots
 *   spotvarycolor - scale the color variation of the spots
 *   seed - random number seed for many of the computations
 *
 * WARNING: lens flare is notorious as a sign of cheesy, cheap computer
 *   graphics.  Use this effect with extreme care!  
 *
 ***************************************************************************
 *
 * Author: Larry Gritz & Tony Apodaca, 1999
 *
 * Contacts:  lg@pixar.com
 *
 * $Revision: 1.1 $    $Date: 2006/02/25 20:11:54 $
 *
 ****************************************************************************/

#include "k3d_patterns.h"


/* Helper function: compute the aspect ratio of the frame */
float
aspectratio()
{
  uniform point Pcorner0 = transform("NDC", "screen", point(0, 0, 0));
  uniform point Pcorner1 = transform("NDC", "screen", point(1, 1, 0));
  return (xcomp(Pcorner1) - xcomp(Pcorner0)) / (ycomp(Pcorner1) -
						ycomp(Pcorner0));
}


/* Helper function: compute the camera's diagonal field of view */
float
cameradiagfov()
{
  uniform vector corner = vector(transform("NDC", "camera", point(1, 1, 0)));
  uniform float halfangle = acos(normalize(corner).vector(0, 0, 1));
  return 2 * halfangle;
}


color
rainbow(float x, dx)
{
#define R	color(1,0,0)
#define O	color(1,.5,0)
#define Y	color(1,1,0)
#define G	color(0,1,0)
#define B	color(0,0,1)
#define Ii	color(.375,0,0.75)
#define V	color(0.5,0,0.5)
  return filteredpulse(0, 1, x, dx) * spline(x, V, V, Ii, B, G, Y, O, R, R);
}



surface k3d_lensflare(float intensity = 1.0;
		      float bloomintensity = 1;
		      float bloomradius = 0.5;
		      float bloomstarry = 0.75;
		      float bloomnpoints = 25;
		      float bloomfalloff = 8;
		      float starburstintensity = 0.075;
		      float starburstradius = 0.5;
		      float starburstnpoints = 100;
		      float starburstfalloff = 3;
		      float rainbowintensity = 0.03;
		      float rainbowradius = 0.5; float rainbowwidth = 0.2;
		      float nspots = 50; float disky = 3; float ringy = 1;
		      float blotty = 1;
		      float bloony = 1; float spotintensity = 0.08;
		      float spotvarycolor = 0.5; float seed = 143;
  )
{
  uniform float nrand = 0;
  uniform float urand()
  {
    extern uniform float nrand, seed;
    nrand += 1;
    return cellnoise(nrand, seed);
  }


  Ci = 0;
  Oi = 0;

  uniform float aspect = abs(aspectratio());
  uniform float lensfov = cameradiagfov();

  point Pndc = (transform("NDC", P) - vector(.5, .5, 0)) * 2;
  Pndc *= vector(aspect, 1, 0);
  float dPndc = filterwidthp(Pndc);

  illuminance(P, vector "camera"(0, 0, 1), PI / 2)
  {
    float atten = acos(zcomp(normalize(vector transform("camera", P + L))));
    atten = 1 - smoothstep(1, 2, abs(atten) / (lensfov / 2));

    float brightness =
      atten * intensity * (comp(Cl, 0) + comp(Cl, 1) + comp(Cl, 2)) / 3;

    color Cflare = 0;

    nrand = 0;
    point Plight = (transform("NDC", P + L) - vector(.5, .5, 0)) * 2;
    Plight *= vector(aspect, 1, 0);

    vector Lvec = Plight - Pndc;
    float angle = atan(ycomp(Lvec), xcomp(Lvec)) + PI;

    /*
     * Handle the image of the lamp.  There are 3 effects:
     * the bloom, a small red ring flare, and the triple starburst.
     */
    float dist = length(Lvec);

    if(bloomintensity > 0)
      {
	float radius = sqrt(brightness) * 5 * mix(.2, bloomradius, urand());
	float bloom = pnoise(bloomnpoints * angle / (2 * PI), bloomnpoints);
	bloom = mix(0.5, bloom, bloomstarry);
	bloom = mix(1, bloom, smoothstep(0, 0.5, dist / radius));
	bloom = pow(1 - smoothstep(0.0, radius * bloom, dist), bloomfalloff);
	Cflare += bloom * (bloomintensity / intensity) / brightness;
      }

    /* Starburst */
    if(starburstintensity > 0)
      {
	float radius =
	  sqrt(brightness) * 5 * mix(.2, starburstradius, urand());
	float star = float pnoise(starburstnpoints * angle / (2 * PI),
				  starburstnpoints);
	star =
	  pow(1 - smoothstep(0.0, radius * star, dist), starburstfalloff);
	Cflare += star * (starburstintensity / intensity) / brightness;
      }

    /* Rainbow */
    if(rainbowintensity > 0)
      {
	Cflare +=
	  brightness * (rainbowintensity / intensity) *
	  rainbow((dist / rainbowradius - 1) / rainbowwidth,
		  (dPndc / rainbowradius) / rainbowwidth);
      }

    /*
     * Now emit the random rings themselves
     */
    vector axis = normalize(vector Plight);
    uniform float i;
    nrand = 20;			/* Reset on purpose! */
    for(i = 0; i < nspots; i += 1)
      {
	uniform float alongaxis = urand();
	point cntr = point(mix(-1.7, 1.7, alongaxis) * axis);
	float axisdist = distance(cntr, Pndc);
	float radius = mix(0.04, .1,
			   pow(urand(), 2)) * distance(cntr, Plight);
	color clr = Cl;
	clr *= 1 + spotvarycolor * (color cellnoise(i) - 0.5);
	float bright = 1 - (2 * radius);
	bright *= bright;

	uniform float alltypes = (disky + ringy + blotty + bloony);
	uniform float type = urand() * alltypes;


	float int = 0;
	if(type < disky)
	  {			/* Flat disk */
	    int = 1 - filterstep(radius, axisdist - dPndc / 2,
				 axisdist + dPndc / 2);
	  }
	else if(type < (disky + ringy))
	  {			/* Ring */
	    int = filteredpulse(radius, radius + 0.05 * axisdist,
				axisdist, dPndc);
	  }
	else if(type < (disky + ringy + blotty))
	  {			/* Soft spot */
	    int = 1 - smoothstep(0, radius, axisdist);
	  }
	else
	  {			/* Spot with soft hole in middle */
	    int = smoothstep(0, radius, axisdist) - filterstep(radius,
							       axisdist -
							       dPndc / 2,
							       axisdist +
							       dPndc / 2);
	  }
	Cflare += spotintensity * bright * clr * Cs * int;
      }

    Ci += Cflare * Cl * atten;
  }

  Ci *= intensity;
}
