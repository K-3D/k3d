

#include "k3d_filterwidth.h"
#include "k3d_project.h"
#include "k3d_displace.h"
#include "k3d_material.h"
#include "k3d_reflections.h"
#include "k3d_locillum.h"
#include "k3d_noises.h"

color altMaterialPlastic (normal Nf;  color basecolor,selftcolor,Rcolor,RFcolor;
                       float Ka, Kd, Ks, roughness,selft,Kr,Krfr;)
{
    extern vector I;
    return (basecolor * ( Ka*ambient() + Kd*diffuse(Nf)+selft*selftcolor ) )
	   + Kr*Rcolor+Krfr*RFcolor
     + Ks*specular(Nf,-normalize(I),roughness);
}

color
myEnvironment (point P;  vector R;  float Kr, blur;  DECLARE_ENVPARAMS;)
{
    color C = 0;
    float alpha;
    if (envname != "") {
        if (envspace == "NDC")
            C = ReflMap (envname, P, blur, alpha);
        else
            C = Environment (envname, envspace, envrad, P, R, blur, alpha);
    }
    return C;
}

float myrand(point p;)
{
	float x,hx;
	x=abs(xcomp(p));
	x+=abs(ycomp(p));
	x+=abs(zcomp(p));

	float n=log(x,10);
	n-=6;
	n=round(n);
	x=x/pow(10,n);
	hx=mod(x,1000000)/1000;
	x=mod(x+hx+x*100,1000);
	return x/1000;
}

float tex_clouds(point P;float depth)
{
	float val=0;
	float i,arm=1;
	float Karm=1;

	for(i=0;i<depth;i=i+1)
	{
		//val=val*(1-Karm)+Karm*snoise(arm*P);
		val=val+Karm*snoise(arm*P);
		Karm*=0.8;
		arm*=2;
	}
	return clamp((val+1)/2,0,1);
}

float tex_wood(point PP; float ringscale)
{
	float grainy = 1;

    float my_t = zcomp(PP) / ringscale;
    point PQ = point (xcomp(PP)*8, ycomp(PP)*8, zcomp(PP));
    my_t += noise (PQ) / 16;

    PQ = point (xcomp(PP), my_t, ycomp(PP)+12.93);
    float r = ringscale * noise (PQ);
    r -= floor (r);
    r = 0.2 + 0.8 * smoothstep(0.2, 0.55, r) * (1 - smoothstep(0.75, 0.8, r));
    PQ = point (xcomp(PP)*128+5, zcomp(PP)*8-3, ycomp(PP)*128+1);
    float r2 = grainy * (1.3 - noise (PQ)) + (1-grainy);

	return clamp(r*r2*r2,0,1);
}

color colorMap(string mapname, space; 
		float scalex, scaley, scalez, octaves, blur;)
{
	point transp;
	color newc;

	transp=transform(space,P)*vector (scalex, scaley, scalez);

	if (mapname=="clouds")
		newc=tex_clouds(transp,octaves);
	else if (mapname=="noise")
		newc=myrand(transp);
	else if (mapname=="wood")
		newc=tex_wood(transp,octaves);
	else 
	{
		newc=texture(mapname,s*scalex,t*scaley,"blur",blur,"fill",-1,"width",0);
    if (comp(newc,1)==-1) newc=comp(newc,0); /* treat a 1-channel texture map as a greyscale */
	}
			  return newc;
}

color BlendColor(string mode; float K; color newc, oldc)
{
  color res=oldc;
  if (mode=="m") res+=K*newc;
  if (mode=="f") res*= (1-K)*(color 1)+K*newc;
	return res;
}

float BlendFloat(string mode; float K, newf, oldf)
{
  float res=oldf;

 if (mode=="m") res=(res+K*newf)/(1+K);
 if (mode=="f") res*= ((1-K)+K*newf);

	return res;
}

