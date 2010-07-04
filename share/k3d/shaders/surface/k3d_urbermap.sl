

surface k3d_urbermap (

string abColorMap=""; 
float abColorMapBlur=1; 
float abUseColorMapAlpha=0; 
float abDiffuse=0.8; 
string abDiffuseMap=""; 
float abDiffuseMapBlur=1; 
float abSpecular=0.3; 
string abSpecularMap=""; 
float abSpecularMapBlur=1; 
color abSpecularColor=color(1.00,1.00,1.00); 
string abSpecularColorMap=""; 
float abSpecularColorMapBlur=1; 
float abRoughness=0.3; 
string abRoughnessMap=""; 
float abRoughnessMapBlur=1; 
string abTransparencyMap=""; 
float abInvertTransparencyMap=0; 
float abTransparencyMapBlur=1; 
color abIncandescence=color(0.00,0.00,0.00); 
string abIncandescenseMap=""; 
float abIncandescenseMapBlur=1; 
string abReflectionMap=""; 
float abReflectionUp=0; 
float abReflectivity=0.1; 
float abReflectionMapBlur=1; 
string abBumpMap=""; 
float abBumpMapBlur=1; 
float abBumpScale=1; 
float abDoDisplacement=0; 
float abUseNormals=0; 
point STMatrix0=point(1,0,0); 
point STMatrix1=point(0,1,0); ) 
{
/** Surface main-code start **/

normal Nf;
vector V, D;
point PP;
color Csurf, Cspec, Copac, Cincand, Crefl;
float ss, tt, roughness, diff, spec, bmp;


ss = vector(s, t, 1) . vector(transform("shader", STMatrix0));
tt = vector(s, t, 1) . vector(transform("shader", STMatrix1));


if(abColorMap != "")
{
Csurf = Cs * color texture(abColorMap, ss, tt,
"swidth", abColorMapBlur,
"twidth", abColorMapBlur );
if(abUseColorMapAlpha != 0)
{
Copac = float texture(abColorMap[3], ss, tt,
"swidth", abColorMapBlur,
"twidth", abColorMapBlur );

Csurf = Csurf / Copac;
}
else
{
Copac = color(1, 1, 1);
}
}
else
{
Csurf = Cs;
Copac = color(1);
}


if(abDiffuseMap != "")
{
diff = abDiffuse * float texture(abDiffuseMap, ss, tt,
"swidth", abDiffuseMapBlur,
"twidth", abDiffuseMapBlur );
}
else
{
diff = abDiffuse;
}


if(abSpecularMap != "")
{
spec = abSpecular * float texture(abSpecularMap, ss, tt,
"swidth", abSpecularMapBlur,
"twidth", abSpecularMapBlur );
}
else
{
spec = abSpecular;
}


if(abSpecularColorMap != "")
{
Cspec = abSpecularColor * color texture(abSpecularColorMap, ss, tt,
"swidth", abSpecularColorMapBlur,
"twidth", abSpecularColorMapBlur );
}
else
{
Cspec = abSpecularColor;
}


if(abRoughnessMap != "")
{
roughness = abRoughness * float texture(abRoughnessMap, ss, tt,
"swidth", abRoughnessMapBlur,
"twidth", abRoughnessMapBlur );
}
else
{
roughness = abRoughness;
}


if(abTransparencyMap != "")
{
if(abInvertTransparencyMap != 0)
{
Copac = Copac * color texture(abTransparencyMap, ss, tt,
"swidth", abTransparencyMapBlur,
"twidth", abTransparencyMapBlur );
}
else
{
Copac = Copac * (color(1) - color texture(abTransparencyMap, ss, tt,
"swidth", abTransparencyMapBlur,
"twidth", abTransparencyMapBlur ));
}
}


if(abIncandescenseMap != "")
{
Cincand = abIncandescence * color texture(abIncandescenseMap, ss, tt,
"swidth", abIncandescenseMapBlur,
"twidth", abIncandescenseMapBlur );
}
else
{
Cincand = abIncandescence;
}



if( abBumpMap != "" )
{
bmp = abBumpScale * float texture( abBumpMap, ss, tt,
"swidth", abBumpMapBlur,
"twidth", abBumpMapBlur );
PP = transform("shader", P);
Nf = normalize( ntransform("shader", N) );
PP += bmp * Nf;
PP = transform("shader", "current", PP);
Nf = calculatenormal(PP);

if (abUseNormals == 1) {
normal deltaN = normalize(N) - normalize(Ng);
Nf = normalize(Nf) + deltaN;
}

if( abDoDisplacement == 1.0 )
P = PP;
}
else
{
Nf = N;
}


Nf = faceforward( normalize(Nf), I );
V = -normalize(I);


if( abReflectionMap != "" )
{
D = reflect(-V, Nf);
D = vtransform("worldspace", D);
if( abReflectionUp != 0 )
{
D = vector(-zcomp(D), xcomp(D), ycomp(D));
}
Crefl = abReflectivity *
color environment(abReflectionMap, D,
"swidth", abReflectionMapBlur,
"twidth", abReflectionMapBlur );
}
else
Crefl = color(0);

/** Surface main-code end **/

Ci = Csurf * (Cincand + ambient() + diff * diffuse(Nf)) + (spec * Cspec * (specular(Nf, V, roughness) + Crefl));

}
