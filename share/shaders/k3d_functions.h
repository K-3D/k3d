/* k3d_functions.h
 * 
 * various functions collected from k3d_*.sl files
 *
 * add #include "k3d_functions.h" to your .sl files to use
 */
 
 
 /*
 	* Basic Math
 	*/
 	
#define sqr(x) ((x)*(x))
 
#define isOdd(x) ((mod(x,2) == 1) ? 1 : 0)  /* also in rmnotes as odd(x) even(x) */
	
	/*
	 * finters steps and pulses
	 */
	 
#define boxstep(a,b,x) clamp(((x)-(a))/((b)-(a)),0,1) /* also in rmnotes, but all that code will end up here anyway. */
  
#define smoothPulse(a, b, fuzz, loc)  (smoothstep (a-fuzz, a+fuzz, loc) - smoothstep (b-fuzz, b+fuzz, loc) )

/* separate fuzzes */
#define smoothPulse2Fuzz(a, b, afuzz, bfuzz, loc)   (smoothstep (a-afuzz, a, loc) - smoothstep (b, b+bfuzz, loc))

	
	
 
 
 
 