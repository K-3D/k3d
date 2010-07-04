
volume k3d_altitude_fog(
	float max_height = 1; 
	float min_height = -1;
	matrix m = 1;
	color background = 0;
	)
{
	point PP = transform(m, transform("world", P));
	float fog = 1 - smoothstep(min_height, max_height, zcomp(PP));

	Ci = mix(Ci, background, fog);
	Oi = mix(Oi, color(1, 1, 1), fog);
}

