
/**************************************************************************/
/**************************************************************************/
/*** DO NOT EDIT THE .SL FILE FOR THIS SHADER *****************************/
/*** INSTEAD EDIT THE .SLM4 FILE WHICH GENERATES THE .SL FILE *************/
/**************************************************************************/
/**************************************************************************/

/*
* To do: accomodate negative widths. twosided/onesided cards.
*/

/*
** $Id: k3d_softboxes.sl,v 1.1 2006/02/25 20:11:54 tshead Exp $
**
** Derived from softbox3 v1.2 - Author Bjorke for all
**
** Full RCS log in .slm4 file
**
**
*/

#define BOOL float
#define ENUM float

#define SHAD_BOX_FILT 0
#define SHAD_GAUSSIAN_FILT 1

#define TEX_GAUSSIAN_FILT 0
#define TEX_BOX_FILT 1
#define TEX_RADIAL_FILT 2
#define TEX_DISK_FILT 3

#define HALFSIZE

#define OBEY_THIS "Reflectivity"

#define CLASSIC 0
#define SPHERICAL 1

#define CUBEFACE 0
#define LATLONG 1



/**************************************************************************/
/* Superellipse soft clipping - straight out of "uberlight"		***/
/* Input:								***/
/*   - point Q on the x-y plane						***/
/*   - the equations of two superellipses (with major/minor axes given	***/
/*      by  a,b and A,B for the inner and outer ellipses, respectively)	***/
/* Returns:								***/
/*   - 0 if Q was inside the inner ellipse				***/
/*   - 1 if Q was outside the outer ellipse				***/
/*   - smoothly varying from 0 to 1 in between				***/
/**************************************************************************/

/* this is the identical function used by sqLight etc */

float clipSuperellipse (
    point Q;          /* Test point on the x-y plane */
    uniform float a, b;       /* Inner superellipse */
    uniform float A, B;       /* Outer superellipse */
    uniform float roundness;  /* Same roundness for both ellipses */
) {
    varying float result;
    varying float x = abs(xcomp(Q)), y = abs(ycomp(Q));
    if (roundness < 1.0e-6) {
	/* Simpler case of a square */
	result = 1 - (1-smoothstep(a,A,x)) * (1-smoothstep(b,B,y));
    } else {
	/* more-difficult rounded corner case */
	varying float re = 2/roundness;		/* roundness exponent */
	varying float q = a * b * pow (pow(b*x, re) + pow(a*y, re), -1/re);
	varying float r = A * B * pow (pow(B*x, re) + pow(A*y, re), -1/re);
	result = smoothstep (q, r, 1);
    }
    return result;
}

/*****************************************************************/
/*** Given info on a softbox, ************************************/
/*****************************************************************/

