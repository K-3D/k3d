/* Renamed to PQslateroof.sl for RMR -- talrmr@SpamSucks_pacbell.net */

/* 
	slateroof.sl - a surface shader working with slateroofd.sl, to
	introduce a slate color onto roof tiles, making the last row of tiles
	transparent (and black) so that the edge of the roof looks irregular.
	You must bear this in mind when modelling - to avoid the roof ending
	before it reaches the wall which supposedly supports it! The shader
	introduces a random color variation so that one tile will be lighter or
	darker than the next Parameters:
 
   Ka: Coefficient of ambient light;
 	Kd: Coefficient of diffuse light;
   sfreq: number of tiles on the s direction
   tfreq: number of tiles in the t direction
 	maxadd: the maximum amount of overlap of one tile and another
 	ramp: the amount of a tile used for the initial rise to the maximum height
 		a value between 0 and 1
 	gap: the size of the gap between one tile and the next in the s direction,
 		measured as a proportion of the tile (so the value of gap must lie between 0 
 		and 1)
 	maxcolorvary: the maximum amount by which the color of a slate can vary from the 
 		average
 	txtscale: a scaling factor for the slate coloration - the larger txtscale the
 		the smaller the detail on the tile pattern
 	factor: a seed used for adjusting the randomness - if you have two roofs with
 		the same sfreq, tfreq change the value of factor to ensure they have a 
 		different random patterning. A value > 20 works best
 		

 		
 				
 		
 	Peter Quint 17/11/2000


	Nb. This shader would normally be used with the slateroofd shader to
	produce an appropriate displacement and message passing from the
	displacement shader would eliminate the need for a lot of the
	calculations in this shader. Renderdc, however, does not yet support
	message passing, so to produce the test image the code has been
	duplicated here 
*/



surface
k3d_slateroof(  float 	Ka = .2,
 							Kd = .8,
                   	sfreq = 10,
                   	tfreq = 10,
				   		maxadd = .5,
							ramp = .1,
							gap = .05,
							maxcolorvary = .2,
							factor = 22.238,
							txtscale = 1)
	/* sfreq and tfreq must not be below 3 */				   

