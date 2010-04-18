/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.368 */
/* Listing 16.25  Surface shader to make a cylinder look like a filament */

/*
 * filament(): map a filament-like spiral onto the surface of a cylinder.
 */

#include "k3d_patterns.h"

surface k3d_filament ( 
	float frequency	= 5.0;
	float phase = 0.0;
	float width = 0.3;
	)
{
	float offset = mod((t*frequency + s + phase), 1.0);

	Oi = filteredpulse(0.5 - width / 2, 0.5 + width / 2, offset, filterwidth(offset));
	Ci = Cs * Oi;
}

