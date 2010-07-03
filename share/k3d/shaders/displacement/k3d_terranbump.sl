
#include "k3d_noises.h" 


#define N_OFFSET 0.7
#define VERY_SMALL 0.0001



displacement k3d_terranbump(float spectral_exp = 0.5;
			    float lacunarity = 2, octaves = 7;
			    float bump_scale = 0.04; float multifractal = 0;
			    float dist_scale = .2; float offset = 0;
			    float sea_level = 0;)
{
  float chaos;
  point Ptexture, tp;
  float l, o, a, i, weight;	/* Loop variables for fBm calc */
  float bumpy;

  /* Do all shading in shader space */
  Ptexture = transform("shader", P);

  if(multifractal == 0)
    {				/* use a "standard" fBm bump function */
      o = 1;
      l = 1;
      bumpy = 0;
      for(i = 0; i < octaves; i += 1)
	{
	  bumpy += o * snoise(l * Ptexture);
	  l *= lacunarity;
	  o *= spectral_exp;
	}
    }
  else
    {				/* use a "multifractal" fBm bump function */
      /* get "distortion" vector, as used with clouds */
      Ptexture += dist_scale * DNoise(Ptexture);
      /* compute bump vector using MfBm with displaced point */
      o = spectral_exp;
      tp = Ptexture;
      weight = abs(VLNoise(tp, 1.5));
      bumpy = weight * snoise(tp);
      for(i = 1; i < octaves && weight >= VERY_SMALL; i += 1)
	{
	  tp *= lacunarity;
	  /* get subsequent values, weighted by previous value */
	  weight *= o * (N_OFFSET + snoise(tp));
	  weight = clamp(abs(weight), 0, 1);
	  bumpy += snoise(tp) * min(weight, spectral_exp);
	  o *= spectral_exp;
	}
    }

  /* get the "height" of the bump, displacing by offset */
  chaos = bumpy + offset;

  /* set bump for land masses (i.e., areas above "sea level") */
  if(chaos > sea_level)
    P += (bump_scale * bumpy) * normalize(Ng);

  /* Recalculate the surface normal (this is where all the real magic is!) */
  N = calculatenormal(P);
}
