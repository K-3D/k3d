/* SuperToon.sl V1.1    Felipe Esquivel 
 * <felipe@siggraph.org.mx>, abril 2002
 *
 * Toon shader con antialiasing. Modula diffuse mediante escaloneo de modo que
 * no se tenga un degradado en el cambio de color. En su lugar el cambio se
 * hace en tres puntos definidos por lim2, lim3 y lim4. A falta de antialiasing
 * analitico se utiliza delta para determinar el nivel de antialiasing del shader.
 *
 * Toon shader with non-analitic antialiasing (controlled by delta). Difusion is modulated with
 * clamping, the inflection points are lim2, lim3 and lim4. This shader is very cheap, 
 * reason why no border detection is allowed. Now with specular.
 */

color splitColor(color valor; float delta; float lim2; float lim3; float lim4;){
  color C = 0;

  if(comp(valor, 0) < lim3 - delta){
    C = smoothstep(lim2 - delta, lim2 + delta, comp(valor, 0))* lim2;
  }
  else if(comp(valor, 0) >= lim3 - delta && comp(valor, 0) < lim4 - delta){
    C = lim2 + (smoothstep(lim3 - delta, lim3 + delta, comp(valor, 0))* lim3);
  }
  else if(comp(valor, 0) >= lim4 - delta){
    C = lim3 + (smoothstep(lim4 - delta, lim4, comp(valor, 0))* lim4);
  }

  return C;
}

surface k3d_supertoon(float Kd = 1,
                        Ks = 0,
                        roughness = 0.5, /* specular roughness */
                        delta = 0.015,   /* antialiasing level */
                        lim2 = 0.3,      /* inflection points  */
                        lim3 = 0.7,
                        lim4 = 1;
                  color specColor = 1;)
{
    normal Nf = normalize(N);
    vector NI = normalize(I);
    color  difusion = 0;
    color  especular = 0;

    if(Ks != 0){ /* Some optimization. If the multiplier is zero, why call an expensive function? */
      especular = specular(Nf, -NI, roughness);
      especular = splitColor(especular, delta * 3, lim3, lim4, lim4);
    }
    if(Kd != 0){
      difusion = diffuse(faceforward(Nf, NI, Nf));
      difusion = splitColor(difusion, delta, lim2, lim3, lim4);
    }
    
    Oi = Os;
    Ci = Os * (Cs * Kd * difusion + Ks * especular * specColor);
}


