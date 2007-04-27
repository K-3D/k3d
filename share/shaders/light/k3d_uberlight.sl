/****************************************************************************
 * uberlight.sl - a light with many fun controls.
 *
 * Description:
 *   Based on Ronen Barzel's paper "Lighting Controls for Computer
 *   Cinematography" (in Journal of Graphics Tools, vol. 2, no. 1: 1-20).
 *
 * Rather than explicitly pass "from" and "to" points to indicate the
 * position and direction of the light (as spotlight does), this light
 * emits from the origin of the local light shader space and points
 * toward the +z axis (also in shader space).  Thus, to position and
 * orient the light source, you must translate and rotate the
 * coordinate system in effect when the light source is declared.
 * Perhaps this is a new idea for some users, but it isn't really
 * hard, and it vastly simplifies the math in the shader.
 *
 * Basic color/brightness controls:
 *   intensity - overall intensity scaling of the light
 *   lightcolor - overall color filtering for the light
 *
 * Light type:
 *   lighttype - one of "spot", "omni", or "arealight".  Spot lights are
 *       those that point in a particular direction (+z in local light
 *       space, for this light).  Omni lights throw light in all directions.
 *       Area lights are emitted from actual geometry (this only works on
 *       BMRT area lights for the time being).
 *
 * Distance shaping and falloff controls:
 *   cuton, cutoff - define the depth range (z range from the origin, in
 *       light coordinates) over which the light is active.  Outside
 *       this range, no energy is transmitted.
 *   nearedge, faredge - define the width of the transition regions
 *       for the cuton and cutoff.  The transitions will be smooth.
 *   falloff - defines the exponent for falloff.  A falloff of 0 (the
 *       default) indicates that the light is the same brightness
 *       regardless of distance from the source.  Falloff==1 indicates
 *       linear (1/r) falloff, falloff==2 indicates 1/r^2 falloff
 *       (which is physically correct for point-like sources, but
 *       sometimes hard to use).
 *   falloffdist - the distance at which the incident energy is actually
 *       equal to intensity*lightcolor.  In other words, the intensity
 *       is actually given by:   I = (falloffdist / distance) ^ falloff
 *   maxintensity - to prevent the light from becoming unboundedly
 *       large when the distance < falloffdist, the intensity is
 *       smoothly clamped to this maximum value.
 *   parallelrays - when 0 (the default), the light appears to emanate
 *       from a single point (i.e., the rays diverge).  When nonzero, 
 *       the light rays are parallel, as if from an infinitely distant
 *       source (like the sun).
 *
 * Shaping of the cross-section.  The cross-section of the light cone
 * is actually described by a superellipse with the following
 * controls:
 *   shearx, sheary - define the amount of shear applied to the light
 *       cone direction.  Default is 0, meaning that the center of the
 *       light cone is aligned with the z-axis in local light space.
 *   width, height - define the dimensions of the "barn door" opening.
 *       They are the cross-sectional dimensions at a distance of 1
 *       from the light.  In other words, width==height==1 indicates a
 *       90 degree cone angle for the light.
 *   wedge, hedge - the amount of width and height edge fuzz,
 *       respectively.  Values of 0 will make a sharp cutoff, larger
 *       values (up to 1) will make the edge softer.
 *   roundness - controls how rounded the corners of the superellipse
 *       are.  If this value is 0, the cross-section will be a perfect
 *       rectangle.  If the value is 1, the cross-section will be a
 *       perfect ellipse.  In-between values control the roundness of
 *       the corners in a fairly obvious way.
 *   beamdistribution - controls intensity falloff due to angle.
 *       A value of 0 (the default) means no angle falloff.  A value
 *       of 1 is roughly physically correct for a spotlight and 
 *       corresponds to a cosine falloff.  For a BMRT area light, the
 *       cosine falloff happens automatically, so 0 is the right physical
 *       value to use.  In either case, you may use larger values to
 *       make the spot more bright in the center than the outskirts.
 *       This parameter has no effect for omni lights.
 *
 * Cookie or slide filter:
 *   slidename - if a filename is supplied, a texture lookup will be
 *       done and the light emitted from the source will be filtered
 *       by that color, much like a slide projector.  If you want to
 *       make a texture map that simply blocks light, just make it
 *       black-and-white, but store it as an RGB texture.  For
 *       simplicity, the shader assumes that the texture file will
 *       have at least three channels.
 *
 * Projected noise on the light:
 *   noiseamp - amplitude of the noise.  A value of 0 (the default) 
 *       means not to use noise.  Larger values increase the blotchiness
 *       of the projected noise.
 *   noisefreq - frequency of the noise.
 *   noiseoffset - spatial offset of the noise.  This can be animated,
 *       for example, you can use the noise to simulate the
 *       attenuation of light as it passes through a window with 
 *       water drops dripping down it.
 * 
 * Shadow mapped shadows.  For PRMan (and perhaps other renderers),
 * shadows are mainly computed by shadow maps.  Please consult the
 * PRMan documentation for more information on the meanings of these
 * parameters.
 *   shadowmap - the name of the texture containing the shadow map.  If
 *       this value is "" (the default), no shadow map will be used.
 *   shadowblur - how soft to make the shadow edge, expressed as a
 *       percentage of the width of the entire shadow map.
 *   shadowbias - the amount of shadow bias to add to the lookup.
 *   shadownsamps - the number of samples to use.
 *
 * Ray-traced shadows.  These options work only for BMRT:
 *   raytraceshadow - if nonzero, cast rays to see if we are in shadow.
 *       The default is zero, i.e., not to try raytracing.
 *   nshadowrays - The number of rays to trace to determine shadowing.
 *   shadowcheat - add this offset to the light source position.  This
 *       allows you to cause the shadows to emanate as if the light
 *       were someplace else, but without changing the area
 *       illuminated or the appearance of highlights, etc.
 *
 * "Fake" shadows from a blocker object.  A blocker is a superellipse
 * in 3-space which effectively blocks light.  But it's not really
 * geometry, the shader just does the intersection with the
 * superellipse.  The blocker is defined to lie on the x-y plane of
 * its own coordinate system (which obviously needs to be defined in
 * the RIB file using the CoordinateSystem command).
 *   blockercoords - the name of the coordinate system that defines the
 *       local coordinates of the blocker.  If this is "", it indicates 
 *       that the shader should not use a blocker at all.
 *   blockerwidth, blockerheight - define the dimensions of the blocker's
 *       superellipse shape.
 *   blockerwedge, blockerhedge - define the fuzzyness of the edges.
 *   blockerround - how round the corners of the blocker are (same
 *       control as the "roundness" parameter that affects the light
 *       cone shape.
 *
 * Joint shadow controls:
 *   shadowcolor - Shadows (i.e., those regions with "occlusion" as
 *       defined by any or all of the shadow map, ray cast, or
 *       blocker) don't actually have to block light.  In fact, in
 *       this shader, shadowed regions actually just change the color
 *       of the light to "shadowcolor".  If this color is set to
 *       (0,0,0), it effectively blocks all light.  But if you set it
 *       to, say (.25,.25,.25), it will make the shadowed regions lose
 *       their full brightness but not go completely dark.  Another
 *       use is if you are simulating sunlight: set the lightcolor to
 *       something yellowish and make the shadowcolor dark but
 *       somewhat bluish.  Another effect of shadows is to set the
 *       __nonspecular flag so that the shadowed regions are lit only
 *       diffusely, without highlights.
 * 
 * Other controls:
 *   nonspecular - when set to 1, this light does not create
 *       specular highlights!  The default is 0, which means it makes
 *       highlights just fine (except for regions in shadows, as
 *       explained above).  This is very handy for lights that are
 *       meant to be fill lights, rather than key lights.
 *       NOTE: This depends on the surface shader looking for, and
 *       correctly acting upon, this parameter.  The built-in functions
 *       diffuse(), specular() and phong() all do this, for PRMan 3.5
 *       and later, as well as BMRT 2.3.5 and later.  But if you write
 *       your own illuminance loops in your surface shader, you've got
 *       to account for it yourself.  The PRMan user manual explains how
 *       to do this.
 *   __nondiffuse - the analog to nonspecular; if this flag is set to
 *       1, this light will only cast specular highlights but not
 *       diffuse light.  This is useful for making a light that only
 *       makes specular highlights, without affecting the rest of the
 *       illumination in the scene.  All the same caveats apply with
 *       respect to the surface shader, as described above for
 *       __nonspecular.
 *   __foglight - the "noisysmoke" shader distributed with BMRT will add
 *       atmospheric scattering only for those lights that have this
 *       parameter set to 1 (the default).  In other words, if you use
 *       this light with noisysmoke, you can set this flag to 0 to
 *       make a particular light *not* cause illumination in the fog.
 *       Note that the noisysmoke shader is distributed with BMRT but
 *       will also work just fine with PRMan (3.7 or later).
 *
 * NOTE: this shader has one each of: blocker, shadow map, slide, and
 * noise texture.  Some advanced users may want more than one of some or
 * all of these.  It is left as an exercise for the reader to make such
 * extensions to the shader.
 *
 ***************************************************************************
 *
 * This shader was written as part of the course notes for ACM
 * SIGGRAPH '98, course 11, "Advanced RenderMan: Beyond the Companion"
 * (co-chaired by Tony Apodaca and Larry Gritz).  Feel free to use and
 * distribute the source code of this shader, but please leave the
 * original attribution and all comments.
 *
 * This shader was tested using Pixar's PhotoRealistic RenderMan 3.7
 * and the Blue Moon Rendering Tools (BMRT) release 2.3.6.  I have
 * tried to avoid Shading Language constructs which wouldn't work on
 * older versions of these renderers, but I do make liberal use of the
 * "vector" type and I often declare variables where they are used,
 * rather than only at the beginning of blocks.  If you are using a
 * renderer which does not support these new language features, just
 * substitute "point" for all occurrances of "vector", and move the
 * variable declarations to the top of the shader.
 *
 * Author: coded by Larry Gritz, 1998
 *         based on paper by Ronen Barzel, 1997
 *
 * Contacts:  {lg|ronen}@pixar.com
 *
 *
 * $Revision: 1.2 $    $Date: 2006/03/08 16:01:22 $
 *
 ****************************************************************************/




