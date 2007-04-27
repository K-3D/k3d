/*IDfill written by Ivan DeWolf
 *simple toon fill shader with shadows
 *compare this to plastic.sl
 */
surface 
k3d_fill( 
	float	Kd			=   1, 
		Ka			=  .7,
		min			=   0.05,
		max			=   0.055)
{
	normal Nf = faceforward(normalize(N), I );
	Ci = Cs * (Ka*ambient() + Kd*(smoothstep(min,max,comp(diffuse(Nf),0))));
}
