light k3d_slideprojector(float intensity = 2000;
			 color lightcolor = 1;
			 point from = point(0, 0, 0);
			 point to = point(0, 0, 1);
			 point up = point(0, 1, 0); float coneangle = 45;
			 float conedeltaangle = 5; float beamdistribution = 2;
			 string slidename = "")
{
  uniform vector Z = normalize(to - from);
  uniform vector X = Z ^ up;
  uniform vector Y = normalize(X ^ Z);
  X = normalize(Y ^ Z);

  uniform float rconeangle = radians(coneangle) * 0.5;
  uniform float rconedeltaangle = radians(conedeltaangle);
  uniform float spread = 1 / tan(rconeangle);

  float attenuation, cosangle;
  float Pt, Pu, Pv, sloc, tloc;

  color Ct;

  illuminate(from, Z, rconeangle)
  {
    L = Ps - from;
    Pt = L.Z;
    Pu = L.X;
    Pv = L.Y;

    sloc = spread * Pu / Pt;
    tloc = spread * Pv / Pt;
    sloc = sloc * .5 + .5;
    tloc = tloc * -.5 + .5;

    cosangle = (L.Z) / length(L);
    attenuation = pow(cosangle, beamdistribution) / (L.L);
    attenuation *=
      smoothstep(cos(rconeangle), cos(rconeangle - rconedeltaangle),
		 cosangle);

    if(slidename == "")
      {
	Ct = 0.0;
      }
    else
      {
	Ct = color texture(slidename, sloc, tloc);
      }

    Cl = attenuation * intensity * lightcolor * Ct;
  }
}
