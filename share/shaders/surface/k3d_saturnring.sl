/*
 * TLRing.sl -- surface for a saturn like ring to be used on a disk
 *
 * DESCRIPTION:
 *	When put on a disk will give a "saturn-like ringed" apearence with
 * varing colors and transparency.
 *
 * PARAMETERS:
 *	Ka, Kd - the usual
 *	cutoff - what point to start rings (radius of transparency)
 *	ringrad - radius of ring
 *	opacity - the opacity of the rings (may not be used anymore)
 *
 * HINTS:
 *	The default values assume that the disk has a radius of one.  If it is
 *  otherwise then they will neeb to be changed.
 *
 * AUTHOR: Tal Lancaster
 *	tal@SpamSucks_cs.caltech.edu
 *
 * History:
 *	Created: 5/15/95
 */

#define RING1 0.83    /* Relative spacing for outermost ring */
#define RING2 0.77    /* Relative spacing for next outermost ring */    
#define RING3 0.62    /* Relative spacing for   "  outermost ring */
#define RING4 0.58    /* Relative spacing for   "  outermost ring */
#define RING5 0.55    /* Relative spacing for   "  outermost ring */

#define snoise25(x) (2.5 * (noise(x) - 1)) /* why this variation of snoise? */

 
surface k3d_saturnring (float Ka = 1.0, Kd = 1.0,
	cutoff = 0.55, ringrad = 1.0, opacity = 0.5;)
{
	point PP;       /* Transformed point */
	point Nf;       /* Forward facing Normalized vector of incident light */
	float val;      /* length of PP */
	float relpos;   /* relative position of PP on disk 
	                               (a percentage distance) */
	float oi = 0.0; /* Opacity holder */
	color cs;	    /* color holder */
	color dgrey = color (.266, .266, .266);      /* A shade of dark grey */
	color dyellow = color (.73, .664, .398);     /* A shade of dark yellow */
	color dpink = color (.664, .465,  .465);     /* A shade of dark pink */
	color mutedgreen = color (.531, .531, .398); /* A shade of muted green */
	
	PP = transform ("shader", P);
	val= length (PP);
	 
#define DEBUG 0
#if DEBUG	
	printf ("val %f ringrad %f \n",
	 val,  ringrad);
#endif
	 
	if (val < cutoff * ringrad) {
		/* Creating an inner disk that is transparent to place the planet */
		Oi = 0.0;	
	}
	else {
		/* Create rings of varing transparency */
		relpos =  val / ringrad;
		oi =  (relpos + snoise25 (40* relpos) - floor(relpos) );
		
#define DEBUG2 0
#if DEBUG2
		printf ("oi = %f\n", oi);
#endif

		/* Create some gaps of completely transparent rings */
		if (oi > 1.0)
			oi = 0.0;
	}
	
	
	if (oi == 0.0)
		Oi = 0.0;
		
	else {

		if (relpos >  RING1) 
			cs = dgrey;
		else if (relpos >  RING2) {
			/* Want transparent section */
			oi = 0.0;
			Oi = 0.0;
		}
		else if (relpos > RING3) {
			cs = dyellow;	
		}
		else if (relpos > RING4)
			cs = dpink;	
		else 
			cs = mutedgreen;	

		if (oi != 0.0) {
			Oi = oi;
			/* A Matte model */
			Nf = faceforward (normalize(N), I);
			Ci = Oi * (cs * (Ka*ambient() + Kd*diffuse(Nf) ));
		}
	}
}
