#include "k3d_noises.h"

surface k3d_imagelayerclouds(float txtscale = 1;
			     color skycolor = color(.15, .15, .6);
			     color cloudcolor = color(1, 1, 1);
			     float octaves = 8, omega = 0.5, lambda = 2;
			     float threshold = 0.5;)
{
  float value;
  color Ct;			/* Color of the surface */
  point PP;			/* Surface point in shader space */
  float i, a, l, o;

//  PP = txtscale * transform ("shader", P);
  PP = txtscale * point(s, t, s);

  /* Use fractional Brownian motion to compute a value for this point */
/*  value = fBm (PP, omega, lambda, octaves); */
  value = 0;
  l = 1;
  o = 1;
  a = 0;
  for(i = 0; i < octaves; i += 1)
    {
      a += o * snoise(PP * l);
      l *= 2;
      o *= omega;
    }
  value = a;

  Ct = mix(skycolor, cloudcolor, smoothstep(threshold, 1, value));

  /* Shade like matte, but use color Ct */
  Oi = 1;			/* Make it opaque */
  Ci = Ct;			/* This makes the color disregard the lighting */
}
