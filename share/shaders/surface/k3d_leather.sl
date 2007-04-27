/*
 * JexSkin.sl
 *
 * I have done this shader from JMredapple.sl for RMR
 *
 * add some parameter, change many thing
 *
 *and offer you this shader, it was done (JMredapple.sl) by Jonathan Merritt
 *without hes shader i wouldnt be able to do mine so send him flowers :)
 *
 *	re-autor istarion@3dvf.net    web site http://istarion.3dvf.net
 *
 * 	old-autor jmerritt@warpax.com   :-)
*/


surface
k3d_leather (
	float Ks = .2;           /* Specular reflection coeff.    */
	float Kd = 1;            /* Diffuse reflection coeff.     */
	float Ka = 1;             /* Ambient light coeff.          */
	float Kr = .2;             /* Mirror-like reflection coeff. */
	float roughness = .1;     /* Specular roughness param.     */          
	color red   = color(0.8,0.6,0.4);
	color dred  = color(0.5,0.3,0.10);
	color green = color(0.8,0.6,0.4);
	color brown = color(0.5,0.3,0.10);
	color black = color(0.5,0.3,0.10);
	
	/*BLOTCH sont les zebrure*/
	
	float BLOTCH_TCF = 5;		/*zebrure sur la peau*/
	float BLOTCH_SF = 5;		/*definis le s */
	float BLOTCH_TF = 5;		/*definis le s */
    float paramdisp = 1;
	float txtscale = .05;     	/* Ugly kludge (see above...)    */
	color specularcolor = 1;  	/* Specular reflection color     */

	
)
{
	/*
	 * Shader variables
	 */
	vector Nf, V, Rdir;
	point PP, newP;
	float base_turb, blotch_turb, disp_turb;
	float small_noise, blotch, speck, disp = 0, blackness;
	color cs, small_speckle, base_color, reflect;





	/*
	 * Standard settings
	 */
	PP = transform("shader", P);
	V = normalize(-I);


	/*
	 * Pick the base color for the apple.
	 *
	 * The base color consists of patches of pure green,
	 * and patches of finely speckled red and green. These
	 * are set up so that the poles of the apple (as "t"
	 * goes to 1 or 0) are colored more and more green.
	 *
	 * The apple geometry is such that lines of constant
	 * "t" are lines of longditude (from pole to pole).
	 * The constants T1 - T4 specify locations of the
	 * start of 'greenness' toward the poles.
	 */
	#define BASE_SF          1      /* s-factor for big noise          */
	#define BASE_TF          1      /* t-factor for big noise          */
	#define BASE_NF          100    /* scaling factor for small noise  */
	#define BASE_NOISE_AMP   0.2    /* small noise color mix amplitude */
	#define BASE_GRC         0.4    /* shift factor for more red       */
	#define T1               0.0    /* t < T1 is pure green            */
	#define T2               0.18   /* lerp s.t. T1->T2 => green->red  */
	#define T3               0.8    /* T2->T3 => red can exist here    */
	#define T4               1.0    /* lerp s.t. T3->T4 => red->green  */
	                                /* t > T4 is pure green            */
	base_turb = noise(BASE_SF*sin(2*PI*s) + PI + label,
	                  BASE_TF*t + label);
	base_turb = pow(base_turb, (1/redness));
	small_noise = snoise(BASE_NF*PP*txtscale/TSCALE);
	small_speckle = mix(red, green, BASE_GRC+
	                    (small_noise*BASE_NOISE_AMP));
	if (t < T1)
		base_turb = 0;
	else if (t >= T1 && t < T2)
		base_turb *= (t-T1)/(T2-T1);
	else if (t >= T3 && t < T4)
		base_turb *= (T4-t)/(T4-T3);
	else if (t >= T4)
		base_turb = 0;
	base_color = spline(base_turb, green, green,
	                    small_speckle, small_speckle,
	                    small_speckle);


	/*
	 * Pick the blotch color.
	 *
	 * The blotch color is finally determined by mixing,
	 * using the blotching coefficient "blotch". This
	 * section sets that coefficient, using a funky yet
	 * really simple noise routine.
	 */
	/*#define BLOTCH_SF          20   s-factor for blotches  */
	/*#define BLOTCH_TF          15   t-factor for blotches  */
	/*#define BLOTCH_TCF         0     zebrure                */
	#define BLOTCH_DELTA       0.1  /* 'nother scaling kludge */
	#define BLOTCH_SPECK_COEFF 0.3  /* blotch specking coeff  */
	blotch_turb = noise(BLOTCH_SF*sin(2*PI*(s+.1234)) + PI + label,
	                    BLOTCH_TF*t + label) + BLOTCH_DELTA;
	blotch = pow(blotch_turb, 1.3) * BLOTCH_TCF * 
	         pow(base_turb, 2);
	blotch = blotch * (1+small_noise) +
	         small_noise * BLOTCH_SPECK_COEFF;
	if (blotch > 1) blotch = 1;


	/*
	 * Set the brown specking.
	 *
	 * The apple has brown speckles on it, set by mixing
	 * using the speckling coefficient "speck". These
	 * specks are very dot-like in nature, and this
	 * dottiness technique is stolen from LG's starfield 
	 * shader.
	 */
	#define SPECK_NF        0     /* un dented dessus que je met a zero*/    
	#define SPECK_CUTOFF    .45   /*Cutoff for 'tops' of specks*/ 
	speck = pow(smoothstep(SPECK_CUTOFF, 1,
	            noise(SPECK_NF*PP*txtscale/TSCALE)), 3);

	/*
	 * Determine where the apple goes black at the poles.
	 * This simulates the spots where the stalk would be
	 * attached, and where the wierd bit at the very bottom
	 * is.
	 */
	#define BEDGE  .0
	#define BWIDTH .0
	blackness = 1-(smoothstep(BEDGE, BEDGE+BWIDTH, t)*
	               smoothstep(BEDGE, BEDGE+BWIDTH, 1-t));


	/*
	 * Combine what we have so far to set the surface
	 * color.
	 */
	cs = mix(base_color, dred, blotch);
	cs = mix(cs, brown, speck);
	cs = mix(cs, black, blackness);


	/*
	 * Set the shading surface normal.
	 *
	 * Here we set the surface normal to fix up the specular
	 * highlights. We'd like them perturbed by the small
	 * noise, affected a little by the brown specks, and
	 * also dented a bit (alas, no real apples are perfectly
	 * un-dented).
	 */
	#define DISP_SF     10             /* s-factor for dent noise */
	#define DISP_TF     30             /* t-factor for dent noise */
	#define DISP_DENT_AMP     (1/15)   /* dent amplitude          */
	#define DISP_SMNOISE_AMP  (1/1000) /* small noise amplitude   */
	#define DISP_SPECK_AMP    (1/40)   /* speckle disp. amplitude */
	disp += noise(DISP_SF*sin(2*PI*s) + PI + label,
	              DISP_TF*t + label) * DISP_DENT_AMP * txtscale / TSCALE;
	disp += small_noise * DISP_SMNOISE_AMP * txtscale / TSCALE;
	disp -= speck * DISP_SPECK_AMP * txtscale / TSCALE;
	disp = paramdisp * disp;
	newP = calculatenormal(P + disp * normalize(N));
	Nf = faceforward(normalize(newP), I);


	/*
	 * 'Mirror' reflections
	 *
	 * Here, we raytrace for the slight mirrored reflections
	 * in the surface of an apple. They don't add much, but
	 * may be needed for the 'perfect' apple :-).
	 *
	 * Note: You'll need Larry Gritz's raytrace helper files
	 * for this bit!
	 */
#ifdef USE_LG_RAYTRACE
	if (Kr > .01) {
		Rdir = normalize(reflect(normalize(I), Nf));
		reflect = RayTrace(P, Rdir, 0, 1, 1);
	} else {
		reflect = 0;
	}
#else
	reflect = 0;
#endif


	/*
	 * Combine everything to get Ci, in the standard form.
	 */
	Oi = Os;
	Ci = Os * (cs * (Ka*ambient() + Kd*diffuse(Nf)) +
	           specularcolor * (Kr * reflect +
	                            Ks * specular(Nf, V, roughness)));
		
}



