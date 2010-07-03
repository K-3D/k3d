/* incandPlastic.sl   version 1.0, Jan. 2002
 * Felipe Esquivel <felipe@siggraph.org.mx>
 * 
 * Este es un shader plastico con componente de
 * incandescencia como el contenido en un Blinn. 
 *
 * This is a normal plastic shader and includes an
 * incandescence value like the blinn shader.
 */
surface 
k3d_incandplastic(color SurfaceColor = color(0, 0.25, 1);
	      color SurfaceOpacity = 1;
	      float Ka = 0;
	      float Kd = 0.9;
	      float Ks = 0.5;
	      float Roughness = 0.05;
	      color SpecularColor = 1;
	      color Incandescence = color(0, 0, 0.3);)
{
  normal Nf;
  vector NI;
  
  Nf = faceforward(normalize(N), I, normalize(N));
  NI = -normalize(I);
  Ci = SurfaceOpacity * (SurfaceColor * (Ka * ambient() + Kd * diffuse(Nf)) +
			 SpecularColor * Ks * specular(Nf, NI, Roughness) +
			 Incandescence);
  Oi = SurfaceOpacity;
}
