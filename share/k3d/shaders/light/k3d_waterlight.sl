/** k3d_waterlight

Copyright (c) 1995-2004, Timothy M. Shead

A cheap trick to simulate underwater caustics - best used for deep-sea effects

*/

light k3d_waterlight(
	float minimum_intensity = 0.3;
	float maximum_intensity = 1.0;
	float frequency = 1;
	float phase = 0;
	color lightcolor = 1;
	string shadowname="";
	float  samples=16;
	float  blur=.01;
	float  bias=.01;
	)
{
	solar(vector "shader" (0, 0, -1), 0)
	{
		Cl = mix(minimum_intensity, maximum_intensity, float noise(frequency * Ps)) * lightcolor;
		
		if(shadowname != "")
		{
			Cl *= 1 - shadow(shadowname, Ps, "samples", samples, "blur", blur, "bias", bias);
		}
	}
}

