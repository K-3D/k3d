/*
 * outlet.sl -- surface shader for an electrical outlet
 *
 * DESCRIPTION:
 *   Makes an electrical outlet.  Perfect for slapping on a bilinear
 *   patch to make a wall more interesting.  The patch should be
 *   twice as high as it is wide.  *Totally* cheesy if you see it from
 *   up close, but from across the room, it's a nice touch of detail.
 *   I should probably add some bump mapping or other variation, but
 *   I'm lazy, okay?
 * 
 * PARAMETERS:
 *   Ka, Kd 	   	work just like the matte shader
 *   screwKs            Ks parameter for the screw
 *
 *
 * AUTHOR: written by Larry Gritz
 *
 * HISTORY:
 *      June 1992 -- first written by lg for the Kitchen image
 *      17 Jan 1994 -- recoded by lg in correct shading language.
 *
 * last modified  17 Jan 1994 by Larry Gritz
 */



surface
k3d_outlet ( float Ka = 1, Kd = 1, Km = 0.1;
	 float screwKs = 0.5; )
{
  color Ct;
  float x, y;
  float ks, kd, roughness;
  point Nf;
  
  /* defaults */
  kd = Kd;
  ks = 0;
  roughness = 0.25;
  Ct = Cs;

  x = abs (u - 0.5);
  y = 2 * v;

  if ((x*x + (y-1)*(y-1)) < 0.0025) {
      Ct = Cs * 0.5;
      ks = screwKs;
      kd = 0;
    }
  else if (x > 0.08  &&  x < 0.14  &&
	   ((y > 0.42 && y < 0.62) || (y > 1.22 && y < 1.42)))
    {
      Ct = 0.1 * Cs;
    }
  else if (x < 0.25  &&  ((y > 0.35 && y < 0.85) || (y > 1.15 && y < 1.65))) {
      if (((x*x + (y-0.75)*(y-0.75)) < 0.0025) ||
	  ((x*x + (y-1.55)*(y-1.55)) < 0.0025)) {
	  Ct = 0.1 * Cs;
	}
      else {
	  Ct = 0.75 * Cs;
	}
    }


  /* Use the plastic illumination model */
  Nf = faceforward (normalize(N),I);
  Oi = Os;
  Ci = Os * ( Ct * (Ka*ambient() + kd*diffuse(Nf)) +
	      ks * specular(Nf,-normalize(I),roughness));

}