/* Superellipse soft clipping
 * Input:
 *   - point Q on the x-y plane
 *   - the equations of two superellipses (with major/minor axes given by
 *        a,b and A,B for the inner and outer ellipses, respectively)
 * Return value:
 *   - 0 if Q was inside the inner ellipse
 *   - 1 if Q was outside the outer ellipse
 *   - smoothly varying from 0 to 1 in between
 */
float clipSuperellipse(point Q;	/* Test point on the x-y plane */
		       float a, b;	/* Inner superellipse */
		       float A, B;	/* Outer superellipse */
		       float roundness;	/* Same roundness for both ellipses */
  )
{
  float result = 0;
  float x = abs(xcomp(Q)), y = abs(ycomp(Q));
  if(x != 0 || y != 0)
    {				/* avoid degenerate case */
      if(roundness < 1.0e-6)
	{
	  /* Simpler case of a square */
	  result = 1 - (1 - smoothstep(a, A, x)) * (1 - smoothstep(b, B, y));
	}
      else if(roundness > 0.9999)
	{
	  /* Simple case of a circle */
	  float sqr(float x)
	  {
	    return x * x;
	  }
	  float q = a * b / sqrt(sqr(b * x) + sqr(a * y));
	  float r = A * B / sqrt(sqr(B * x) + sqr(A * y));
	  result = smoothstep(q, r, 1);
	}
      else
	{
	  /* Harder, rounded corner case */
	  float re = 2 / roundness;	/* roundness exponent */
	  float q = a * b * pow(pow(b * x, re) + pow(a * y, re), -1 / re);
	  float r = A * B * pow(pow(B * x, re) + pow(A * y, re), -1 / re);
	  result = smoothstep(q, r, 1);
	}
    }
  return result;
}





