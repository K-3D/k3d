#ifdef RCSIDS
static char rcsid[] = "$Id: brickbump.sl,v 1.1.1.1 2002/02/10 02:35:49 tal Exp $";
#endif

/*
 * brickbump.sl -- displacement shader for bricks.
 *
 * Description:
 *   Makes displacements for a wall of bricks.  This is the companion
 *   shader to the surface "brick" shader.  The parameters work exactly
 *   the same.  Of course, you can use it with any surface shader, and
 *   in fact matte or plastic gives those nice white cinder block walls.
 *   However, if you do use it with "brick", the parameters MUST match,
 *   or your bricks will look very strange.
 * 
 * Parameters:
 *    brickwidth                Width of a brick (in st space)
 *    brickheight               Height of a brick (in st space)
 *    mortarthickness           Thickness of the mortar (in st space)
 *    rowvary                   How much does each row shift?
 *    jagged                    How much do bricks deviate from squares?
 *    pitting                   The amplitude of the "pits" on the face of
 *                                 the bricks.
 *    pockfrequency             The st frequency of the pits.
 *    groovedepth               The depth of the grooves between bricks.
 *
 * AUTHOR: written by Larry Gritz, 1992 (and subsequently modified)
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/02/10 02:35:49 $
 *
 */



#include "k3d_noises.h"
#include "k3d_patterns.h"



displacement
brickbump ( float jagged = 0.006;
	    float raggedamp = 0.04, raggedfreq = 12;
            float brickwidth = .28, brickheight = .07;
	    float mortarthickness = .014;
	    float rowvary = .5, pitting = 0.01;
	    float pockfrequency = 10, groovedepth = 0.01; )
{
#define sqr(x) ((x)*(x))
    float sbrick, tbrick;
    float ss, tt;
    float fact, disp;
    uniform float BMWIDTH = (brickwidth+mortarthickness);
    uniform float BMHEIGHT = (brickheight+mortarthickness);
    uniform float MWF = (mortarthickness*0.5/BMWIDTH);
    uniform float MHF = (mortarthickness*0.5/BMHEIGHT);

    basicbrick (s, t, BMWIDTH, BMHEIGHT, 0.5, rowvary, 1, jagged,
	       sbrick, tbrick, ss, tt);

    ss += raggedamp * snoisexy ((s+tbrick*5.15)*raggedfreq,
			       (t+sbrick*23.8)*raggedfreq);
    tt += raggedamp * snoisexy ((s+tbrick*11.4)*raggedfreq,
			       (t+sbrick*7.2)*raggedfreq);
    ss += raggedamp/2 * snoisexy ((s+tbrick*5.15)*raggedfreq*2,
				 (t+sbrick*23.8)*raggedfreq*2);
    tt += raggedamp/2 * snoisexy ((s+tbrick*11.4)*raggedfreq*2,
				 (t+sbrick*7.2)*raggedfreq*2);

    fact = 1;
    disp = 0;
    if (tt < MHF) {
	/* We're in the top horizontal groove */
	disp = groovedepth * (sqr((tt)/MHF) - 1);
    }
    else if (tt > (1.0-MHF)) {
      /* Bottom horizontal groove */
	disp = groovedepth * (sqr((1-tt)/MHF) - 1);
    }
    if (ss < MWF) {
	disp = min (disp, 0.85 * groovedepth * (sqr(ss/MWF) - 1));
    }
    else if (ss > (1.0-MWF)) {
	disp = min (disp, 0.85 * groovedepth * (sqr((1-ss)/MWF) - 1));
    }

    fact = smoothstep (0, 1.3*MHF, tt) - smoothstep (1.0-1.3*MHF, 1, tt);
    fact *= (smoothstep (0, 1.3*MWF, ss) - smoothstep (1.0-1.3*MWF, 1, ss));
    fact = pitting * (0.75 * fact + 0.25);
    disp -= fact * pow(noise ((ss+sbrick)*pockfrequency/BMHEIGHT,
			      (tt+tbrick)*pockfrequency/BMWIDTH), 0.25);

    P += disp * normalize(N);
    N = calculatenormal (P);
}
