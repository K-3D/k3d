

surface k3d_hilltop (

float Ka=0; 
float Kd=.8; 
color ambientcolor8=1; 
color aColor3=color(0,0,0); 
color aColor6=color(0.45,0.58,0.32); 
float TopDir=1; 
string geospace="shader"; 
float Altitude=0.8; 
float Blend=.7; 
float octaves2=4; 
float freq2=10; ) 
{
/** Surface main-code start **/
matrix ident=1;
point PP2=point(0.0,0.0,0.0); 
/** Turbulence_Brick_2 **/
point temp_PP2;
temp_PP2=transform("shader",P);
uniform float i2;
float size2,sum2;
sum2 = 0;
size2 = 1;
for (i2=0;i2<octaves2;i2+=1)
{
sum2 += noise(temp_PP2*freq2*size2)/size2;
size2 *= 2;
}
float temt_f2=clamp(sum2-0.4,0,1);
float Result;
normal Nn;
Nn = ntransform(geospace, N);
Nn = normalize(Nn);
if (TopDir == 0)
Result = xcomp(Nn) + clamp(temt_f2,0,1);
else
if (TopDir == 1)
Result = ycomp(Nn) + clamp(temt_f2,0,1);
else
if (TopDir == 2)
Result = zcomp(Nn) + clamp(temt_f2,0,1);

Result = (Result + 1)/2;
Result = 1 - smoothstep(Altitude, Altitude+(1-Altitude)*Blend, Result);
normal Nf8 = faceforward (normalize(N),I);

/** Surface main-code end **/

Ci = ambientcolor8 * Ka * ambient() + (Result*aColor3+(1-Result)*aColor6) * Kd * diffuse(Nf8);
Oi = color((1-(Result)));
}
