displacement bumpy( float Km = 1; string texturename = "";) 
{
	float amp = Km * float texture(texturename, s, t); 
	P += amp * normalize(N); 
	N = calculatenormal(P); 
}
