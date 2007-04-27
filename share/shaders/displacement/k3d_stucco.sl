/*
 * stucco.sl -- displacement shader for stucco
 *
 * DESCRIPTION:
 *   Displacees a surface to make it look like stucco.
 * 
 * PARAMETERS:
 *   Km 	   	the amplitude of the stucco pimples
 *   power	   	controls the shape of the pimples
 *   frequency  	the frequency of the pimples
 *
 *
 * AUTHOR: written by Larry Gritz (lg@bmrt.org)
 *
 */

displacement k3d_stucco(float Km = 0.05, power = 5, frequency = 10;)
{
  float magnitude;
  point PP;

  PP = transform("shader", P);
  magnitude = Km * pow(noise(PP * frequency), power);
  P += magnitude * normalize(N);
  N = calculatenormal(P);
}
