#include "k3d_noises.h"
#include "k3d_constants.h"


surface k3d_cyclone(float Ka = 0.5, Kd = 0.75; float max_radius = 1;
		    float twist = 0.5; float scale = .7, offset = .5;
		    float omega = 0.675; float octaves = 4;
  )
{
  float radius, dist, angle, sine, cosine, eye_weight, value;
  point Pt;			/* Point in texture space */
  point PN;			/* Normalized vector in texture space */
  point PP;			/* Point after distortion */
  float l, o, a, i;		/* Loop control for fractal sum */

  /* Transform to texture coordinates */
  Pt = transform("shader", P);

  /* Rotate hit point to "cyclone space" */
  PN = normalize(Pt);
  radius = sqrt(xcomp(PN) * xcomp(PN) + ycomp(PN) * ycomp(PN));

  if(radius < max_radius)
    {				/* inside of cyclone */
      /* invert distance from center */
      dist = pow(max_radius - radius, 3);
      angle = PI + twist * TWOPI * (max_radius - dist) / max_radius;
      sine = sin(angle);
      cosine = cos(angle);
      PP =
	point(xcomp(Pt) * cosine - ycomp(Pt) * sine,
	      xcomp(Pt) * sine + ycomp(Pt) * cosine, zcomp(Pt));
      /* Subtract out "eye" of storm */
      if(radius < 0.05 * max_radius)
	{			/* if in "eye" */
	  eye_weight = (.1 * max_radius - radius) * 10;	/* normalize */
	  /* invert and make nonlinear */
	  eye_weight = pow(1 - eye_weight, 4);
	}
      else
	eye_weight = 1;
    }
  else
    PP = Pt;

  if(eye_weight > 0)
    {				/* if in "storm" area */
      /* Compute VLfBm */
      l = 1;
      o = 1;
      a = 0;
      for(i = 0; i < octaves && o >= VERY_SMALL; i += 1)
	{
	  a += o * VLNoise(PP * l, 1);
	  l *= 2;
	  o *= omega;
	}
      value = abs(eye_weight * (offset + scale * a));
    }
  else
    value = 0;

  /* Thin the density of the clouds */
  Oi = value * Os;

  /* Shade like matte, but with color scaled by cloud opacity */
  Ci =
    Oi * Cs * (Ka * ambient() + Kd * diffuse(faceforward(normalize(N), I)));
}
