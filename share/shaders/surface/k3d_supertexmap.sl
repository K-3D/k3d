/***************************************************************************
 * supertexmap.sl
 *
 * Description:
 *    Apply a texture map (possibly with associated alpha) 
 *    to a plastic surface.  This is essentially a replacement for the
 *    standard "paintedplastic", but with much more flexibility in the
 *    coordinate mapping of the texture.
 *
 * Parameters:
 *    Ka, Kd, Ks, roughness, specularcolor - the usual meaning.
 *    texturename - the name of the texture file.
 *    projection - specifies the projection type, one of "st", "planar",
 *                 "perspective", "cylindrical", "spherical".
 *    textureprojspace - the space in which the texture projection is
 *                applied; either a standard space like "shader", or a
 *                named coordinate system.
 *    mx - 16 floats giving an 3-D affine transformation to apply to the
 *                projected point before texture coordinates are extracted.
 *    truedisp - 1 for true displacement, 0 for bump mapping
 *
 * Author: Larry Gritz (gritzl@acm.org)
 *
 * Reference:
 *   _Advanced RenderMan: Creating CGI for Motion Picture_, 
 *   by Anthony A. Apodaca and Larry Gritz, Morgan Kaufmann, 1999.
 *
 ***************************************************************************/

#include "k3d_filterwidth.h"
#include "k3d_project.h"
#include "k3d_displace.h"
#include "k3d_material.h"


surface k3d_supertexmap(float Ka = 1, Kd = .5, Ks = .5, roughness = .1;
			/* base color */
			string Csmapname = "", Csproj = "st", Csspace =
			"shader";
			float Csmx[16] =
			{
			1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
			float Csblur = 0;
			/* opacity */
			string Osmapname = "", Osproj = "st", Osspace =
			"shader";
			float Osmx[16] =
			{
			1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
			float Osblur = 0;
			/* specularity */
			string Ksmapname = "", Ksproj = "st", Ksspace =
			"shader";
			float Ksmx[16] =
			{
			1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
			float Ksblur = 0;
			/* displacement */
			string dispmapname = "", dispproj = "st", dispspace =
			"shader";
			float dispmx[16] =
			{
			1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
			float dispblur = 0;
			float truedisp = 1;
  )
{
  /* Start out with the regular plastic parameters, unless overridden
   * by maps.
   */
  color Ct = Cs, Ot = Os;
  float ks = Ks;
  float disp = 0;

  /* Color mapping */
  if(Csmapname != "")
    Ct =
      ApplyColorTextureOver(Ct, Csmapname, Csproj, P, Csspace,
			    array_to_mx(Csmx), Csblur);

  /* Opacity mapping */
  if(Osmapname != "")
    Ot =
      ApplyColorTextureOver(Ct, Osmapname, Osproj, P, Osspace,
			    array_to_mx(Osmx), Osblur);

  /* specularity mapping */
  if(Ksmapname != "")
    ks =
      ApplyFloatTextureOver(Ks, Ksmapname, Ksproj, P, Ksspace,
			    array_to_mx(Ksmx), Ksblur);

  /* displacement mapping */
  if(dispmapname != "")
    {
      disp =
	ApplyFloatTextureOver(disp, dispmapname, dispproj, P, dispspace,
			      array_to_mx(dispmx), dispblur);
      N = Displace(normalize(N), dispspace, disp, truedisp);
    }

  /* Illumination model - just use plastic */
  normal Nf = faceforward(normalize(N), I);
  Ci = MaterialPlastic(Nf, Ct, Ka, Kd, ks, roughness);
  Oi = Ot;
  Ci *= Oi;
}
