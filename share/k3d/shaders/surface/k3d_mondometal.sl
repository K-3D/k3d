/* MondoMetal.sl   Felipe Esquivel, july 2001
 * felipe@siggraph.org.mx
 * 
 * Layered shader with mondo as a base and a metal modulated with a matte file.
 *
 * Version ampliada de Mondo.sl. Recibe un archivo B/N como mate, donde
 * las partes blancas producen un efecto metalico y donde lo oscuro 
 * se comporta igual que mondo simple.
 */

surface
k3d_mondometal(
    string abColorMap = "";
    float abColorMapBlur = 1;
    float abUseColorMapAlpha = 0;
    float abDiffuse = .8;
    string abDiffuseMap = "";
    float abDiffuseMapBlur = 1;
    float abSpecular = .3;
    string abSpecularMap = "";
    float abSpecularMapBlur = 1;
    color abSpecularColor = color(1,1,1);
    string abSpecularColorMap = "";
    float abSpecularColorMapBlur = 1;
    float abRoughness = .3;
    string abRoughnessMap = "";
    float abRoughnessMapBlur = 1;
    string abTransparencyMap = "";
    float abInvertTransparencyMap = 0;
    float abTransparencyMapBlur = 1;
    color abIncandescence = color(0,0,0);
    string abIncandescenseMap = "";
    float abIncandescenseMapBlur = 1;
    string abReflectionMap = "";
    float abReflectionUp = 0;
    float abReflectivity = .1;
    float abReflectionMapBlur = 1;
    string abBumpMap = "";
    float abBumpMapBlur = 1;
    float abBumpScale = 1.0;
    float abDoDisplacement = 0.0;
    float abUseNormals = 0;
    float Ka = 0.05;
    float Kd = 0.80;
    float Ks = 0.80;
    float roughness = 0.25;
    color Cmetal = color(0.0,0.76,0.79);
    string metalMatte = "";
    point STMatrix0 = point "shader" (1,0,0);
    point STMatrix1 = point "shader" (0,1,0);
)
{




    extern vector I;

    normal Nf;
    vector V, D;
    point PP;
    color Csurf, Cspec, Copac, Cincand, Crefl;
    color Cmatte = 0;
    float ss, tt, roughness, diff, spec, bmp;

    /* apply STMatrix to texture coordinates **/
    ss = vector(s, t, 1) . vector(transform("shader", STMatrix0));
    tt = vector(s, t, 1) . vector(transform("shader", STMatrix1));
    
    /* determine color **/
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
            /* need to un-premultiply the color in this case */
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

    /* determine diffuse **/
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
    
    /* determine specular */
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
    
    /* determine specular color */
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
    /* determine roughness */
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
    
    /* determine opacity */
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
    
    /* determine incandescence */
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
    
    
    /* do the bump */
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
            P = PP; /* usually it's better to do displacements in displacement shader */
    }
    else
    {
        Nf = N;
    }

    /* compute shading variables **/
    Nf = faceforward(normalize(Nf), I , normalize(Nf));
    V = -normalize(I);

    /* get reflection from environment map **/
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

    /* Averiguamos si la textura es metálica o no */
    if(metalMatte != "")
    {
        Cmatte = color texture(metalMatte, ss, tt,
                            "swidth", abColorMapBlur,
                            "twidth", abColorMapBlur);
    }


 
    /* now add it all together */
    Oi = Os * Copac;
    
    /* Textura normal de Mondo*/
    if(comp(Cmatte, 1) < 0.5){
      Ci = Os * Copac * (Csurf * (Cincand + ambient() + diff * diffuse(Nf)) + 
           (spec * Cspec * (specular(Nf, V, roughness) + Crefl) ));
    }
    /* Textura metálica */
    else{
      Ci = Cmetal * Oi * (Ka * ambient() + Kd * diffuse(Nf) +
            Ks * specular(Nf,-normalize(I),roughness));
    }
}



