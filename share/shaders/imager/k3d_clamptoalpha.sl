/*
 * clamptoalpha.sl -- imager shader that forces Ci to be non-negative
 *    and with each component to greater than the alpha value of the
 *    pixel.
 */

imager
k3d_clamptoalpha()
{
  Ci = clamp(Ci, color(0, 0, 0), color(alpha, alpha, alpha));
}
