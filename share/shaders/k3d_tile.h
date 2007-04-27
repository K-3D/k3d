/* 	tile.h
		a file to produce an antaliased tiling pattern
*/



/* this function considers a square wave pulse, with a given period and width,
the width being expressed as a proportion of the period, and so varying between 0 and 1 */
float integratedpulse (					float x, pulseperiod, pulsewidth;
												output	float which, offset)
	{
		float where = x / pulseperiod;
		which = floor(where);
		offset = where - which;
		/* sum up the contribution of all tiles except the present */
		float basetotal = which * pulsewidth;
		float thistile = offset - min(max(0, offset - pulsewidth / 2), 1 - pulsewidth);
		return (thistile + basetotal) * pulseperiod;
	} 

/*
Given the dimensions of a series of tiles, works out which tile we are in, and the
coordinates within that tile (normalized to the range 0 - 1). It returns a float
representing a filtered calculation of the extent the current micropolygon lies
within an edge (1 = all within edge, 0 = all within tile).
Note that the parameters sedge, tedge give the total width of the edge, so that e.g there
will be an edge of width sedge / 2 on both the left and right of the tile 
*/

float tile(		float ss,									/* current s texture coordinate */
										tt, 								/* current t texture coordinate */
										swidth, 						/* width of tile in s direction */
										twidth, 						/* width of tile in t direction */
										sfiltwidth, 				/* size of antalias filter in s direction */
										tfiltwidth, 				/* size of antalias filter in t direction */
										sedge, 							/* tile edge in t direction, as a proportion of tile size (0 - 1)*/
										tedge;							/* tile edge in s direction, as a proportion of tile size (0 - 1)*/
							output float 	swhich,			/* s coordinate of which tile we are in (an integer) */
													 	twhich,			/* t coordinate of which tile we are in (an integer) */
													 	soffset,		/* s offset within tile (0 - 1) */
														toffset)		/* t offset within tile (0 - 1) */
	{
		float sintegral = integratedpulse(ss + sfiltwidth, swidth, sedge, swhich, soffset); 	
		float tintegral = integratedpulse(tt + tfiltwidth, twidth, tedge, twhich, toffset); 	
		sintegral = abs(sintegral - integratedpulse(ss, swidth, sedge, swhich, soffset)) 
			/sfiltwidth;
		tintegral = abs(tintegral - integratedpulse(tt, twidth, tedge, twhich, toffset)) 
			/tfiltwidth;
		/*printf("soffset = %f\tsfiltwidth = %f\tsintegral = %f\n",soffset,sfiltwidth, sintegral); */
		return max(sintegral, tintegral);
	}

