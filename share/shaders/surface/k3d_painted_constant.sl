surface k3d_painted_constant(
	string texturename = "";
	float s0 = 0.0;
	float s1 = 1.0;
	float t0 = 0.0;
	float t1 = 1.0;
	)
{
	color Ct = color(1, 1, 1);
	float Ot = 1;
	
	if(texturename != "")
		{
			float ss = mix(s0, s1, s);
			float tt = mix(t0, t1, t);
			
			Ct = color texture(texturename, ss, tt);
			Ot = float texture(texturename[3], ss, tt);
		}

	Oi = Os * Ot;
	Ci = Oi * Cs * Ct;
}

