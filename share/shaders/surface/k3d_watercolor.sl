/*
 *	The shader exploits that fact that the Renderman interface already
 *	provides a built-in Fresnel quantity calculator, which provides
 *	R, T, ^nr, and ^nt using the surface normal, incident direction
 *	vector, and index of refraction. The shader for the air-to-water 
 * 	case is as follows:
 */


surface k3d_watercolor(
	color upwelling = color(0, 0.2, 0.3);
	color sky = color(0.69,0.84,1);
	color air = color(0.1,0.1,0.1);
	float nSnell = 1.34;
	float Kdiffuse = 0.91;
	string envmap = "";
	)
{
	float reflectivity;
	vector nI = normalize(I);
	vector nN = normalize(Ng);
	float costhetai = abs(nI . nN);
	float thetai = acos(costhetai);
	float sinthetat = sin(thetai)/nSnell;
	float thetat = asin(sinthetat);
	if(thetai == 0.0)
	{
		reflectivity = (nSnell - 1)/(nSnell + 1);
		reflectivity = reflectivity * reflectivity;
	}
	else
	{
		float fs = sin(thetat - thetai) / sin(thetat + thetai);
		float ts = tan(thetat - thetai) / tan(thetat + thetai);
		reflectivity = 0.5 * ( fs*fs + ts*ts );
	}
	vector dPE = P-E;
	float dist = length(dPE) * Kdiffuse;
	dist = exp(-dist);
	
	if(envmap != "")
	{
		sky = color environment(envmap, nN);
	}
	Ci = dist * ( reflectivity * sky + (1-reflectivity) * upwelling ) + (1-dist)* air;
}

