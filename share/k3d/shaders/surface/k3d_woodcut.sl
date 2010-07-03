/*
 * 2006-03-13 dan@3-e.net
 *
 */

#include "k3d_rmannotes.h"
#include "k3d_noises.h"
  
 surface k3d_woodcut (
string up="y"; 
float ringscale=5; 
float contrast=0.3; 
float brightness=0.75; 
float random=0; 
float nsize=5; ) { 

color surface_color, layer_color;
color surface_opac, layer_opac, Cr;
float fuzz = 0.3;
float stripemin, stripemax, tt;
float y, z, i, n, ns;
float tfreq = 50;

color illumcolor = 0.0;
float illumination = 0.0;
float Ka = 0.10;
float Kd = 0.70;
float Ks = 0.20;
vector Nf, V; 
point Psh;
float roughness = 0.2;

Psh = transform("shader", P);

Nf = faceforward( normalize(N), I );
V = -normalize(I); 

if (up == "z") { 
z = zcomp(Psh);
z *= ringscale;
tt = mod(z,1);
}
else {
y = ycomp(Psh);
y *= ringscale;
tt = mod(y,1);
}

illumcolor = (Ka*ambient() + Kd*diffuse(Nf) + Ks*specular(Nf,V,roughness));
illumination = max(max(comp(illumcolor, 0), comp(illumcolor, 1)), comp(illumcolor, 2));

n = 0;
ns = nsize;
for (i = 0; i < 6.0; i += 1.0) {

n += snoise(Psh * ns) / ns;
ns *= 2.17;
}

illumination = (illumination * contrast) + (1.0 - contrast)/2.0 + 
(brightness - 1.0) + n*random;

if (illumination < 0.01) {
surface_color = color (0.0, 0.0, 0.0);
layer_color = 0.0; 
}
else if (illumination > 0.99) {
surface_color = color (1.0, 1.0, 1.0);
layer_color = 1.0;
}
else {
surface_color = color (0.0, 0.0, 0.0);
layer_color = 1.0;
}
surface_opac = 1.0;


stripemin = 0.5 - smoothstep(0.0, 1.0, illumination)/2.0;
stripemax = 0.5 + smoothstep(0.0, 1.0, illumination)/2.0;
float val = pulse(stripemin, stripemax, fuzz, tt);
layer_opac = color val;
surface_color = blend(surface_color, layer_color, layer_opac);
float color_val = mix(0.0, 1.0, val);
Cr = spline (color_val,
color (0.0, 0.0, 0.0),
color (0.1, 0.05, 0.0),
color (0.8, 0.6, 0.4),
color (1.0, 1.0, 0.8),
color (1.0, 1.0, 1.0),
color (1.0, 1.0, 1.0));
 float spacescale = length(vtransform("shader", normalize(N)));
 vector Ndisp = normalize(N) * (0 / max(spacescale,1e-6));
 P += 0 * Ndisp;
 N = normalize (calculatenormal (P + (1-0)*Ndisp)); 
 Ci = surface_opac * Cr; Oi = 1; }

