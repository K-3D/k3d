light
k3d_causticlight ( float intensity = 1;
	       color lightcolor = 1;
	       point from = point "shader" (0,0,0);
	       point to = point "shader" (0,0,1);
	       float coneangle = radians(30);
	       float conedeltaangle = radians(5);
	       float beamdistribution = 2;
	       string shadowname = "";
	       float  samples = 16;
	       float  blur = 0.01;
	       float  bias = 0.01;
               float  threshold = 0.5;
	       float  noiseamp = 0, noisefreq = 1, noisepow = 1;
    )
{
    uniform vector axis = normalize(to-from);

    illuminate (from, axis, coneangle) {
	float cosangle = (L . axis) / length(L);
	float atten = pow (cosangle, beamdistribution) / (L . L);
	atten *= smoothstep (cos(coneangle), cos(coneangle-conedeltaangle),
			     cosangle);
	Cl = atten * intensity * lightcolor;
        if (shadowname != "") {
            float caustic = shadow (shadowname, Ps, "samples", samples,
				    "blur", blur, "bias", bias);
	    caustic = smoothstep (threshold, 1, caustic);
	    if (noiseamp != 0) {
		point PL = transform ("shader", Ps);
		caustic *= noiseamp * pow (noise(PL*noisefreq), noisepow);
	    }
            Cl *= caustic;
	}
    }
}
