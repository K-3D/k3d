light 
k3d_hdri2 (float intensity = 1;
       color lightcolor = 1;
       string envname = "";
       string envspace = "world";
       float envblur = 0.5;
       string shadowname = "";
       float shadowblur = 0.001;
       float shadowbias = 0.01;
       float shadowsamples = 1;)
{
    vector Lenv = vtransform (envspace, P-Ps);
    illuminate (P) {
	if (envname != "")
	    Cl = environment (envname, Lenv, "blur", envblur);
	else Cl = 0;
        if (shadowname != "")
            Cl *= 1 - color shadow (shadowname, Ps, "samples", shadowsamples,
				    "blur", shadowblur, "bias", shadowbias);
    }
}
