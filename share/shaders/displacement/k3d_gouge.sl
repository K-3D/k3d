/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.379 */
/* Listing 16.32  Displacement shader for beating up a surface with a texture  */

/*
 * gouge(): Use a texture map to displace a surface.
 */
displacement
k3d_gouge(
	float	Km	= 0.03,
		spin	= 0.0;		/* each pin gets its texture spun */
	string	texturename = "")
{
	float	y = ycomp(transform("object",P));	/* convert to object space	 */

   /* Use the same texture map that the surface shader uses. Spin it a different
    *  amount for each pin so that similarity is not detected.  The texture 
    *  determines the size of the gouge. Multiply by the Km factor, then 
    *  displace the surface point inwards by that amount. 
    */

   if( texturename != "") 
   {  P += (-Km * texture(texturename,s+spin,y/15.0)) * normalize(N);
      N = calculatenormal(P);
   }
}
