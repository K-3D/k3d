light k3d_hdri1 (string envname = "")
{
    solar () {
	if (envname != "")
	    Cl = environment (envname, -L, "blur", 0.5);
	else Cl = 0;
    }
}
