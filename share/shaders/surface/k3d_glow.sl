surface k3d_glow(
	float attenuation = 2;
	color edgecolor = color(0, 0, 1);
	)
{
	float falloff = I.N;

	if(falloff < 0)
	{
		falloff = falloff * falloff / (I.I * N.N);
		falloff = pow(falloff, attenuation);

		Ci = mix(edgecolor, Cs, falloff) * falloff;
		Oi = 1e-5;
	}
	else
	{
		Ci = 0;
		Oi = 0;
	}
}

