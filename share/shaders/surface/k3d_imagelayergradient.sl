surface k3d_imagelayergradient(color zenith = color(0, 0, 1);
			       color sky = color(.5, .5, 1);
			       color ground = color(.6, .6, .1);
			       color nadir = color(0, 0, 0);)
{
  Oi = Os;

  if(v < 0.5)
    {
      Ci = Cs * mix(zenith, sky, v * 2.0);
    }
  else
    {
      Ci = Cs * mix(ground, nadir, (v - 0.5) * 2.0);
    }
}
