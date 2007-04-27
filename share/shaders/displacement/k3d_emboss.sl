/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.383 */
/* Listing 16.36  Displacement shader embossing a surface using a texture */

/*
 * emboss(): emboss a pencil with lettering.  It uses the same texture
 * 	map as sdixon() to define the lettering.
 */
displacement
k3d_emboss(	
	float	Km		= .05;
	string	texturename	= "")
{
	if( texturename != "") {
		P -= Km * texture(texturename, s, t) * normalize(N);
		N = calculatenormal(P);
	}
}
