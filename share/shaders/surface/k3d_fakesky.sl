/*
 * fakesky.sl
 *
 * Shader a la const that slaps a fixed blue color up on the top half of a
 * sphere, with color varying a bit by altitude, in an attempt to mimic the
 * clear sky.
 */
	
surface k3d_fakesky(vector up = (0,0,1); color skycolor = (.5, .6, 1.)) {
    Oi = Os;
    Ci = Os * (.5 + .5 * max(0., (up . normalize(N)))) * 1.8 * skycolor;
}

