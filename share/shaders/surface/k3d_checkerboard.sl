#include "k3d_patterns.h"

surface k3d_checkerboard(float Ka = 1.0;
			 float Kd = 1.0;
			 float Ks = 0.5;
			 float roughness = 0.1; color specularcolor = 1.0;
			 uniform float TileS = 6.0; uniform float TileT = 6.0;
			 color EvenColor = 0.0;)
{
  point Nf;
  float tiles, tilet;
  float filters, filtert, filterst;
  float filterwidths, filterwidtht;
  float mixamount;
  color checkcolor;

/*
	// Calculate our "filter zone" around the edges of each tile ...
	filters = mod((s * TileS * 2.0) - 1.0, 2.0);
	filtert = mod((t * TileT * 2.0) - 1.0, 2.0);

	filterwidths = SFILTERWIDTH * TileS;
	filterwidtht = TFILTERWIDTH * TileT;

	filters = filteredpulse(1.0 - filterwidths, 1.0 + filterwidths, filters, filterwidths);
	filtert = filteredpulse(1.0 - filterwidtht, 1.0 + filterwidtht, filtert, filterwidtht);

	filterst = filters + filtert;
*/

  // Calculate which tile we're in ...
  tiles = step(1.0, mod(s * TileS, 2.0));
  tilet = step(1.0, mod(t * TileT, 2.0));

  if(tiles == tilet)
    {
      mixamount = 0.0;
    }
  else
    {
      mixamount = 1.0;
    }

  checkcolor = mix(EvenColor, Cs, mixamount);

  // Diffuse reflection model ...
  Nf = normalize(faceforward(N, I));
  Oi = Os;
  Ci =
    Os * (checkcolor * (Ka * ambient() + Kd * diffuse(Nf)) +
	  specularcolor * Ks * specular(Nf, -normalize(I), roughness));
}
