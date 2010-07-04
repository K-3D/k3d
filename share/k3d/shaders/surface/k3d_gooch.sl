/* Renamed to MKgooch.sl for RMR -- tal@SpamSucks_cs.caltech.edu */

/* s_gooch.sl - a simple implementation of the Gooch
 *              non-photorealistic lighting model
 *
 * DESCRIPTION   : This model is described in "A Non-Photorealistc
 *                 Lighting Model For Automatic Technical 
 *                 Illustration" in the Siggraph 1998 Proceedings.
 *                 http://www.cs.utah.edu/~gooch/SIG98/abstract.html
 *
 * INPUTS        : 
 *  Ka           : ambient factor
 *  Kd           : diffuse factor
 *  Ks           : specular factor
 *  alpha        : Gooch cool color factor
 *  beta         : Gooch warm color factor
 *  b            : used to construct the cool color
 *  y            : used to construct the warm color
 *  roughness    : specular roughness
 *  specularcolor: specular hilight color
 *
 * AUTHOR: written by Mike King
 *               
 ***************************************************/

surface
k3d_gooch (float Ka = 0;
         float Kd = 1;
         float Ks = 0;
         float alpha = .25;
         float beta = .5;
         float b = .55;
         float y = .3;
         float roughness = .1;
         color specularcolor = 1;)
{
    normal Nf = faceforward (normalize(N),I);
    color blue = color(0,0,b);
    color yellow = color(y,y,0);

    color Cgooch = color(0,0,0); 
    float ldotn, blendval;
    color kcool,kwarm;

    kcool = blue + ( Cs * alpha);
    kwarm = yellow + (Cs * beta);
    illuminance(P,Nf,PI) {
        ldotn = (normalize(L)).Nf;
        blendval = 0.5*(1+ldotn);
        Cgooch += mix(kcool,kwarm,blendval); 
    }
    
    Oi = Os;
    Ci = Os * ((Ka*ambient() + 
                Kd*Cgooch +
                specularcolor*Ks*specular(Nf,-normalize(I),roughness)));
}


