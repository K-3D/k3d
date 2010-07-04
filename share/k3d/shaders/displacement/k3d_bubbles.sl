/*
 * FEbubbles.sl
 * 
 * A simple semi-sphere carpet displacement
 * Felipe Esquivel  felipeer@hotmail.com
 * Nov 2000
 * 
 */
displacement k3d_bubbles(float radius = 0.5;   /* values between -0.5 and 0.5          */
                       float Kmag = 1;       /* scale                                */
                       float frequency = 1;) /* number of bubbles per linear surface */

{
 float distance2point;
 point myCenter = (0, 0, 0);
 float magnitud = 0;
 
 float smod = mod((s) * frequency, 1) - 0.5;
 float tmod = mod((t) * frequency, 1) - 0.5;
 point myPoint = (smod, 0, tmod);
 distance2point = distance(myPoint, myCenter);
 if (distance2point <= radius) 
    magnitud = sqrt((radius * radius) - (distance2point * distance2point));

 P += Kmag * magnitud * normalize(N);
 N = calculatenormal(P);
}
