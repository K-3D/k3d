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

#include "k3d_patterns.h"

surface k3d_warningstripes(
	float Ka = 1;
	float Kd = 1;
	float Frequency = 4;
	color StripeColor = color(0, 0, 0)
	)
{
	vector Nf = normalize(faceforward(N, I));
	
	float stripe_position = filteredpulsetrain(0.5 / Frequency, 1.0 / Frequency, u+v, filterwidth(u+v));
	color Ct = mix(Cs, StripeColor, stripe_position);

	Oi = Os;
	Ci = Os * Ct * (Ka * ambient() + Kd * diffuse(Nf));
}


