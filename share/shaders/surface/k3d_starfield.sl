/* DESCRIPTION:
	Makes a star field.  Best when used as a surface shader for the inside	of a large sphere.
	
	We partition space into a 1x1x1 grid, rendering one solid spherical "star" per cell.

	PARAMETERS:

	AUTHOR: written by Timothy M. Shead
*/

surface k3d_starfield(float intensity = 2.0; float frequency = 0.1; float size = 0.3; float irregularity = 2.0)
{
	// Work in object coordinates ...
	point PP = transform("object", frequency * P);
	
	// Get the nearest star ...
	point star_center = point(0.5 * (floor(xcomp(PP)) + ceil(xcomp(PP))), 0.5 * (floor(ycomp(PP)) + ceil(ycomp(PP))), 0.5 * (floor(zcomp(PP)) + ceil(zcomp(PP))));

	star_center += irregularity * (point noise(star_center) - point(0.5, 0.5, 0.5));

	// Calculate the distance to the nearest star ...
	float star_distance = distance(PP, star_center);
	
	float inside_star = 1 - smoothstep(0.0, size, star_distance);
	
	// Give stars relative intensities ...
	float star_intensity = float cellnoise(star_center);
	
	Ci = intensity * star_intensity * inside_star * inside_star;
}


