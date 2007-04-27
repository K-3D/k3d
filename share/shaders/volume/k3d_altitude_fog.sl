
volume k3d_altitude_fog(
	float max_height = 1; 
	float min_height = 0;
	color background = 0;
	)
{
	point PP = transform("world", P);
	float fog = 1 - smoothstep(min_height, max_height, ycomp(PP));

	Ci = mix(Ci, background, fog);
	Oi = mix(Oi, color(1, 1, 1), fog);
}

