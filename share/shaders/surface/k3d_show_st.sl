/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.344 */
/* Listing 16.11  Shader mapping texture-space coordinates to colors */

/*
 * show_st(): color surface point according to its s,t coordinates.
 */
surface
k3d_show_st()
{
	Ci = color(s, t, 0);
	Oi = 1;
}

