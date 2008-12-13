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

#include "mesh_python.h"
#include "mesh_selection_python.h"

#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/result.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

static boost::python::object convert(const k3d::mesh_selection::records_t& Value)
{
	boost::python::list records;

	for(k3d::mesh_selection::records_t::const_iterator record = Value.begin(); record != Value.end(); ++record)
		records.append(boost::python::make_tuple(record->begin, record->end, record->weight));

	return records;
}

static const k3d::mesh_selection::records_t convert(const list& Value)
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

static const boost::python::object get_points(const k3d::mesh_selection& Self)
{
	return convert(Self.points);
}

static const boost::python::object get_edges(const k3d::mesh_selection& Self)
{
	return convert(Self.edges);
}

static const boost::python::object get_faces(const k3d::mesh_selection& Self)
{
	return convert(Self.faces);
}

static const boost::python::object get_nurbs_curves(const k3d::mesh_selection& Self)
{
	return convert(Self.nurbs_curves);
}

static const boost::python::object get_nurbs_patches(const k3d::mesh_selection& Self)
{
	return convert(Self.nurbs_patches);
}

static void set_points(k3d::mesh_selection& Self, const list& Value)
{
	Self.points = convert(Value);
}

static void set_edges(k3d::mesh_selection& Self, const list& Value)
{
	Self.edges = convert(Value);
}

static void set_faces(k3d::mesh_selection& Self, const list& Value)
{
	Self.faces = convert(Value);
}

static void set_nurbs_curves(k3d::mesh_selection& Self, const list& Value)
{
	Self.nurbs_curves = convert(Value);
}

static void set_nurbs_patches(k3d::mesh_selection& Self, const list& Value)
{
	Self.nurbs_patches = convert(Value);
}

static boost::python::object component_select_all()
{
	return convert(k3d::mesh_selection::component_select_all());
}

static boost::python::object component_deselect_all()
{
	return convert(k3d::mesh_selection::component_deselect_all());
}

static void store(const k3d::python::mesh& Mesh, k3d::mesh_selection& Selection)
{
	k3d::mesh_selection::store(Mesh.wrapped(), Selection);
}

static void merge(const k3d::mesh_selection& Selection, k3d::python::mesh& Mesh)
{
	k3d::mesh_selection::merge(Selection, Mesh.wrapped());
}

void define_class_mesh_selection()
{
	scope outer = class_<k3d::mesh_selection>("mesh_selection",
		"Stores a change in mesh selection state.\n\n"
		"Changes are stored as lists of tuples, where each tuple represents a seletion weight to be applied to a range of indices.  "
		"There is one list for each type of primitive: vertices, polyhedron edges, polyhedron faces, linear curves, cubic curves, "
		"NURBS curves, bilinear patches, bicubic patches, and NURBS patches.")
		.def("select_all", &k3d::mesh_selection::select_all,
			"Returns a L{mesh_selection} that explicitly selects every component.")
		.staticmethod("select_all")
		.def("deselect_all", &k3d::mesh_selection::deselect_all,
			"Returns a L{mesh_selection} that explicitly deselects every component.")
		.staticmethod("deselect_all")
		.def("select_null", &k3d::mesh_selection::select_null,
			"Returns a L{mesh_selection} that does not select or deselect any components.")
		.staticmethod("select_null")
		.def("store", &store,
			"Stores a mesh's selection state in a L{mesh_selection}.")
		.staticmethod("store")
		.def("merge", &merge,
			"Merges a L{mesh_selection} with a mesh's selection state.")
		.staticmethod("merge")
		.def("add_component", &k3d::mesh_selection::add_component)
		.def("clear", &k3d::mesh_selection::clear,
			"Clears a mesh selection so that it will not have any effect on mesh state.")
		.def("empty", &k3d::mesh_selection::empty,
			"Returns True if the entire mesh selection is empty (will not have any effect on mesh state).")
		.def(self == self)
		.def(self != self)
		.def(self_ns::str(self))
		// Deprecated
		.add_property("points", get_points, set_points,
			"Stores changes in selection state for mesh vertices.")
		.add_property("edges", get_edges, set_edges,
			"Stores changes in selection state for polyhedron vertices.")
		.add_property("faces", get_faces, set_faces,
			"Stores changes in selection state for polyhedron face primitives.")
		.add_property("nurbs_curves", get_nurbs_curves, set_nurbs_curves,
			"Stores changes in selection state for NURBS curve primitives.")
		.add_property("nurbs_patches", get_nurbs_patches, set_nurbs_patches,
			"Stores changes in selection state for NURBS patch primitives.")
		.def("component_deselect_all", component_deselect_all,
			"Returns a list for use with L{mesh_selection} that deselects an entire range of mesh components.")
		.staticmethod("component_deselect_all")
		.def("component_select_all", component_select_all,
			"Returns a list for use with L{mesh_selection} that selects an entire range of mesh components.")
		.staticmethod("component_select_all")
		;

	class_<k3d::mesh_selection::component>("component")
		.def(init<uint_t, uint_t, k3d::selection::type>())
		.def(init<uint_t, uint_t, k3d::selection::type, uint_t, uint_t, double_t>())
		.def("add_range", &k3d::mesh_selection::component::add_range)
		.def(self == self)
		;
}

} // namespace python

} // namespace k3d

