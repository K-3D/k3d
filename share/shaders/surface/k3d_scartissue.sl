/*  IDscartissue.sl written by Ivan DeWolf
 *  an unremarkable recursive noise to make scartissue
 *  feel free to copy, distribute, hack and/or abuse this code 
 *  in any way you see fit, but please leave my name near the top
 */
surface
k3d_scartissue(
	float	Ks			=  2, 
		Kd			= .5, 
		Ka			=  1, 
		roughness		= .2; 
)
{
	point Psh = transform("object",P);
	uniform float i, freq = 2.0, offset = 0;
	float nz = 0;
	color light = color (1,.3,.2);
	color dark = color (.015,.02,.02);
	color base;
	normal Nn = normalize(N);
	normal Nf = faceforward(normalize(N), I );
	vector V = normalize(-I);

	for (i = 0; i < 10; i = i + 1) {
		nz += abs(.5 - noise( (freq * Psh)+offset)) / freq;
		freq *= 1.2;
		offset += 10;
	}
	base = mix(dark,light,nz);
	P += Nn*nz*.3;
	N = calculatenormal(P);
	Nf = faceforward(normalize(N), I );

	Ci = ( base * (Ka + Kd*diffuse(Nf)) + 
	 	(1-nz)*Ks * specular(Nf,V,roughness) );
}
