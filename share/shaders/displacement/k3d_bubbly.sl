/*  bubbly.sl written a while ago by Ivan DeWolf
 *  ivan@SpamSucks_martian-tools.com
 *  feel free to copy, distribute, hack and/or abuse this code 
 *  in any way you see fit, but please leave my name near the top
 *
 *  basicly it's F1 squared as described in 
 *  "a cellular texture basis function" by Steven Worley in the 
 *  siggraph proceedings from 1996, except this uses a noised grid of cells
 *  instead of the sparse convolution in the paper.
 *  not as elegant, but hey, it works.
 *
 *  the "bubbles" are paraboloids diplaced along the normal. 
 *  try lowering bubsize for pimples.
 *  try negative values for Kd.
 *  try anything you feel like.
 *
 *  PARAMETERS:
 *  mult    - multiples of bubbles per unit
 *  Nzscale - scale of the noise that randomizes the location of 
 *            the bubbles. set this to zero for a perfect grid of bubbles.
 *	      dont go higher than two with this.
 *  Kd      - coefficient of displacement. 
 *	      Displacement bounds and this value should be equal.
 *  bubsize - basicly the radius of the bubbles. distance in rVu's.
 */

displacement
k3d_bubbly(
  float  mult	 = 5,
         Nzscale = 1,
         Kd      = .2,
         bubsize = 1)
{
  normal Nn = normalize(N);
  float a,b,c,bub;
  float dist, shortest=10000;
  point Po = transform("object",P)*mult;

  /*true cell center, surrounding cell centers, noised cell center*/
  point trucell, surrcell, nzcell;
  vector offset;
  
  setxcomp(trucell,floor(xcomp(Po))+.5);
  setycomp(trucell,floor(ycomp(Po))+.5);
  setzcomp(trucell,floor(zcomp(Po))+.5);
			     
  /*what is the shortest distance to a noised cell center?*/
  for(a = -1; a<= 1; a+=1){
    for(b = -1; b<=1; b += 1){
      for(c = -1; c<=1; c += 1){
	offset = vector(a,b,c);
	surrcell = trucell+offset;
	nzcell = surrcell + ((vector cellnoise(surrcell)-.5)*Nzscale);
	dist = distance(Po,nzcell);
	if(dist<shortest) shortest = dist;
      }
    }
  }
  bub = clamp(shortest,0,bubsize)/bubsize; 
  P+= Nn*(pow(bub,2)-1)*Kd;
  N = calculatenormal(P);
}

































