/****************************************************************************
 * superplank.sl -- another surface shader for wood planks.
 *
 * Description:
 *   Makes texture of wooden planks in s-t space.  This wood looks rather
 *   like varnished oak planks, with staggered planks, rings and grain,
 *   reflections (either traced or via reflection map), and bumps.
 *   It can make the plank pattern as either straight staggered planks
 *   or in a parquet pattern.
 *
 * Parameters:
 *   Ka, Kd, Ks, specularcolor, roughness - work just like the plastic shader
 *   Kr, eta - reflection amount and index of refraction (for fresnel)
 *   Ktrace, Krefl, reflmap - const for trace, for refl map, filename
 *   txtscale - overall scaling factor for the texture
 *   Km - overall scaling factor for bumpiness.
 *   lightwood - wood-like color from which the various shades are derived.
 *   plankwidth - width of each plank (in terms of s/t)
 *   planklength - length of each plank (in terms of s/t)
 *   groovewidth - width of the grooves between the planks (in terms of s/t)
 *   groovedepth - depth of the groove indentations (in shader space units)
 *   groovecolor - the color of the "grooves" between the planks
 *   plankpattern - plank orientation pattern  1=straight, 2=parquet
 *   plankspertile - for parquet, number of sub-planks
 *   plankstagger - for staggered, how much are the rows of planks staggered
 *   plankvary - controls how much wood color varies from plank to plank
 *   ringscale - scaling for the ring spacing
 *   ringwidth - relative width of the dark ring parts
 *   wavy - relative wavyness of the ring pattern
 *   grainy - relative graininess (0 = no fine grain)
 *   grainscale - scaling for the fine grain
 *   graindepth - depth of grain and ring grooves
 *   varnishbumpfreq - frequency of bumps in the varnish coating
 *   varnishbumpamp - height of bumps in the varnish coating
 *
 * Antialiasing: this shader does a pretty good job of antialiasing itself,
 *   even with low sampling densities.
 *
 * Author: written by Larry Gritz
 *          current contact address: gritzl@acm.org
 *
 * $Revision: 1.1 $     $Date: 2006/02/25 20:11:54 $
 *
 ****************************************************************************/

#include "k3d_noises.h"
#include "k3d_patterns.h"




