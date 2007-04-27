
surface k3d_checkerboard_solid(
	float Ka = 1.0;
	float Kd = 1.0;
	float Ks = 0.5;
	float roughness = 0.1;
	color specularcolor = 1.0;
	uniform float TileX = 1.0;
	uniform float TileY = 1.0;
	uniform float TileZ = 1.0;
	color EvenColor = 0.0;
	string shadingspace = "shader";
	)
{
	color Ct;
	point Nf;
	float x, y, z, sum;

	point Pshad = transform(shadingspace, P);

	x = mod(floor(xcomp(Pshad) / TileX), 2.0);
	y = mod(floor(ycomp(Pshad) / TileY), 2.0);
	z = mod(floor(zcomp(Pshad) / TileZ), 2.0);

	sum = mod(x + y + z, 2.0);
	if(sum >= 0.5)
		{
			Ct = EvenColor;
		}
	else
		{
			Ct = Cs;
		}

	// Diffuse reflection model ...
	Nf = normalize(faceforward(N, I));
	Oi = Os;
	Ci = Os * (Ct * (Ka * ambient() + Kd * diffuse(Nf)) + specularcolor * Ks * specular(Nf, -normalize(I), roughness));
}