surface
k3d_texblender (float Ka = 1, Kd = .5, Ks = .2, roughness = .3, Kr=0,selft=0;
		float Rblur=0.1,RFblur=0.1;
		float Krfr=0,eta=0.8,Ko=1;

		float cKmap[5]={0,0,0,0,0};
		float oKmap[5]={0,0,0,0,0};
		float sKmap[5]={0,0,0,0,0};
		float mKmap[5]={0,0,0,0,0};
		float bKmap[5]={0,0,0,0,0};
		float rKmap[5]={0,0,0,0,0};
		float iKmap[5]={0,0,0,0,0};
		float aKmap[5]={0,0,0,0,0};
		float nGmap=0;
		float Kcs=1;
		float Sgmx[5]={1,1,1,1,1},Sgmy[5]={1,1,1,1,1},Sgmz[5]={1,1,1,1,1};
		float Depth[5]={3,3,3,3,3};
		float Gblur[5]={0,0,0,0,0};
		string Gmapname[5]={"","","","",""};
		string Gspace[5]={"object","object","object","object","object"};
		string Gmode[5]={"m","m","m","m","m"};

		DECLARE_DEFAULTED_ENVPARAMS;
    )
{
    color Ct = Kcs*Cs, Ot = Os*Ko,Cselft=Kcs*Cs;
    float ks = Ks,kr=Kr;
    float disp = 0,i,Alpha=1;
    vector V ,D,badN,corr,dispDir,ndir;

    normal Nf =normalize( faceforward(normalize(N),I));
    dispDir=normalize( faceforward(normalize(Ng),I));
    corr=Nf-dispDir;

	for(i=0;i<nGmap;i=i+1)
	{
		
		color MC=colorMap(Gmapname[i],Gspace[i],Sgmx[i],Sgmy[i],Sgmz[i],
											Depth[i],Gblur[i]);
    float MF=(comp(MC,0)+comp(MC,1)+comp(MC,2))/3; // when we want a float
		string mode=Gmode[i];
		
		if((Alpha*cKmap[i])!=0) Ct=BlendColor(mode,Alpha*cKmap[i],MC,Ct);
		if((Alpha*oKmap[i])!=0) Ot=BlendColor(mode,Alpha*oKmap[i],MC,Ot);
		if((Alpha*sKmap[i])!=0) ks=BlendFloat(mode,Alpha*sKmap[i],MF,ks);
		if((Alpha*mKmap[i])!=0) kr=BlendFloat(mode,Alpha*mKmap[i],MF,kr);
		if((Alpha*rKmap[i])!=0) roughness=BlendFloat(mode,Alpha*rKmap[i],
                                                 MF,roughness);
		if(aKmap[i]!=0) Alpha=BlendFloat("f",aKmap[i],MF,1);
		if((Alpha*iKmap[i])!=0) selft=BlendFloat(mode,Alpha*iKmap[i],MF,selft);
		if((Alpha*bKmap[i])!=0) disp=BlendFloat(mode,Alpha*bKmap[i],MF,disp);
	}
	if(disp!=0)
	{
			ndir=normalize(corr+Displace(dispDir,"shader",disp,0));
			ndir=normalize(ndir-(Nf*(ndir.Nf)));
			ndir=normalize(Nf+ndir);
			ndir=normalize(ndir-(Nf*(ndir.Nf)));
			Nf+=disp*(normalize(ndir));
			Nf=normalize(Nf);
	}

	color env=0,benv=0;
	float olds=raysamples;
	V = normalize(I);
	
	if((kr!=0) && (((Nf.V)>=0) || (raylevel()==0)) ) 
	{
		if(Rblur==0) raysamples=1;
		env=myEnvironment(P,normalize(reflect(V,Nf)),1,Rblur,ENVPARAMS);
	}
	if(Krfr!=0) 
	{
		if(RFblur==0) raysamples=1;
		else raysamples=olds;
		benv=myEnvironment(P,refract(V,Nf,(V.Nf > 0) ? 1.0/eta : eta),1,RFblur,ENVPARAMS);
	}
    Ci = altMaterialPlastic (Nf,Ct,Cselft,env,benv,
				Ka,Kd,ks,roughness,selft,kr,Krfr);
    Oi = Ot;  Ci *= Oi;
}

