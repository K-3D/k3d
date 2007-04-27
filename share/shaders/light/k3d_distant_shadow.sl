light k3d_distant_shadow(
	float  intensity=1 ;
	color  lightcolor=1 ;
	point from = point "shader" (0,0,0) ;
	point to   = point "shader" (0,0,1) ;
	string shadowname="";
	float  samples=16;
	float  blur=.01;
	float  bias=.01;
	)
{
	solar(to - from, 0.0)
	{
		Cl = intensity * lightcolor;

		if(shadowname != "")
		{
			Cl *= 1 - shadow(shadowname, Ps, "samples", samples, "blur", blur, "bias", bias);
		}

	}
}

