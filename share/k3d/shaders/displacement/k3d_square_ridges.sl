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

/// Filtering code courtesy of the Advanced RenderMan book ... where else?

#define MIN_FILTER_WIDTH 1.0e-6
#define filter_width(x) max(abs(Du(x)*du) + abs(Dv(x)*dv), MIN_FILTER_WIDTH)

/*
float pulse(float edge0, edge1, x)
{
	return step(edge0, x) - step(edge1, x);
}

float pulse_train(float edge, period, x)
{
	return pulse(edge, period, mod(x, period));
}
*/

float filtered_pulse_train(float edge, period, x, dx)
{
	float w = dx / period;
	float x0 = x/period - w/2;
	float x1 = x0 + w;
	float nedge = edge / period;
	
	float integral(float t)
	{
		extern float nedge;
		return ((1 - nedge) * floor(t) + max(0, t-floor(t)-nedge));
	}
	
	return (integral(x1) - integral(x0)) / w;
}

displacement k3d_square_ridges(
	float Km = 1.0;
	float Frequency = 8.0;
	float Offset = 0.25;
	)
{
	float ridge_position = filtered_pulse_train(0.5 / Frequency, 1.0 / Frequency, t + (Offset / Frequency), filter_width(t));
//	float ridge_position = pulse_train(0.5 / Frequency, 1.0 / Frequency, u, filter_width(u));

	vector Nn = normalize(N);
	P += Nn * ((Km * ridge_position) / length(vtransform("shader", Nn)));
//	P += Km * ridge_position * N;
	N = calculatenormal(P);
}


