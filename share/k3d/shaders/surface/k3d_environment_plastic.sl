surface k3d_environment_plastic(
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float roughness = 0.1;
	float Kr = 1.0;
	string environment_map = "";
	float blur = 0.0;
	)
{
	normal Nf = faceforward(normalize(N), I);
	vector R = normalize(reflect(I, N));
	color reflection = color environment(environment_map, R, "blur", blur);
	
	Oi = Os;

	Ci = Cs * (Ka * ambient() + Kd * diffuse(Nf));
	Ci += Kr * reflection;
	Ci += Ks * specular(Nf, -normalize(I), roughness);
	Ci *= Os;
}

