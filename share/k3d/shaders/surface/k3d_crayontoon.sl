/* crayonToon.sl Ver. 1.0 Felipe Esquivel
 * Abril 2002    <felipe@siggraph.org.mx>
 * 
 * Este shader es un constant con un poco de difusion,
 * hay que escoger con cuidado ambos colores para obtener
 * el estilo deseado. Mi intencion fue hacer un color dependiente
 * del otro pero es mejor dejar al usuario esa eleccion. 
 *
 * This shader is a constant with little diffusion, 
 * choose wisely both colors to obtain the crayon look. My
 * idea was to make one color dependable of the other but is
 * better to let the user change this.
 */
surface k3d_crayontoon(color colorBase = color(1, .92, .44),
                         colorSombra = color(1, .81, .37);
                   float eta = 1.3,
                         Kd = .33;)
{
  float Kr1, Kt1;
  vector Nf = normalize(N);

  fresnel(normalize(I), faceforward(Nf, I, Nf), 1/eta, Kr1, Kt1);
  if(Kt1 > 1) Kt1 = 1;

  Ci = Os * (mix(colorBase, colorSombra, Kt1) + Kd * Kt1 * diffuse(Nf) * colorBase);
  Oi = Os;
}

