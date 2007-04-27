/* I took wave's lead and renamed plastic to DPPlastic.sl -- tal@SpamSucks_cs.caltech.edu */

/* 
 * plastic.sl
 *
 * AUTHOR: Darwyn Peachy
 *
 * REFERENCES:
 *    _Texturing and Modeling: A Procedural Approach_, by David S. Ebert, ed.,
 *    F. Kenton Musgrave, Darwyn Peachey, Ken Perlin, and Steven Worley.
 *    Academic Press, 1994.  ISBN 0-12-228760-6.
 */
 
surface
k3d_plastic2(float Ka = 1, Kd = 0.5, Ks = 0.5;
        float roughness = 0.1;
        color specularcolor = color (1,1,1))
{
    point Nf = faceforward(normalize(N), I);
    point V = normalize(-I);

    Oi = Os;
    Ci = Os * (Cs * (Ka * ambient() + Kd * diffuse(Nf))
         + specularcolor * Ks * specular(Nf, V, roughness));
}
