/* IDoutline written by Ivan DeWolf
 * it's the valdez algorithm. short and sweet.
 * width sets the line width.
 */
surface
k3d_outline(float width = .05)
{

  normal Nn = normalize (-N);
  float dot = Nn . normalize(I);

  P += Nn*width;
  Ci = 0;
  Oi = smoothstep(-.01,0,dot);
}