/* Volumetric light shaping
 * Inputs:
 *   - the point being shaded, in the local light space
 *   - all information about the light shaping, including z smooth depth
 *     clipping, superellipse x-y shaping, and distance falloff.
 * Return value:
 *   - attenuation factor based on the falloff and shaping
 */
float ShapeLightVolume(point PL;	/* Point in light space */
		       string lighttype;	/* what kind of light */
		       vector axis;	/* light axis */
		       float znear, zfar;	/* z clipping */
		       float nearedge, faredge;
		       float falloff, falloffdist;	/* distance falloff */
		       float maxintensity;
		       float shearx, sheary;	/* shear the direction */
		       float width, height;	/* xy superellipse */
		       float hedge, wedge, roundness;
		       float beamdistribution;	/* angle falloff */
  )
{
  /* Examine the z depth of PL to apply the (possibly smooth) cuton and
   * cutoff.
   */
  float atten = 1;
  float PLlen = length(PL);
  float Pz;
  if(lighttype == "spot")
    {
      Pz = zcomp(PL);
    }
  else
    {
      /* For omni or area lights, use distance from the light */
      Pz = PLlen;
    }
  atten *= smoothstep(znear - nearedge, znear, Pz);
  atten *= 1 - smoothstep(zfar, zfar + faredge, Pz);

  /* Distance falloff */
  if(falloff != 0)
    {
      if(PLlen > falloffdist)
	{
	  atten *= pow(falloffdist / PLlen, falloff);
	}
      else
	{
	  float s = log(1 / maxintensity);
	  float beta = -falloff / s;
	  atten *= (maxintensity * exp(s * pow(PLlen / falloffdist, beta)));
	}
    }

  /* Clip to superellipse */
  if(lighttype != "omni" && beamdistribution > 0)
    atten *= pow(zcomp(normalize(vector PL)), beamdistribution);
  if(lighttype == "spot")
    {
      atten *=
	1 - clipSuperellipse(PL / Pz - point(shearx, sheary, 0), width,
			     height, width + wedge, height + hedge,
			     roundness);
    }
  return atten;
}




