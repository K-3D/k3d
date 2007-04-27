/*
 * bluescreen.sl
 *   puts a background color behind an image.
 *
 * PARAMETERS:
 *   background - the color of the background
 */

imager k3d_bluescreen (

color background = color(0.0, 0.0, 1.0); 
string texturename = ""
)
{
float resolution[3];
float lenx, leny;
color tex;

    if (option("Format", resolution) == 1.0) {
       lenx = xcomp(P)/ resolution[0];
       leny = ycomp(P)/ resolution[1];
    }

    if (texturename != "") {
	tex = texture(texturename, lenx, leny);
	Ci+=(1-alpha)*  tex;
    }
    else
       Ci+=(1-alpha)*background;
    Oi=1;
}
