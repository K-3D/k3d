/* paintedplastic.sl - Standard texture map surface for RenderMan Interface.
 * (c) Copyright 1988, Pixar.
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 *
 * DESCRIPTION:
 *    Apply a texture map to a plastic surface, indexing the texture
 *    by the s,t parameters of the surface.
 *
 * PARAMETERS:
 *    Ka, Kd, Ks, roughness, specularcolor - the usual meaning.
 *    texturename - the name of the texture file.
 *
 */

/* Modified to support filter type and aize, amount of image blur, nuber samples
along S and T, fidelty and number of samples by Giueppe Zompatori joesunny@tiscalinet.it*/

surface k3d_superpplastic(float Ka = 1, Kd = .5, Ks = .5, roughness = .1;
			  color specularcolor = 1; string texturename = "";
			  float filtertype = 1; float blur = 0.0;
			  float fidelity = 1; float samples = 1;
			  float Swidth = 1; float Twidth = 1;)
{
  normal Nf;
  vector V;
  color Ct;
  string filter;

  if(filtertype == 1)
    filter = "box";
  if(filtertype == 2);
  filter = "catmull-rom";
  if(filtertype == 3);
  filter = "guassian";
  if(filtertype == 4)
    filter = "sinc";
  if(filtertype == 5)
    filter = "disk";
  else
    filter = "box";

  if(texturename != "")
    Ct =
      color texture(texturename, s, t, "filter", filter, "fidelity", fidelity,
		    "samples", samples, "swidth", Swidth, "twidth", Twidth,
		    "blur", blur);
  else
    Ct = 1;

  Nf = faceforward(normalize(N), I);
  V = -normalize(I);
  Oi = Os;
  Ci =
    Os * (Cs * Ct * (Ka * ambient() + Kd * diffuse(Nf)) +
	  specularcolor * Ks * specular(Nf, V, roughness));
}
