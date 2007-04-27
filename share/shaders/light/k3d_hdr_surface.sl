/*Environment Light to project an environment map onto 3D geometry - designed
* to work with 32 bit floating point TIFF environment maps to give a higher 
* dynamic range 
*
*Created by Simon Bunker 27th September 2001
*simon@rendermania.com http://www.rendermania.com/HDRI/
*
*This shader is made freely available under the proviso that this copyright
*notice remain intact and that I am acknowledged as the original author. Please
*post links back to the above address*or get in contact if you have any queries
*or bug reports related to this shader 
*
*/

color hdrenv(string envname;vector R;float blur)
{
	color hdrtex;

	vector D = normalize(vtransform("world",R));

	float Dx = xcomp(D);
	float Dy = ycomp(D);
	float Dz = zcomp(D);

	float r = 0.159154943 * acos(Dz) / sqrt((Dx * Dx) + (Dy * Dy));

	float ss = 0.5 + (Dx * r);
	float tt = 0.5 + (Dy * r);
	ss = 1 - ss;

	if(envname !=""){
		hdrtex = texture(envname,ss,tt,"blur",blur);
	}

	return hdrtex;
}


surface k3d_hdr_surface (
	float intensity =1.0;
	float exposure_compensation_stops = 0.0;
	string envname = "";
	string envspace = "shader";
	string mappingtype = "probe";
	point origin = (0,0,0);
	float blur = 0.0;
)
{

	color Ct;

	vector R = normalize(vtransform(envspace,(origin - P)));

	if (mappingtype == "probe"){
		if(envname != ""){
			Ct = hdrenv(envname,R,blur);
		}
	}
	else if (mappingtype == "environment"){
		if(envname != ""){
			Ct = environment(envname,R,"blur",blur);
		}
	}
	else if (mappingtype == "planar"){
		if(envname != ""){
			Ct = texture(envname,s,t,"blur",blur);
		}
	}

	float exposure = pow(2,exposure_compensation_stops);

	Oi = Os;
	Ci = Cs * Oi * Ct * intensity * exposure;

}
