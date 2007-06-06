// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "mesh_selection_python.h"

#include <k3dsdk/mesh_selection.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

const k3d::mesh_selection::records_t mesh_selection_records(const list& Value)
{
	k3d::mesh_selection::records_t records;

	long count = len(Value);
	for(long i = 0; i != count; ++i)
	{
		object record = Value[i];
		unsigned long begin = extract<unsigned long>(record[0]);
		unsigned long end = extract<unsigned long>(record[1]);
		double weight = extract<double>(record[2]);
		records.push_back(k3d::mesh_selection::record(begin, end, weight));
	}

	return records;
}

const k3d::mesh_selection::records_t mesh_selection_get_points(const k3d::mesh_selection& lhs)
{
	return lhs.points;
}

const k3d::mesh_selection::records_t mesh_selection_get_edges(const k3d::mesh_selection& lhs)
{
	return lhs.edges;
}

const k3d::mesh_selection::records_t mesh_selection_get_faces(const k3d::mesh_selection& lhs)
{
	return lhs.faces;
}

const k3d::mesh_selection::records_t mesh_selection_get_linear_curves(const k3d::mesh_selection& lhs)
{
	return lhs.linear_curves;
}

const k3d::mesh_selection::records_t mesh_selection_get_cubic_curves(const k3d::mesh_selection& lhs)
{
	return lhs.cubic_curves;
}

const k3d::mesh_selection::records_t mesh_selection_get_nurbs_curves(const k3d::mesh_selection& lhs)
{
	return lhs.nurbs_curves;
}

const k3d::mesh_selection::records_t mesh_selection_get_bilinear_patches(const k3d::mesh_selection& lhs)
{
	return lhs.bilinear_patches;
}

const k3d::mesh_selection::records_t mesh_selection_get_bicubic_patches(const k3d::mesh_selection& lhs)
{
	return lhs.bicubic_patches;
}

const k3d::mesh_selection::records_t mesh_selection_get_nurbs_patches(const k3d::mesh_selection& lhs)
{
	return lhs.nurbs_patches;
}

void mesh_selection_set_points(k3d::mesh_selection& lhs, const list& rhs)
{
	lhs.points = mesh_selection_records(rhs);
}

void mesh_selection_set_edges(k3d::mesh_selection& lhs, const list& rhs)
{
	lhs.edges = mesh_selection_records(rhs);
}

void mesh_selection_set_faces(k3d::mesh_selection& lhs, const list& rhs)
{
	lhs.faces = mesh_selection_records(rhs);
}

void mesh_selection_set_linear_curves(k3d::mesh_selection& lhs, const list& rhs)
{
	lhs.linear_curves = mesh_selection_records(rhs);
}

void mesh_selection_set_cubic_curves(k3d::mesh_selection& lhs, const list& rhs)
{
	lhs.cubic_curves = mesh_selection_records(rhs);
}

void mesh_selection_set_nurbs_curves(k3d::mesh_selection& lhs, const list& rhs)
{
	lhs.nurbs_curves = mesh_selection_records(rhs);
}

void mesh_selection_set_bilinear_patches(k3d::mesh_selection& lhs, const list& rhs)
{
	lhs.bilinear_patches = mesh_selection_records(rhs);
}

void mesh_selection_set_bicubic_patches(k3d::mesh_selection& lhs, const list& rhs)
{
	lhs.bicubic_patches = mesh_selection_records(rhs);
}

void mesh_selection_set_nurbs_patches(k3d::mesh_selection& lhs, const list& rhs)
{
	lhs.nurbs_patches = mesh_selection_records(rhs);
}

void export_mesh_selection()
{
	class_<k3d::mesh_selection>("mesh_selection",
		"Stores a change in mesh selection state.\n\n"
		"Changes are stored as lists of tuples, where each tuple represents a seletion weight to be applied to a range of indices.  "
		"There is one list for each type of primitive: vertices, polyhedron edges, polyhedron faces, linear curves, cubic curves, "
		"NURBS curves, bilinear patches, bicubic patches, and NURBS patches.")
		.def("empty", &k3d::mesh_selection::empty,
			"Returns True if the entire mesh selection is empty (will not have any effect on mesh selection state).")
		.add_property("points", mesh_selection_get_points, mesh_selection_set_points,
			"Stores changes in selection state for mesh vertices.")
		.add_property("edges", mesh_selection_get_edges, mesh_selection_set_edges,
			"Stores changes in selection state for polyhedron vertices.")
		.add_property("faces", mesh_selection_get_faces, mesh_selection_set_faces,
			"Stores changes in selection state for polyhedron face primitives.")
		.add_property("linear_curves", mesh_selection_get_linear_curves, mesh_selection_set_linear_curves,
			"Stores changes in selection state for linear curve primitives.")
		.add_property("cubic_curves", mesh_selection_get_cubic_curves, mesh_selection_set_cubic_curves,
			"Stores changes in selection state for cubic curve primitives.")
		.add_property("nurbs_curves", mesh_selection_get_nurbs_curves, mesh_selection_set_nurbs_curves,
			"Stores changes in selection state for NURBS curve primitives.")
		.add_property("bilinear_patches", mesh_selection_get_bilinear_patches, mesh_selection_set_bilinear_patches,
			"Stores changes in selection state for bilinear patch primitives.")
		.add_property("bicubic_patches", mesh_selection_get_bicubic_patches, mesh_selection_set_bicubic_patches,
			"Stores changes in selection state for bicubic patch primitives.")
		.add_property("nurbs_patches", mesh_selection_get_nurbs_patches, mesh_selection_set_nurbs_patches,
			"Stores changes in selection state for NURBS patch primitives.")
		.def(self == self)
		.def(self != self)
		.def(self_ns::str(self));
}

} // namespace python

} // namespace k3d

