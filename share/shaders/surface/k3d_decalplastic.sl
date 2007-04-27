// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\author Tim Shead (tshead@k-3d.com)
*/

surface k3d_decalplastic(
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float roughness = 0.1;
	color specularcolor = 1.0;
	string texturename = "";
	float s0 = 0.0;
	float s1 = 1.0;
	float t0 = 0.0;
	float t1 = 1.0;
	)
{
	color Ct = Cs;
	if(texturename != "")
		{
			float ss = mix(s0, s1, s);
			float tt = mix(t0, t1, t);
			float alpha = float texture(texturename[3], ss, tt);
			Ct = (1.0 - alpha) * Cs + (alpha * color texture(texturename, ss, tt));
		}

	normal Nf = faceforward(normalize(N), I);
	vector V = -normalize(I);
	Oi = Os;
	Ci = Oi * Ct * (Ka * ambient() + Kd * diffuse(Nf)) + specularcolor * Ks * specular(Nf, V, roughness);
}