{
   uniform float swidth = 1 / sfreq, 		/* Tile width in the s directio */
                 twidth = 1 / tfreq, 		/* ditto for the t direction */
                 offset = swidth / 2,		/* the amount by which alternate rows are offset */
					  Km = 1;
   float scoord = s, tcoord = 1 - t; 		/* re map the t coord so that the coving is at the top */
	float stile, ttile, 							/* An integer identifying the current tile */
			s_offset, t_offset, 					/* Offsets withing the current tile, between 0, 1 */
			newstile, newscoord, 				/* Used in calculations to identify an overlapping tile */
			cs, ct; 									/* Coordinates set to the centre of the tile and fed into the noise function */
   float add, 										/* Add, between 0 and 1, hold the amount the current tile overlaps the next */
			news_offset, 
			disps, dispt, disp, dispo,      	/* Displacements - s direction, t direction, final displacement, and 
										  					displacement for the non-overlapping tile */
			temp_s_offset, 
			temp_t_offset, newadd;			 
   uniform 	float colorfactor = 3.276 * factor;	/* Used in noise calculations */	
   color 	Ct = 0;								/* A temporary color variable */
	float 	colorvary = 0,				    	/* the random amount by which a tile is lightened or darkened */
				newcolorvary, ocolorvary, ns;


	normal Nf = normalize(faceforward(N,I));


	/* Work out which tile we are in, and the offset within that tile */
   ttile = tcoord / twidth;
	if (mod(ttile,2) >= 1)
      scoord = scoord + offset; /* Displace alternate rows */
   stile = scoord / swidth;
   s_offset = stile - floor(stile);
   t_offset = ttile - floor(ttile);
   stile = floor(stile);
   ttile = floor(ttile);

	/* Work out the displacement assuming the point is in the 
   current tile */


	/* the noise function is always taken at the same point in the tile 
	 * normally the center, except in the short tiles which lie at the
	 * start and end of an offset row, where the middle of the left side 
    * of the tile is used */
	ct = ttile + .5;
	cs = stile + .5;
	if ((mod(ct,2) >= 1)  && ((stile == 0) || (stile == sfreq)))
	/* An offset row, so we must deal with the half tiles */
	cs = stile;
	if (ttile < tfreq - 1)
		add = noise(cs * factor, ct * factor) * maxadd;
	else 
		add = 0;
	ocolorvary = (noise(cs * colorfactor, ct * colorfactor) - 1) * 2 * maxcolorvary;
	colorvary = ocolorvary;
	/* now calculate the displacement */
	temp_t_offset = t_offset / (1 + add); 
	if (1 - temp_t_offset <= ramp / (1 + add))
   	dispt = Km / 2 + smoothstep(0, ramp / (1 + add), (1 - temp_t_offset)) * Km / 2;
   else
	   dispt = Km / 2 + smoothstep( 0, 1 - ramp /(1 + add), temp_t_offset) * Km / 2;
	if (s_offset > .5)
	   temp_s_offset = 1 - s_offset;
	else
		temp_s_offset = s_offset;
	if (temp_s_offset < gap * (1 + add))
	   disps = smoothstep(0, gap * (1 + add), temp_s_offset) * Km;
	else
   	disps = Km;
	dispo = min(disps, dispt);

	   

	/* Work out whether the point is in fact in another tile 
	 * storing the new tile in stile, ttile and new offset
	 * in s_offset, t_offset */
	if ((t_offset <= maxadd) && (ttile >= 1))
		{
			/* the point might be in another tile */
			if (mod(ttile,2) >= 1)
			/* We are in an offset tile row adjust the scoord appropriately for
			 * the row below */
				newscoord = scoord - offset;
			else
				newscoord = scoord + offset;
			newstile = newscoord / swidth;
			news_offset = newstile - floor(newstile);
			newstile = floor(newstile);
			/* Now calculate cs, ct the reference point for the noise calculation */					
			ct = ttile - .5;
			cs = newstile + .5;
			if ((mod(ct,2) >= 1) && ((newstile == 0) || (newstile == sfreq)))
			/* An offset row, so we must deal with the half tiles */
				cs = newstile;
			newadd = noise(cs * factor, ct * factor) * maxadd;
			newcolorvary =  (noise(cs * colorfactor, ct * colorfactor) - 1) * 2 * maxcolorvary;
			if (t_offset <= newadd)
				/* if t_offset <= newadd we are actually in the overlapping tile */
				{
					/* set parameters to overlapping tile */
					ttile = ttile - 1;
					stile = newstile;
					t_offset = t_offset + 1;
					s_offset = news_offset;
					add = newadd;
					colorvary = newcolorvary; 
				}
		}
	/* calculate the displacement again */
	t_offset = t_offset / (1 + add); 
	if (1 - t_offset <= ramp / (1 + add))
			dispt = Km / 2 + smoothstep(0, ramp / (1 + add), (1 - t_offset)) * Km / 2;
		else
		    dispt = Km / 2 + smoothstep( 0, 1 - ramp /(1 + add), t_offset) * Km / 2;
	if (s_offset > .5)
		s_offset = 1 - s_offset;
	if (s_offset < gap / (1 + add))
		disps = Km * smoothstep(0, gap / (1 + add), s_offset);
	else
		disps = Km;
	disp = min(disps, dispt);
	if (dispo > disp)
		colorvary = ocolorvary;
	/*printf("#dispo = %f, disp = %f, colorvary = %f\n",dispo,disp,colorvary); */

	ns = noise( s * txtscale * 59.8534 * sfreq, t * txtscale * 59.8534 * tfreq);
	Ct = color spline( 	ns, 
			color( .41, .44, .60),
			color( .65, .70, .82),
			color( .39, .31, .35),
			color( .50, .41, .53),
			color( .47, .47, .63));
	Ct = Ct * (1 + colorvary);
	if (ttile < tfreq - 1) 
		Oi = 1;
	else
		{
			Oi = 0;
			Ct = 0;
		};
	Ci = Ct * (Ka * ambient() + Kd * diffuse(faceforward( normalize(N), I )));
	  
}