void softbox_contrib2(
    varying point	surfPt;
    varying vector	reflVect;
    uniform string	boxCoords;
    uniform string	boxTexture;
    uniform float	boxTexStr,
			boxTexBlur,
			boxFilter,
			boxWidth,
			boxHeight,
			boxWEdge,
			boxHEdge,
			boxRoundness;
    uniform color	boxColor,
    			boxOpac; 
    uniform float	decayExp;
    output float theDist;
    output color theColor;
    output color theOpac;
) {
    uniform string filtTypes[4] = {"gaussian","box","radial-bspline","disk"};
    uniform string theFilterName = filtTypes[clamp(boxFilter,0,3)];
    varying float contrib;
    varying color ct = 1;
    varying float ot = 1; 
    /* Get the surface position */
    varying point Pb1 = transform (boxCoords, surfPt);
    varying vector Vlight = vtransform (boxCoords, reflVect);
    varying float zv = zcomp(Vlight);
    varying point Pplane = Pb1 - Vlight*(zcomp(Pb1)/zcomp(Vlight));
#ifdef HALFSIZE
    uniform float bw2 = boxWidth/2;
    uniform float bh2 = boxHeight/2;
#else
#define bw2 boxWidth
#define bh2 boxHeight
#endif
    uniform float we = max(boxWEdge,-bw2);
    uniform float he = max(boxHEdge,-bh2);
    uniform float bW = bw2+we;
    uniform float bH = bh2+he;
    uniform float iW = min(bW,bw2);
    uniform float iH = min(bH,bh2);
    uniform float oW = max(bW,bw2);
    uniform float oH = max(bH,bh2);
    if (sign(zcomp(Pb1)) == sign(zcomp(Vlight))) {
	contrib = 0;
    } else if (abs(zv) < 0.0001) {
	contrib = 0;
    } else {
	contrib = 1 - clipSuperellipse (Pplane, iW, iH, oW,oH, boxRoundness);
	if (boxTexture != "") {
	    uniform float nChans;
	    textureinfo(boxTexture,"channels",nChans);
	    varying float theS = (oW+xcomp(Pplane))/(oW*2);
	    varying float theT = (oH-ycomp(Pplane))/(oH*2);
	    theS = min(2,max(-1,theS));
	    theT = min(2,max(-1,theT));
	    if (nChans>1) {
		ct = texture(boxTexture,theS,theT,
			    "filter",	theFilterName,
			    "blur",	boxTexBlur);
		if (boxTexStr != 1) {
		    ct = (ct*boxTexStr)+(1-boxTexStr);
		}
	    }
	    if ((nChans==1)||(nChans>3)) {
		uniform float alphaChan;
		if (nChans==1) {
		    alphaChan = 0;
		} else {
		    alphaChan = 3;
		}
		ot = float texture(boxTexture[alphaChan],theS,theT,
			    "filter",	theFilterName,
			    "blur",	boxTexBlur);
	    }
	}
    }
#pragma nolint
    varying point ppC = transform(boxCoords,"world",Pplane);
    varying point spw = transform("world",surfPt);
    varying float pDist = length(ppC - spw); /* in "world" coords */
    theDist = pDist;
    theOpac = contrib * boxOpac * ot;
    theColor = contrib * ct * boxColor / pow(pDist,decayExp);	/* premultiplied!!!!! */
}

/**************************************************/
/*** SHADOW ***************************************/
/**************************************************/

float sbShadow(
    uniform string	theName;
    varying point	thePoint;
    uniform ENUM	theFilt;
    uniform float	theBlur,
			theSamples,
			theBias;
) {
    uniform string filtTypes[2] = {"box", "gaussian"};
    uniform string theFilterName = filtTypes[clamp(theFilt,0,1)];
    varying float inShadow = shadow (theName,
			    thePoint,
			    "filter",	theFilterName,
			    "blur",	theBlur,
			    "samples",	theSamples,
			    "bias",	theBias);
    return(inShadow);
}

/*****************************************************************/
/*** MAIN SHADER *************************************************/
/*****************************************************************/

