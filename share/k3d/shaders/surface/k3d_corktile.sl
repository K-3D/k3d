/* Renamed to PQcorktile.sl for RMR -- talrmr@SpamSucks_pacbell.net */

/* corktile.sl - a surface shader imitating cork tiling
 *
 * Author: Peter Quint
 * Last Update: 16 October 2000
 *
 * parameters
 *
 * Ka, Kd, Ks, roughness, specularcolor - take their usual meanings
 * Ksgap - the specular component for tile gaps : by default 0.3 * Ks
 * txtscale - the amount of detail / magnification of the image, this also determines
 *   (if tiles=1) the number of tiles fitted into the texture as s and t vary from 0 to 1
 * tiles - the number of tiles in a unit square of texture before txtscale is applied
 *	 (this means the number of tiles actually seen is the square of txtscale * tiles)
 * gap - the size of the gap or groove between tiles, measured as a proportion of the tile,
 *	 thus a gap of .05 will mean that 2.5% of the tile around each edge will be taken up with
 *   the groove (2.5% either side = 5% in total = 0.05)
 * bumpheight - the total height for bump mapped bumps - irregularities on the surface plus
 *   the displacement of the tile itself out of the gap
 * tileheight - the proportion of the bumpheight that is taken up with the rise of the tile from
 * 	 the gap between the tiles, the remainder of bumpheight is taken up with the bumps on the 
 *   surface of the tile. So if tileheight = 1 there are no bumps on the tile surface, if
 *   tileheight = 0 there are bumps on the tile surface of height tileheight, but no difference
 *   in height between the gap and the tile itself
 *
 * The shader makes an attempt at being self antaliasing
 *
 */

#include "k3d_noises.h" /* the standard BMRT include file */
#include "k3d_tile.h"

/* Bias function - Perlin and Hoffert 1989 */

float
bias (float b, x)
{
	return pow(x, log(b)/log(0.5));
}

color cork(		point PP;
							float txtscale, filtwidth)
	{
		/* 	the pattern is built up of the sum of two offset noise functions, perturbed slightly
		and used to select a color from a spline, combined with an overlay of the dark 
		markings that characterise cork
		*/
		color Ct;
		point P2 = transform("shader",PP);
		point PT = P2 * txtscale;
		float fw = filtwidth * txtscale;
		float g;
		float f = fnoise(PT, fw) + fnoise((P2 + 0.553) * txtscale, fw); 
		f = f * fnoise(PT * .854, fw);
		f = f + (fnoise(PT * 5.342, fw) - .5) * .5;  
		f = clamp(f, 0, 1);
		g = fBm((P2 + 8.654) * txtscale, fw, 4, 2, .5);
		Ct = color spline( 	f,
						color( 0, 0, 0),
						color( .25, 0, 0),
						color( .31, .2, 0),
						color( .486, .165, .059),
						color( .47, .18, .09),
						color( .64, .24, .07),
						color( .749, .416, .208)
						); 
		/* Overlay the dark markings */
		if (g > .2)
			{
				g = bias(0.8,(g - .2) / .8);
				Ct = Ct * (1 - g * .65);
			}
		return Ct;
	}

surface
k3d_corktile( 	float	Ka = 1;
          	float	Kd = .75;
         		float	Ks = .3;
						float Ksgap = -1;
         		float	roughness = .05;
	 					float	txtscale = 1;
						float tiles = 1;
						float gap = .05;
						color specularcolor = 1;
						float bumpheight = 0.3;
						float tileheight = .8;
						float bumpfreq = 0.8;)

{
	uniform float Ksg = Ksgap > 0 ? Ksgap : Ks * .3; 
	uniform float tilewidth = 1 / (tiles * txtscale); 
	float disp, adjust;
	point PP = transform("shader",P);
	float filtwidth = filterwidthp(PP),
			swidth = abs(Du(s)*du) + abs(Dv(s)*dv),
			twidth = abs(Du(t)*du) + abs(Dv(t)*dv),
			stfiltwidth = max(swidth, twidth) / tilewidth,
			sw, tw, so, to;
	color Cg, Ct;
	normal Nf;
	float ingap = tile(s, t, tilewidth, tilewidth, swidth, twidth, gap, gap, sw, tw, so, to);
	
	Ct = cork( P, txtscale, filtwidth);	
	/* Calculate color of gap between tiles */
	Cg = color( .20, 0, 0) + filteredsnoise(PP * txtscale, filtwidth * txtscale) * .2;
	so = so > .5 ? 1 - so : so;
	to = to > .5 ? 1 - to : to; 
	adjust = (1 - min(smoothstep(gap * .3, gap * .7, so), smoothstep(gap * .3, gap * .7, to))) * 
		(1  - smoothstep(.2 * gap, gap * .8, stfiltwidth));  
	disp = (tileheight + fnoise(PP * txtscale * bumpfreq + 3, filtwidth * txtscale) * (1 - tileheight)) 
		* bumpheight * (1 - adjust);
	Nf = faceforward(calculatenormal(P + disp *	normalize(N)), I);
	Nf = normalize(Nf);
	/* printf("Nf = %p\n", Nf);	 */
	Oi = Os;
	Ci = Os * (mix( Ct, Cg, ingap) * (Ka*ambient() + Kd*diffuse(Nf)) +
		specularcolor * Ks*specular(Nf,-normalize(I),roughness)); 
	}
