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

#include <k3dsdk/basic_math.h>
#include <k3dsdk/bezier.h>
#include <k3dsdk/ibezier_channel.h>
#include <k3dsdk/vectors.h>

#include <iostream>

int main(int argc, char* argv[])
{
	const k3d::point2 first(0.0, 0.0);
	const k3d::point2 last(1.0, 1.0);
	
	k3d::ibezier_channel::control_points_t control_points;
	control_points.push_back(first);
	control_points.push_back(k3d::Lerp(first, last, 0.2));
	control_points.push_back(k3d::Lerp(first, last, 0.8));
	control_points.push_back(last);	
	
	for(int i = 0; i <= 10; ++i)
	{
		double error;
		unsigned long iterations;
		double value = k3d::bezier_function<3, k3d::point2>(control_points.begin(), control_points.end(), i * 0.1, i * 0.1, 0.00001, 100, error, iterations);

		std::cout << i * 0.1 << ": " << value << " in " << iterations << " iterations" << std::endl;
	}
	
	return 0;
}