light k3d_softboxes(
    string	Comment = "";
#ifdef BMRT
    string	Tcl = "";
    string	NodeName = "";
#else /* !BMRT -- slc compiler doesn't like these definitions */
    string	Tcl = "[addL]";	
    string	NodeName = "$OBJNAME";
#endif /* BMRT */
    float	intensity	= 1;
    color	lightcolor	= color (1,1,1);
    float	decayRate	= 0;
    float	meterDistance = 1;
    string	meterSpace = "";
    float	edgeRolloff = 0,
    		edgeAngle = 90,
		edgeExp = 1;
    string	boxCoord1	= "";
    color	boxColor1	= color (1,1,1),	/* multiplied by lightcolor&intensity */
		boxOpacity1	= color (1,1,1);
    float	boxWidth1	= 1,
		boxWEdge1	= -0.1,
		boxHeight1	= 1,
		boxHEdge1	= -0.1,
		boxRoundness1	= 0;
    string	boxTex1	= "";
    float	boxFilter1	= TEX_GAUSSIAN_FILT,
		boxTexStr1	= 1,
		boxTexBlur1	= 0;
    string	boxCoord2	= "";
    color	boxColor2	= color (1,1,1),	/* multiplied by lightcolor&intensity */
		boxOpacity2	= color (1,1,1);
    float	boxWidth2	= 1,
		boxWEdge2	= -0.1,
		boxHeight2	= 1,
		boxHEdge2	= -0.1,
		boxRoundness2	= 0;
    string	boxTex2	= "";
    float	boxFilter2	= TEX_GAUSSIAN_FILT,
		boxTexStr2	= 1,
		boxTexBlur2	= 0;
    string	boxCoord3	= "";
    color	boxColor3	= color (1,1,1),	/* multiplied by lightcolor&intensity */
		boxOpacity3	= color (1,1,1);
    float	boxWidth3	= 1,
		boxWEdge3	= -0.1,
		boxHeight3	= 1,
		boxHEdge3	= -0.1,
		boxRoundness3	= 0;
    string	boxTex3	= "";
    float	boxFilter3	= TEX_GAUSSIAN_FILT,
		boxTexStr3	= 1,
		boxTexBlur3	= 0;
    string	boxCoord4	= "";
    color	boxColor4	= color (1,1,1),	/* multiplied by lightcolor&intensity */
		boxOpacity4	= color (1,1,1);
    float	boxWidth4	= 1,
		boxWEdge4	= -0.1,
		boxHeight4	= 1,
		boxHEdge4	= -0.1,
		boxRoundness4	= 0;
    string	boxTex4	= "";
    float	boxFilter4	= TEX_GAUSSIAN_FILT,
		boxTexStr4	= 1,
		boxTexBlur4	= 0;
    uniform string	envTexName = "";
    uniform float	EnvType = CLASSIC;
    uniform float	MapType = CUBEFACE;
    uniform float envTexIntensity	= 1;
    uniform float envTexBlur	= 0;
    uniform float envTexStr	= 1;
    uniform float envTexFilter	= 0;
    uniform string envReflSpace	= "";
    color shadowcolor = 0;
    float shadowintensity = 1;
    string shadowname = "";
    ENUM	shadowfilt = SHAD_BOX_FILT;
    float   shadowblur = 0.01,
	    shadowbias = 0,
	    shadowsamples = 16;
    string shadownameb = "";
    ENUM	shadowfiltb = SHAD_BOX_FILT;
    float   shadowblurb = 0.01,
	    shadowbiasb = 0,
	    shadowsamplesb = 16;
    string shadownamec = "";
    ENUM	shadowfiltc = SHAD_BOX_FILT;
    float   shadowblurc = 0.01,
	    shadowbiasc = 0,
	    shadowsamplesc = 16;
    string shadownamed = "";
    ENUM	shadowfiltd = SHAD_BOX_FILT;
    float   shadowblurd = 0.01,
	    shadowbiasd = 0,
	    shadowsamplesd = 16;
    BOOL	NonDiffuse	= 1;
    BOOL	NonSpecular	= 0;
    BOOL	ObeyMateria	= 0;
    BOOL	UseMateriaBlur	= 0;
    output varying float __nondiffuse = 1;
    output varying float __nonspecular = 0;
    string	__category = "reflection";
    output varying float __inShadow = 0;	   
) {
    uniform string rcsInfo = "$Id: k3d_softboxes.sl,v 1.1 2006/02/25 20:11:54 tshead Exp $";
    uniform string filtTypes[4] = {"gaussian","box","radial-bspline","disk"};
    uniform string theFilterName = filtTypes[clamp(envTexFilter,0,3)];
    normal Nf = faceforward(normalize(N),I);
    vector rv = reflect(I,Nf);
    uniform float edgeLimVal = cos(radians(90-clamp(edgeAngle,0,90)));
    uniform string theEnvSpace;
    if (envReflSpace == "") {
	theEnvSpace = "shader";
    } else {
	theEnvSpace = envReflSpace;
    }
    uniform float adjMeterDistance;
    if (meterSpace == "") {
	adjMeterDistance = meterDistance;
    } else {
	uniform point metP = transform(meterSpace,"shader",point (0,0,0));
	adjMeterDistance = length(metP) + meterDistance;
    }
    uniform float adjIntensity = pow(adjMeterDistance,decayRate)*intensity;
    varying float fullShad = 0;
    uniform float materiaRefl = 1;
    uniform float materiaBlur = 0;

    Cl = 0;
    __nondiffuse = NonDiffuse;
    __nonspecular = NonSpecular;
    if (ObeyMateria > 0) {
	if (surface("Reflectivity",materiaRefl) == 0) {
	    if (surface("abReflectivity",materiaRefl) == 0) {
		if (surface("reflectivity",materiaRefl) == 0) {
		    if (surface("Kr",materiaRefl) == 0) {
			materiaRefl = 0;
		    }
		}
	    }
	}
    }
    if (UseMateriaBlur > 0) {
	if (surface("ReflectionMapBlur",materiaBlur) == 0) {
	    if (surface("abReflectionMapBlur",materiaBlur) == 0) {
		if (surface("reflectionMapBlur",materiaBlur) == 0) {
		    materiaBlur = 0;
		}
	    }
	}
    }
    solar() {
	if (materiaRefl != 0) {
	    uniform float boxCt=0;
	    varying float thisDist;
	    varying color thisOpac;
	    varying color thisColor;
	    varying float sortedDist[4];
	    varying color sortedOpac[4];
	    varying color sortedColor[4];
	    if (boxCoord1 != "") {
	    softbox_contrib2(Ps,rv,
boxCoord1,boxTex1,boxTexStr1,
(boxTexBlur1+materiaBlur),boxFilter1,
		    boxWidth1,boxHeight1,boxWEdge1,boxHEdge1,
		    boxRoundness1,boxColor1,boxOpacity1,decayRate,
		    thisDist,thisColor,thisOpac);
	    sortedDist[boxCt]  = thisDist;
	    sortedColor[boxCt] = thisColor;
	    sortedOpac[boxCt]  = thisOpac;
	    boxCt += 1;
	}
	    if (boxCoord2 != "") {
	    softbox_contrib2(Ps,rv,
boxCoord2,boxTex2,boxTexStr2,
(boxTexBlur2+materiaBlur),boxFilter2,
		    boxWidth2,boxHeight2,boxWEdge2,boxHEdge2,
		    boxRoundness2,boxColor2,boxOpacity2,decayRate,
		    thisDist,thisColor,thisOpac);
	    sortedDist[boxCt]  = thisDist;
	    sortedColor[boxCt] = thisColor;
	    sortedOpac[boxCt]  = thisOpac;
	    boxCt += 1;
	}
	    if (boxCoord3 != "") {
	    softbox_contrib2(Ps,rv,
boxCoord3,boxTex3,boxTexStr3,
(boxTexBlur3+materiaBlur),boxFilter3,
		    boxWidth3,boxHeight3,boxWEdge3,boxHEdge3,
		    boxRoundness3,boxColor3,boxOpacity3,decayRate,
		    thisDist,thisColor,thisOpac);
	    sortedDist[boxCt]  = thisDist;
	    sortedColor[boxCt] = thisColor;
	    sortedOpac[boxCt]  = thisOpac;
	    boxCt += 1;
	}
	    if (boxCoord4 != "") {
	    softbox_contrib2(Ps,rv,
boxCoord4,boxTex4,boxTexStr4,
(boxTexBlur4+materiaBlur),boxFilter4,
		    boxWidth4,boxHeight4,boxWEdge4,boxHEdge4,
		    boxRoundness4,boxColor4,boxOpacity4,decayRate,
		    thisDist,thisColor,thisOpac);
	    sortedDist[boxCt]  = thisDist;
	    sortedColor[boxCt] = thisColor;
	    sortedOpac[boxCt]  = thisOpac;
	    boxCt += 1;
	}
	    if (envTexName != "") {
#pragma nolint
		varying vector Rs = normalize (vtransform (theEnvSpace, normalize(-L)));
		if (EnvType == SPHERICAL) {
#pragma nolint
		    varying point PShd = transform (theEnvSpace, Ps);
		    varying float pl = vector(PShd).vector(PShd);
		    varying float pdotv = -vector(PShd).Rs;
		    Rs = vector( PShd + (pdotv + sqrt (abs (1 - pl + ((pdotv)*(pdotv)))))*Rs );
		}
		if( MapType == LATLONG ) {	/* latlong */
		    Rs = vector (-zcomp (Rs), xcomp (Rs), ycomp (Rs));
		}
		Cl = color environment (envTexName, Rs,
					    "filter", theFilterName,
					    "blur", (envTexBlur+materiaBlur));
		if (envTexStr != 1) {
		    Cl = (Cl*envTexStr)+(1-envTexStr);
		}
		Cl *= envTexIntensity;
	    }
	    if (boxCt > 0) {
		uniform float i, j, k;
		for(k=0; k<(boxCt-1); k+=1) {
		    j = 0;
		    for(i=1; i<boxCt; i+=1) {
			if (sortedDist[i]>sortedDist[j]) { /* farthest first */
			    thisDist = sortedDist[j];
			    thisOpac = sortedOpac[j];
			    thisColor = sortedColor[j];
			    sortedDist[j] = sortedDist[i];
			    sortedOpac[j] = sortedOpac[i];
			    sortedColor[j] = sortedColor[i];
			    sortedDist[i] = thisDist;
			    sortedOpac[i] = thisOpac;
			    sortedColor[i] = thisColor;
			}
			j = j+1;
		    }
		}
		color ttc;
		for(k=0; k<boxCt; k+=1) {
		    ttc = Cl;
		    Cl = sortedColor[k]+(Cl*(1-sortedOpac[k]));
		}
	    }
	    /* Apply shadow mapped shadows */
	    varying vector Ln = normalize(L);
	    varying vector Nn = normalize(N);
	    varying vector In = normalize(I);
	    float theDot = Ln.Nn;
	    if (edgeRolloff > 0) {
		float q;
		q = In.Nn/edgeLimVal;
		q = 1 - clamp(edgeRolloff*pow(clamp(abs(q),0,1),1/max(edgeExp,0.001)),0,1);
		Cl = mix(color(0,0,0),Cl,q);
	    }
	    varying float shadowed;
	    if (shadowname != "") {
	    shadowed = sbShadow(shadowname,Ps,shadowfilt,
			shadowblur,shadowsamples,shadowbias);
	    fullShad = max(fullShad,shadowed);
	}
	    if (shadownameb != "") {
	    shadowed = sbShadow(shadownameb,Ps,shadowfiltb,
			shadowblurb,shadowsamplesb,shadowbiasb);
	    fullShad = max(fullShad,shadowed);
	}
	    if (shadownamec != "") {
	    shadowed = sbShadow(shadownamec,Ps,shadowfiltc,
			shadowblurc,shadowsamplesc,shadowbiasc);
	    fullShad = max(fullShad,shadowed);
	}
	    if (shadownamed != "") {
	    shadowed = sbShadow(shadownamed,Ps,shadowfiltd,
			shadowblurd,shadowsamplesd,shadowbiasd);
	    fullShad = max(fullShad,shadowed);
	}
	    __inShadow = fullShad; 
	}
    }
    if (materiaRefl > 0) {
	Cl *= (lightcolor * adjIntensity * materiaRefl);
	Cl = mix(Cl, (shadowcolor*shadowintensity*adjIntensity), fullShad);
    } else {
	Cl = 0;
    }
}

/*************************************************/
/***************************************** eof ***/
/*************************************************/
