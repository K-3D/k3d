/*
 * EM_paper.sl -- loose leaf paper
 *
 * DESCRIPTION:
 *   Makes a patch look like a piece of loose-leaf paper with lines,
 *   the margin, binder holes and writing (as an image texture).
 *   Works in s/t space.
 *
 * PARAMETERS:
 *   Ka, Kd, Ks, specular, roughness - work just like the plastic shader
 *   linestr - how strongly do the lines show up (0.0 to 1.0)
 *   texturename - name of image to map onto paper (optional)
 *
 * ANTIALIASING: no antialiasing. This can be a real problem because of the
 *               thin lines on the paper.
 *
 * AUTHOR: written by Emil Mikulic
 *         email: darkmoon@SpamSucks_connexus.apana.org.au
 *
 * HISTORY:
 *    8 Nov 1997 - started writing shader for RDC under MS VC++
 *    9 Nov 1997 - added binder holes and finished shader
 *   15 Nov 1997 - translated to Renderman SL in Linux
 *
 * last modified 15 Nov 1997 by Emil Mikulic
 *
 * COMMENTS:
 *   Don't worry about the paper anatomy settings. I moved them to
 *   the paper(...) block so that they get calculated on initialisation
 *   (initcode) instead of being re-calced for every sample.
 *
 *   I don't know how many holes there were in paper when _YOU_ were a kid
 *   but in my day there's 7. I actually got a piece of loose-leaf paper
 *   and got all the measurements with a ruler. This is the real thing!!
 *
 *   The paper shader works for A4 pieces of paper, so remeber to make your
 *   patch 21x29.7 units or at least with an approximate aspect ratio. 
 *
 */

surface
k3d_ruledpaper(float Ka = 1, Kd = 0.5, Ks = 0.5;
	float roughness = 0.1;
	color specularcolor = 1;
	float linestr = 1;
	string texturename = "";
    
	/* Paper anatomy settings */
	// Paper width
	float 	pw=21,
	// Paper height 
		ph=29.7,
	// Width of plastic strip
 		shiny=1.25/pw,
	// Spacing between lines
		lines = 0.7 / ph,
	// Thickness of lines
		linet = 0.05 / ph,
	// How far into the page do the lines start
		linef = 0.5 / pw,
	// Top and bottom margins for lines
		margt = 2 / ph,
		margb = 1 - (1.35 / ph),
	// Where does the vertical line (margin) start and end?
		verts = 2.75 / pw,
		verte = 2.85 / pw,
	// Settings for binder holes (circles)
	// Circle center along page (ss coordinate system)
		circms = 1.25,
	// Circle radius in ss/tt coord. system
		circr = 0.3,
	// Bounding box for circles
		boundl = (circms-circr)/pw,
		boundr = (circms+circr)/pw;	)
{

// Fancy macro checks ss/tt coords against bounding box and [if inside]
// calculates if it's inside the circle. If it is, the colour
// becomes black and the opacity becomes 0.

#define circle(sm,tm) { \
	if ((tt>(tm-circr)/ph) && (tt>(tm+circr)/ph))	\
	{						\
		if (distance( point(ss,tt,0), point(sm,tm,0) ) <= circr)	\
			Oi = color(0,0,0);		\
	}						\
}

    point Nf, V;
    color tex;
    float ss,tt;

    // For plastic and diffuse shading
    Nf = faceforward(normalize(N), I);
    V = normalize(-I);
	
	// Check if there's a texture given.
	// Use a nice handwrititng font and you can make it look like
	// there's writing on the paper.
	if (texturename != "")
		tex = color texture(texturename);
	else
		tex = Cs; // Colour of paper
	
	// If we're inside the borders/margins
	if ( (s>linef) && (t>margt) && (t<margb) )
	// Figure out where in the lining we are and draw accordingly
	if ( (mod(t, lines)<=linet) || ((s>=verts) && (s<=verte)) )
		tex = (tex*(1-linestr)) + (color(0.2,0.8,0.8) * linestr);
	
	Oi = Os;

	// Bounding box for circle calcs
	if ((s>boundl) && (s<boundr)) 
	{
		// Figure out where we are on the paper so that we can get
		// accurate, aspecty circles.
		ss = s * pw;
		tt = t * ph;

		// Find bounding boxes first to speed up calcs
		// I wrote this handy macro to automate it!
		circle(circms,2.7);
		circle(circms,4.05);
		circle(circms,10.7);
		circle(circms,14.75);
		circle(circms,18.75);
		circle(circms,25.45);
		circle(circms,26.75);
	}

	// Check if it actually needs shading
	if (Oi == color (0, 0, 0)) { Ci = 0; } else {
		if (s<=shiny) {
			// If inside plasticky reinforcement strip then
			// use plastic shading model.
			Ci = Oi * (tex * (Ka * ambient() + Kd * diffuse(Nf)) + 
			specularcolor * Ks * specular(Nf, V, roughness));
		} else {
			// Else shade diffuse paper
			Ci = Oi * tex * (Ka * ambient() + Kd * diffuse(Nf));
		};
	}
}
