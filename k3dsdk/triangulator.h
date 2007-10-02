#ifndef K3DSDK_TRIANGULATOR_H
#define K3DSDK_TRIANGULATOR_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "types.h"

namespace k3d
{

class mesh;
class point3;

class triangulator
{
public:
	triangulator();
	~triangulator();

	void process(const mesh& Mesh);

private:
	void add_vertex(const point3& Coordinates, uint_t Vertices[4], double_t Weights[4], uint_t& NewVertex);
	void add_triangle(const uint_t Point1, const uint_t Point2, const uint_t Point3);

	virtual void on_add_vertex(const point3& Coordinates, uint_t Vertices[4], double_t Weights[4], uint_t& NewVertex);
	virtual void on_add_triangle(const uint_t Point1, const uint_t Point2, const uint_t Point3);
	virtual void on_begin();

	friend class implementation;
	class implementation;
	implementation* const m_implementation;
};

} // namespace k3d

#endif // K3DSDK_TRIANGULATOR_H