surface k3d_superplank(		/* Parameters: */
			/* BRDF */
			float Ka = 1, Kd = 1;	/* Overall ambient & diffuse response */
			float Ks = .75, roughness = .02;	/* Spec highlight control */
			color specularcolor = 1;	/* Spec reflection color */
			float Kr = 1, eta = 1.5;	/* Mirror refl & index of refr */
			float Ktrace = 1, Krefl = 0;	/* trace and reflection map */
			string reflmap = "";
			/* Overall texturing control */
			float txtscale = 1;	/* Overall scaling factor */
			float Km = 1;	/* Overall bumpiness factor */
			color lightwood = color(.5, .2, .067);	/* light wood color */
			/* Plank layout */
			float plankpattern = 1;	/* 1=straight, 2=parquet */
			float plankspertile = 4;	/* For parquet */
			float plankwidth = .05;	/* Width of a plank */
			float planklength = .75;	/* Length of a plank */
			float groovewidth = 0.001;	/* Width of the grooves between planks */
			float groovedepth = .0004;	/* Depth of the groove */
			color groovecolor = color(.01, .003, .001);
			float plankstagger = 1;	/* How much should rows be staggered */
			float plankvary = 0.8;	/* Wood color variation plank-to-plank */
			/* Wood appearance */
			float ringscale = 25;	/* Larger makes more, thinner rings */
			float ringwidth = 1;	/* Relative ring width */
			float wavy = 0.08;	/* Larger makes rings more wavy */
			float grainy = 1;	/* Relative graininess */
			float grainscale = 60;	/* Larger makes smaller "grains" */
			float graindepth = 0.0001;	/* Depth of divots where grains are */
			float varnishbumpfreq = 30;	/* Bump freq of the varnish */
			float varnishbumpamp = 0.0003;	/* How big are the varnish bumps? */
  )
{
  /* mapping coordinates */
  float ss, tt;
  float grain_s, grain_t, ring_s, ring_t, plank_s, plank_t;
  /* antialiasing */
  float swidth, twidth, fwidth;
  float sw, tw, overallscale;
  /* Planks & grooves */
  uniform float PGWIDTH, PGHEIGHT, GWF, GHF;
  float whichrow, whichplank;	/* Index for each row & plank within row */
  float w, h;			/* temporaries */
  float groovy;			/* 0 in groove, 1 in woody part */
  /* Wood appearance */
  color Ct, woodcolor;
  float r2;
  float fade, ttt;
  float ring;			/* 1 in a ring darkening, 0 where not */
  float grain;			/* 1 inside a grain bit, 0 elsewhere */
  /* Illumination model */
  float adjustedKs;
  vector IN;			/* normalized I vector */
  normal NN;			/* normalized N for displacing */
  normal Nf;			/* forward facing, normalized normal */
  vector R, T_dummy;		/* Refl (and tummy transmit) from fresnel */
  float fresnelKr, fresnelKt;	/* Fresnel reflection coefficients */
  float shadlen;		/* length of a unit of shader space */
  float disp;			/* accumulate displacement here */
  point Pndc;			/* NDC coordinate of P */
  vector V;
  color Cspec;
  float nonspec;
  float tmp;

  /*
   * 1. Setup.
   *
   * Determine the basic mapping, filter sizes for antialiasing, other
   * values used throughout the shader.
   */

  /* First, determine the basic mapping */
  ss = s * txtscale;
  tt = t * txtscale;
  /* Compute the basic filter size for antialiasing */
  swidth = filterwidth(ss);
  twidth = filterwidth(tt);
  fwidth = max(swidth, twidth);

  /* How much current space corresponds to a unit of s or t?
   * We will use this later to help scale the displacement, this making
   * the bumps scale relative to the overall pattern, rather than being
   * strictly tied to the scale of the object.
   */
  overallscale = (length(Deriv(P, ss)));


  /*
   * 2. Plank pattern.
   *
   * Determine which row and plank we're on, and come up with an
   * antialiased term for whether we're in or out of a groove.
   */
  if(plankpattern == 1)
    {
      /* Straight, staggered planks */
      PGWIDTH = plankwidth + groovewidth;
      PGHEIGHT = planklength + groovewidth;
      plank_s = ss / PGWIDTH;
      whichrow = floor(plank_s);
      /* Jiggle each row */
      plank_t = tt / PGHEIGHT + 20 * plankstagger * cellnoise(whichrow);
      whichplank = floor(plank_t);
    }
  else
    {
      /* Parquet pattern */
      PGWIDTH = plankwidth + groovewidth;
      PGHEIGHT = PGWIDTH * plankspertile;
      plank_s = ss / PGWIDTH;
      whichrow = floor(plank_s);
      plank_t = tt / PGHEIGHT;
      whichplank = floor(plank_t);
      if(mod((whichrow / plankspertile) + whichplank, 2) >= 1)
	{
	  plank_s = tt / PGWIDTH;
	  plank_t = ss / PGHEIGHT;
	  whichrow = floor(plank_s);
	  whichplank = floor(plank_t);
	  tmp = ss;
	  ss = tt;
	  tt = tmp;
	  tmp = swidth;
	  swidth = twidth;
	  twidth = tmp;
	}
    }
  /* Now whichplank is a unique integer index for each plank */

  /* Figure out where the grooves are.  The value groovy is 0 where there
   * are grooves, 1 where the wood grain is visible.  Do some simple
   * antialiasing by trying to box filter the edges of the grooves.
   */

  /* compute half width & length of groove as fraction of plank size */
  GWF = groovewidth * 0.5 / PGWIDTH;
  GHF = groovewidth * 0.5 / PGHEIGHT;
  sw = swidth / PGWIDTH;
  tw = twidth / PGHEIGHT;
  if(sw >= 1)
    w = 1 - 2 * GWF;		/* Filter width is wider than the plank itself */
  else
    w = filteredpulse(whichrow + GWF, whichrow + 1 - GWF, plank_s, sw);
  if(tw >= 1)
    h = 1 - 2 * GHF;		/* Filter width is longer than the plank itself */
  else
    h = filteredpulse(whichplank + GHF, whichplank + 1 - GHF, plank_t, tw);
  groovy = w * h;

  /*
   * 3. Ring and grain patterns, color and specularity adjustment.
   *
   * The wood has rings at one scale, grain at a finer scale.  They
   * interact subtly.
   */

  /* Compute the filter width first, so we don't bother with more if
   * the rings are too small to see.
   */
  fwidth = max(swidth * ringscale, twidth * ringscale);
  fade = smoothstep(.75, 4, fwidth);
  if(fade < 0.999)
    {
      ring_s = ss * ringscale;
      ring_t = tt * ringscale;
      ttt = ring_t + whichplank * 28.38 + wavy * noise(8 * ring_s, ring_t);
      ring = ringscale * noise(ring_s - whichplank * 4.18, ttt / 20);
      ring -= floor(ring);
      ring =
	0.3 + 0.7 * smoothstep(.55 - .35 * ringwidth, 0.55,
			       ring) * (1 - smoothstep(0.75, 0.8, ring));
      ring = (1 - fade) * ring + 0.65 * fade;

      /* Grain pattern */
      fwidth = max(swidth * grainscale, twidth * grainscale);
      fade = smoothstep(.75, 4, fwidth);
      if(fade < 0.999)
	{
	  grain_s = ss * grainscale;
	  grain_t = tt * grainscale;
	  r2 = 1.3 - noise(12 * grain_s, grain_t);
	  r2 = grainy * r2 * r2 + (1 - grainy);
	  grain = (1 - fade) * r2 + (0.75 * fade);
	}
      else
	grain = 0.75;
    }
  else
    {
      ring = 0.4875;
      grain = 0.75;
    }
  grain *= (.85 + .15 * ring);

  /* Start with the light wood color */
  woodcolor = lightwood;
  /* Add some plank-to-plank variation in overall color */
  woodcolor *=
    1 - plankvary / 2 + plankvary * float cellnoise(whichplank, whichrow);
  /* Darken the wood according to the ring and grain patterns */
  woodcolor *= (1 - 0.25 * ring) * (1 - .5 * grain);
  /* Combine the rings, grain, plank variation into one surface color */
  Ct = mix(groovecolor, woodcolor, groovy);
  /* Less specular in the grooves, more specular in the dark wood. */
  adjustedKs = Ks * (1 + .2 * ring) * (1 + .3 * grain) * groovy;


  /*
   * 4. Bumps
   *
   * We do some bump mapping to make the grooves and grain depressed,
   * and add some general lumpiness to the varnish layer.
   */
  if(Km > 0)
    {
      /* Grooves */
      disp = -groovedepth * (1 - groovy);
      /* Random bumps on the varnish */
      if(varnishbumpamp > 0)
	disp +=
	  varnishbumpamp * (2 *
			    noise(varnishbumpfreq * ss,
				  varnishbumpfreq * tt) - 1);
      /* Depressions due to grain & rings */
      disp -= graindepth * (.75 * grain + ring);
      /* Find out how long a unit of shader space is, in current units */
      NN = normalize(N);
      shadlen = overallscale / length(ntransform("shader", NN));

      /* Recalculate N */
      N = calculatenormal(P + (Km * shadlen * disp) * NN);
    }

  /*
   * 5. BRDF
   *
   * The wood itself behaves like plastic, the varnish overtop reflects
   * using the fresnel formula (grazing angles reflect like mirrors).
   * Have some subtle interaction between grain and specularity.
   */

  IN = normalize(I);
  V = -IN;
  Nf = faceforward(normalize(N), I);
  fresnel(IN, Nf, 1 / eta, fresnelKr, fresnelKt, R, T_dummy);
  fresnelKr *= Kr;
  Cspec = 0;
  illuminance(P, Nf, PI / 2)
  {
    nonspec = 0;
    lightsource("__nonspecular", nonspec);
    if(nonspec < 1)
      {
	vector H = normalize(normalize(L) + V);
	Cspec +=
	  Cl * ((1 - nonspec) *
		smoothstep(.6, .85, pow(max(0, Nf.H), 8 / roughness)));
      }
  }
  Ci =
    (1 - fresnelKr) * (Ct * (Ka * ambient() + Kd * diffuse(Nf))) +
    adjustedKs * (specularcolor * Cspec);

  /* Only compute reflections if Kr > 0 */
  if(fresnelKr > 0.001)
    {
      if(Krefl > 0 && reflmap != "")
	{
	  Pndc = transform("NDC", P);
	  Ci +=
	    fresnelKr * specularcolor * texture(reflmap, xcomp(Pndc),
						1 - ycomp(Pndc));
	}
      if(Ktrace > 0)
	Ci += Ktrace * fresnelKr * specularcolor * trace(P, R);
    }

  /* Scale by opacity */
  Oi = Os;
  Ci *= Oi;
}
