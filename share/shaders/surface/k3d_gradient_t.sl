surface k3d_gradient_t(
	color start_color = color(1, 0, 0);
	color end_color = color(1, 1, 0);
	float additive = 0.0;
	)
{
	color C = mix(start_color, end_color, t);

	if(additive != 0.0)
		{	
			Ci = C;
			Oi = 0.0;
		}
	else
		{
			Ci = C * Os;
			Oi = Os;
		}
}
