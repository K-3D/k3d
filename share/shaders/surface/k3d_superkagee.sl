

#define BOOL    float
#define ENUM    float

#define FILT_BOX 0
#define FILT_GAUSS 1

/**************************************************/
/*** SHADOW ***************************************/
/**************************************************/

float do_shadow6(
    uniform string      theName;
    varying point       thePoint;
    uniform ENUM        theFilt;
    uniform float       theBlur,
                        theSamples,
                        theBias;
    ) {
    uniform string filtTypes[2] = {"box", "gaussian"};
    uniform string theFilterName = filtTypes[clamp(theFilt,0,1)];
    float inShadow;
#define SHADPOINT thePoint
    inShadow = shadow (theName,
                            SHADPOINT,
                            "filter",   theFilterName,
                            "blur",     theBlur,
                            "samples",  theSamples,
                            "bias",     theBias);
return(inShadow);
}

/******************************************************/
/*** SHADER BEGINS ************************************/
/******************************************************/


surface k3d_superkagee(
    color shadColor = 0;
    string     shadownamea = "";
            ENUM        shadowfilta = FILT_BOX;
            float       shadowblura = 0,
                        shadowbiasa = 0,
                        shadowsamplesa = 16,
                        shadowdensitya = 1;
            
    string     shadownameb = "";
            ENUM        shadowfiltb = FILT_BOX;
            float       shadowblurb = 0,
                        shadowbiasb = 0,
                        shadowsamplesb = 16,
                        shadowdensityb = 1;
            
    string     shadownamec = "";
            ENUM        shadowfiltc = FILT_BOX;
            float       shadowblurc = 0,
                        shadowbiasc = 0,
                        shadowsamplesc = 16,
                        shadowdensityc = 1;
            
    string     shadownamed = "";
            ENUM        shadowfiltd = FILT_BOX;
            float       shadowblurd = 0,
                        shadowbiasd = 0,
                        shadowsamplesd = 16,
                        shadowdensityd = 1;
            
    
) {
    uniform string rcsInfo =
"$Id: k3d_superkagee.sl,v 1.1 2006/02/25 20:11:54 tshead Exp $";
    varying float fullShad = 0;
    varying float shadowed;
    if (shadownamea != "") {
        
        shadowed = clamp(shadowdensitya,0,1) * do_shadow6(shadownamea,P,
                    shadowfilta,shadowblura,shadowsamplesa,shadowbiasa
                    
                    );
        fullShad = max(fullShad,shadowed);
    }if (shadownameb != "") {
        
        shadowed = clamp(shadowdensityb,0,1) * do_shadow6(shadownameb,P,
                    shadowfiltb,shadowblurb,shadowsamplesb,shadowbiasb
                    
                    );
        fullShad = max(fullShad,shadowed);
    }if (shadownamec != "") {
        
        shadowed = clamp(shadowdensityc,0,1) * do_shadow6(shadownamec,P,
                    shadowfiltc,shadowblurc,shadowsamplesc,shadowbiasc
                    
                    );
        fullShad = max(fullShad,shadowed);
    }if (shadownamed != "") {
        
        shadowed = clamp(shadowdensityd,0,1) * do_shadow6(shadownamed,P,
                    shadowfiltd,shadowblurd,shadowsamplesd,shadowbiasd
                    
                    );
        fullShad = max(fullShad,shadowed);
    }
    Oi = Os;
    Ci = Oi * mix(Cs,shadColor,fullShad);
}