/* Evaluate the occlusion between two points, P1 and P2, due to a fake
 * blocker.  Return 0 if the light is totally blocked, 1 if it totally
 * gets through.
 */
float BlockerContribution(point P1, P2;
			  string blockercoords;
			  float blockerwidth, blockerheight;
			  float blockerwedge, blockerhedge;
			  float blockerround;
  )
{
  float unoccluded = 1;
  /* Get the surface and light positions in blocker coords */
  point Pb1 = transform(blockercoords, P1);
  point Pb2 = transform(blockercoords, P2);
  /* Blocker works only if it's straddled by ray endpoints. */
  if(zcomp(Pb2) * zcomp(Pb1) < 0)
    {
      vector Vlight = (Pb1 - Pb2);
      point Pplane = Pb1 - Vlight * (zcomp(Pb1) / zcomp(Vlight));
      unoccluded *=
	clipSuperellipse(Pplane, blockerwidth, blockerheight,
			 blockerwidth + blockerwedge,
			 blockerheight + blockerhedge, blockerround);
    }
  return unoccluded;
}




light k3d_uberlight(
		     /* Basic intensity and color of the light */
		     string lighttype = "spot"; float intensity = 1;
		     color lightcolor = color(1, 1, 1);
		     /* Z shaping and distance falloff */
		     float cuton = 0.01, cutoff = 1.0e6, nearedge =
		     0, faredge = 0;
		     float falloff = 0, falloffdist = 1, maxintensity = 1;
		     float parallelrays = 0;
		     /* xy shaping of the cross-section and angle falloff */
		     float shearx = 0, sheary = 0;
		     float width = 1, height = 1, wedge = .1, hedge = .1;
		     float roundness = 1;
		     float beamdistribution = 0;
		     /* Cookie or slide to control light cross-sectional color */
		     string slidename = "";
		     /* Noisy light */
		     float noiseamp = 0, noisefreq = 4;
		     vector noiseoffset = 0;
		     /* Shadow mapped shadows */
		     string shadowmap = "";
		     float shadowblur = 0.01, shadowbias = .01, shadownsamps =
		     16;
		     color shadowcolor = 0;
		     /* Ray traced shadows */
		     float raytraceshadow = 0, nshadowrays = 1;
		     vector shadowcheat = vector "shader"(0, 0, 0);
		     /* Fake blocker shadow */
		     string blockercoords = "";
		     float blockerwidth = 1, blockerheight = 1;
		     float blockerwedge = .1, blockerhedge =
		     .1, blockerround = 1;
		     /* Miscellaneous controls */
		     float nonspecular = 0;
		     output varying float __nonspecular = 0;
		     output float __nondiffuse = 0;
		     output float __foglight = 1;)
{
  /* For simplicity, assume that the light is at the origin of shader
   * space and aimed in the +z direction.  So to move or orient the
   * light, you transform the coordinate system in the RIB stream, prior
   * to instancing the light shader.  But that sure simplifies the
   * internals of the light shader!  Anyway, let PL be the position of
   * the surface point we're shading, expressed in the local light
   * shader coordinates.
   */
  point PL = transform("shader", Ps);

  /* For PRMan, we've gotta do it the hard way */
  point from = point "shader"(0, 0, 0);
  vector axis = normalize(vector "shader"(0, 0, 1));

  uniform float angle;
  if(lighttype == "spot")
    {				/* Spot light */
      uniform float maxradius = 1.4142136 * max(height + hedge + abs(sheary),
						width + wedge + abs(shearx));
      angle = atan(maxradius);
    }
  else if(lighttype == "arealight")
    {				/* BMRT area light */
      angle = PI / 2;
    }
  else
    {				/* Omnidirectional light */
      angle = PI;
    }
  __nonspecular = nonspecular;

  illuminate(from, axis, angle)
  {
    /* Accumulate attenuation of the light as it is affected by various
     * blockers and whatnot.  Start with no attenuation (i.e., a 
     * multiplicative attenuation of 1.
     */
    float atten = 1.0;
    color lcol = lightcolor;

    /* Basic light shaping - the volumetric shaping is all encapsulated
     * in the ShapeLightVolume function.
     */
    atten *=
      ShapeLightVolume(PL, lighttype, axis, cuton, cutoff, nearedge, faredge,
		       falloff, falloffdist, maxintensity / intensity, shearx,
		       sheary, width, height, hedge, wedge, roundness,
		       beamdistribution);

    /* Project a slide or use a cookie */
    if(slidename != "")
      {
	point Pslide = PL / point(width + wedge, height + hedge, 1);
	float zslide = zcomp(Pslide);
	float xslide = 0.5 + 0.5 * xcomp(Pslide) / zslide;
	float yslide = 0.5 - 0.5 * ycomp(Pslide) / zslide;
	lcol *= color texture(slidename, xslide, yslide);
      }

    /* If the volume says we aren't being lit, skip the remaining tests */
    if(atten > 0)
      {
	/* Apply noise */
	if(noiseamp > 0)
	  {
#pragma nolint
	    float n = noise(noisefreq * (PL + noiseoffset) * point(1, 1, 0));
	    n = smoothstep(0, 1, 0.5 + noiseamp * (n - 0.5));
	    atten *= n;
	  }

	/* Apply shadow mapped shadows */
	float unoccluded = 1;
	if(shadowmap != "")
	  unoccluded *=
	    1 - shadow(shadowmap, Ps, "blur", shadowblur, "samples",
		       shadownsamps, "bias", shadowbias);
	point shadoworigin;
	if(parallelrays == 0)
	  shadoworigin = from;
	else
	  shadoworigin = point "shader"(xcomp(PL), ycomp(PL), cuton);

	/* Apply blocker fake shadows */
	if(blockercoords != "")
	  {
	    unoccluded *=
	      BlockerContribution(Ps, shadoworigin, blockercoords,
				  blockerwidth, blockerheight, blockerwedge,
				  blockerhedge, blockerround);
	  }
	lcol = mix(shadowcolor, lcol, unoccluded);
	__nonspecular = 1 - unoccluded * (1 - __nonspecular);
      }
    Cl = (atten * intensity) * lcol;
    if(parallelrays != 0)
      L = axis * length(Ps - from);
  }
}
