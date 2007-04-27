surface k3d_glow(float attenuation = 2.0;
		 color centercolor = 1.0;
		 color edgecolor = color(1.0, 1.0, 0.0))
{
  float incidence, strength;
  color C;

  incidence = I.N;
  if(incidence < 0)		// If this point is on the front face of the sphere
    {
      // Normalize incidence
      incidence = incidence * incidence / (I.I * N.N);

      // Calculate a "strength" function based on incidence
      strength = pow(incidence, attenuation);

      C = mix(edgecolor, centercolor, strength);

      // Calculate final color and opacity - we set Oi near zero
      // and skip pre-multiplication to get an "add" effect ...
      Ci = C * strength * Os;
      Oi = 0.0001;
    }
  else
    {
      Ci = 0;
      Oi = 0;
    }
}
