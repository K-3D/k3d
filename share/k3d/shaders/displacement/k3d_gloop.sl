/*  IDGloop.sl written 8/99 by Ivan DeWolf
 *  courtesy of Rhythm & Hues  (thanks!)
 *  ivan@SpamSucks_martian-labs.com
 *
 * an example of nonlinear displacements 
 * generated from the gradient of a noise function.
 * Written as an explanation to the talk I gave at
 * the "stupid renderman tricks" section of the 1998
 * Renderman Users Group meeting at siggraph.
 *
 * freq - the frequency of the noise. Lower values
 *        will cause a larger displacement so you will 
 *        have to change your displacement bounds to fit
 *        rhymes with freak.
 *
 * magnitude - how far to displace. probably fine set where it is.
 *
 * displacement bounds can be computed with the following 
 * wildly complex formula:
 *
 * 10*magnitude/freq
 * 
 */

displacement
k3d_gloop(float freq = 5, magnitude = .2){

  vector overdist = .1;
  vector stepsize = magnitude/freq;
  float numsteps = 20;
  point  Psh = transform("object",P)*freq;
  float  i;

  vector dPduN = normalize(vtransform("object",dPdu));
  vector dPdvN = normalize(vtransform("object",dPdv));

  point  Pou = Psh + (dPduN*overdist);/*P Over a distance in U*/
  point  Pov = Psh + (dPdvN*overdist);

  /*noise function at the point, over in u, and over in v*/
  float  nz = noise(Psh)-.5;
  float  nzou = noise(Pou)-.5;
  float  nzov = noise(Pov)-.5;

  float  chu = (nz - nzou);/*change in noise value in u*/
  float  chv = (nz - nzov); 

  /*init deflected derivatives*/
  vector DdPdu = dPduN;
  vector DdPdv = dPdvN;

  vector step = DdPdu^DdPdv;

  /*where it all happens*/
  for(i=1;i<numsteps;i=i+1){
    P -= vtransform("object","current",step)*nz*stepsize;
    DdPdu = normalize(DdPdu+(step*chu));
    DdPdv = normalize(DdPdv+(step*chv));
    step = DdPdu^DdPdv;
  }

  /* wash your hands after displacing P*/
  N = normalize(calculatenormal(P));
}











