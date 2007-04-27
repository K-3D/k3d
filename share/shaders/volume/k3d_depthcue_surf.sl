/* k3d_depthcue_surf inspired by depthcue_surf.sl by Pixar
 * - Standard depthcue surface shader for RenderMan Interface.
 *  Author: Daniel S. Matthews dan@3-e.net
 */

surface k3d_depthcue_surf(

float mindistance = 0, maxdistance = 1, CScale=1000;
string debug="y";
)
{
  float d;
  d = clamp((depth(P) - mindistance) / (maxdistance - mindistance), 0, 1);
  Ci = 0;
  Ci = 1 - mix(Ci, (1,1,1), d)*CScale;
	if (debug=="y")
  	{
			printf("<debug shadername='k3d_depthcue_surf' shadertype='surface'>%p\t%f\t%f\t%c</debug>\n", P, depth(P), d, Ci);
		}
}

