/* Renamed to PQslateroofd.sl for RMR -- talrmr@SpamSucks_pacbell.net */

/*
  slateroofd.sl - a displacement shader to produce tiles on a roof
  
 
    	Parameters:
 
    	Km: Maximum displacement of tiles;
    	sfreq: number of tiles on the s direction
    	tfreq: number of tiles in the t direction
 		maxadd: the maximum amount of overlap of one tile and another
 		ramp: the amount of a tile used for the initial rise to the maximum height
 			a value between 0 and 1
 		gap: the size of the gap between one tile and the next in the s direction,
 			measured as a proportion of the tile (so the value of gap must lie between 0 
 		and 1)
 		factor: a seed used for adjusting the randomness - if you have two roofs with
 			the same sfreq, tfreq change the value of factor to ensure they have a 
 			different random patterning. A value > 20 works best
 	
 		Description
 
		This routine produces a random tiled effect. In the s direction the
		tiles are regular, with every other row offset by half a tile width, as
		normal roof tiles would be. Each tile is expanded by a random amount in
		the t direction so that it overlaps the tile 'below' it. No randomness
		is incorporated in the s direction, which remains regular. The routine
		first determines the dimensions of the current tile, and then works out
		the surface displacement according to a simple formula: a sharp ridge at
		the bottom of the tile, gaps to either side and the whole tile gently
		sloping towards zero at the top of the tile. It then looks to see if the
		point we are considering is actually in the zone of overlap with the
		tile 'above' the current one. If it is we replace the displacement we are
		considering with one calculated for the tile above, being a little
		careful to ensure there are no sudden jumps in displacement.

		Peter Quint 17/11/2000
 
 */


 displacement
k3d_slateroofd(  float 	Km = 1.0,
                   	sfreq = 10,
                   	tfreq = 10,
							maxadd = .5,
							ramp = .1,
							gap = .05,
							factor = 22.838)
/* sfreq and tfreq must not be below 3 */				   

{
   uniform float swidth = 1 / sfreq, 		/* Tile width in the s directio */
                 twidth = 1 / tfreq, 		/* ditto for the t direction */
                 offset = swidth / 2;		/* the amount by which alternate rows are offset */
   float scoord = s, tcoord = 1 - t; 		/* re map the t coord so that the coving is at the top */
   float stile, ttile, 							/* An integer identifying the current tile */
			s_offset, t_offset, 					/* Offsets withing the current tile, between 0, 1 */
			newstile, newscoord, 				/* Used in calculations to identify an overlapping tile */
			cs, ct; 									/* Coordinates set to the centre of the tile and fed into the noise function */
   float add,	 									/* Add, between 0 and 1, hold the amount the current tile overlaps the next */
			news_offset, 
			disps, dispt, disp, dispo,      	/* Displacements - s direction, t direction, final displacement, and 
										 				 	displacement for the non-overlapping tile */
			temp_s_offset, 
			temp_t_offset, newadd;			 
		   
	normal Nf = normalize(N);
	float spacescale = length(vtransform("shader", Nf));
   vector Ndisp = Nf * (1 / max(spacescale,1e-6));
	
   


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

	if (ttile < tfreq - 1)
		{
   	/* the noise function is always taken at the same point in the tile 
   		normally the center, except in the short tiles which lie at the
			start and end of an offset row, where the middle of the left side 
			of the tile is used */
   		ct = ttile + .5;
   		cs = stile + .5;
   		if ((mod(ct,2) >= 1)  && ((stile == 0) || (stile == sfreq)))
			/* An offset row, so we must deal with the half tiles */
				cs = stile;
   		add = noise(cs * factor, ct * factor) * maxadd;
 		}
	else
 		/* the very bottom row cannot expand */
		add = 0;
	/* now calculate the displacement */
	temp_t_offset = t_offset / (1 + add); 
	/* the above adjusts t_offset so that it runs from 0 to 1 within the expanded tile */
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
	disp = dispo;
		   

	/* Work out whether the point is in fact in another tile 	
		storing the new tile in stile, ttile and new offset
		in s_offset, t_offset */
	if ((t_offset <= maxadd) && (ttile >= 1))
		{
			/* the point might be in another tile */
			if (mod(ttile,2) >= 1)
				/* We are in an offset tile row adjust the scoord appropriately for
		 			the row below */
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
			if (t_offset <= newadd)
			/* if t_offset <= newadd we are actually in the overlapping tile */
				{
					/* set parameters to overlapping tile */
					ttile = ttile - 1;
					stile = newstile;
					t_offset = t_offset + 1;
					s_offset = news_offset;
					add = newadd;
				}
			/* calculate the displacement again */
			t_offset = t_offset / (1 + add); 
			if (1 - t_offset <= ramp / (1 + add))
				{
					dispt = Km / 2 + smoothstep(0, ramp / (1 + add), (1 - t_offset)) * Km / 2;
				}
			else
				dispt = Km / 2 +  smoothstep( 0, 1 - ramp, t_offset) * Km / 2;
			if (s_offset > .5)
				s_offset = 1 - s_offset;
			if (s_offset < gap / (1 + add))
				disps = Km * smoothstep(0, gap / (1 + add), s_offset);
			else
				disps = Km;
			disp = min(disps, dispt);
			disp = max(dispo,disp);
			/* taking the maximum of dispo, disp ensures that there
   			are no discontinuities */

		}
	P += Nf * (disp / max(spacescale, 1e-6));
	/*printf("disp = %f\n",disp);*/
   N = calculatenormal(P);
}

