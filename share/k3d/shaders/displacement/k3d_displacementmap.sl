displacement k3d_displacementmap(float Km = 1;
				 float ZeroPoint = 0.0;
				 string texturename = "";)
{
  point PP;
  point N1, N2;
  float Ct;

  Ct = (texturename != "") ? float texture(texturename) : 0;

  N1 = calculatenormal(P);
  P = P - (Km * (Ct - ZeroPoint)) * normalize(N);
  N2 = calculatenormal(P);
  N = normalize(N) + normalize(N2) - normalize(N1);
}
