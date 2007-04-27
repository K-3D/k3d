/* 
 * fur.sl
 *   - Simple algorithm: checkerboard
 *
 *     by Lawrence D. Chin, cs184-bo
 */



displacement
k3d_fur1 ( float height = 1.0,
              fac = 1.0;
    )
{

  float smod = mod(u*fac,1),
        tmod = mod(v*fac,1);

  if (smod < 0.5) {
    if (tmod < 0.8) 
      P += 0.5;
    else 
      P += 0.5 + N * height;
  } else {
    if (tmod < 0.2) 
      P += 0.5 + N * height;
    else
      P += 0.5;
 }
      
  N = calculatenormal(P);
}

