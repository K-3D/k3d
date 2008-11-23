// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include "imaterial_python.h"
#include "mesh_python.h"
#include "owned_instance_wrapper_python.h"
#include "polyhedron_python.h"
#include "utility_python.h"

#include <k3dsdk/polyhedron.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;
#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

class polyhedron
{
public:
	class const_primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::polyhedron::const_primitive> wrapper;

		static object first_faces(wrapper& Self) { return wrap(Self.wrapped().first_faces); }
		static object face_counts(wrapper& Self) { return wrap(Self.wrapped().face_counts); }
		static object polyhedron_types(wrapper& Self) { return wrap(Self.wrapped().polyhedron_types); }
		static object face_first_loops(wrapper& Self) { return wrap(Self.wrapped().face_first_loops); }
		static object face_loop_counts(wrapper& Self) { return wrap(Self.wrapped().face_loop_counts); }
		static object face_selections(wrapper& Self) { return wrap(Self.wrapped().face_selections); }
		static object face_materials(wrapper& Self) { return wrap(Self.wrapped().face_materials); }
		static object loop_first_edges(wrapper& Self) { return wrap(Self.wrapped().loop_first_edges); }
		static object edge_points(wrapper& Self) { return wrap(Self.wrapped().edge_points); }
		static object clockwise_edges(wrapper& Self) { return wrap(Self.wrapped().clockwise_edges); }
		static object edge_selections(wrapper& Self) { return wrap(Self.wrapped().edge_selections); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object face_varying_data(wrapper& Self) { return wrap(Self.wrapped().face_varying_data); }
	};

	class primitive
	{
	public:
		typedef owned_instance_wrapper<k3d::polyhedron::primitive> wrapper;

		static object first_faces(wrapper& Self) { return wrap(Self.wrapped().first_faces); }
		static object face_counts(wrapper& Self) { return wrap(Self.wrapped().face_counts); }
		static object polyhedron_types(wrapper& Self) { return wrap(Self.wrapped().polyhedron_types); }
		static object face_first_loops(wrapper& Self) { return wrap(Self.wrapped().face_first_loops); }
		static object face_loop_counts(wrapper& Self) { return wrap(Self.wrapped().face_loop_counts); }
		static object face_selections(wrapper& Self) { return wrap(Self.wrapped().face_selections); }
		static object face_materials(wrapper& Self) { return wrap(Self.wrapped().face_materials); }
		static object loop_first_edges(wrapper& Self) { return wrap(Self.wrapped().loop_first_edges); }
		static object edge_points(wrapper& Self) { return wrap(Self.wrapped().edge_points); }
		static object clockwise_edges(wrapper& Self) { return wrap(Self.wrapped().clockwise_edges); }
		static object edge_selections(wrapper& Self) { return wrap(Self.wrapped().edge_selections); }
		static object constant_data(wrapper& Self) { return wrap(Self.wrapped().constant_data); }
		static object uniform_data(wrapper& Self) { return wrap(Self.wrapped().uniform_data); }
		static object face_varying_data(wrapper& Self) { return wrap(Self.wrapped().face_varying_data); }
	};


	static object create(mesh& Mesh)
	{
		return wrap_owned(k3d::polyhedron::create(Mesh.wrapped()));
	}

	static object create2(mesh& Mesh, list Vertices, list VertexCounts, list VertexIndices, object Material)
	{
		k3d::mesh::points_t vertices;
		k3d::mesh::counts_t vertex_counts;
		k3d::mesh::indices_t vertex_indices;
		k3d::imaterial* const material = Material ? boost::python::extract<imaterial_wrapper>(Material)().wrapped_ptr()  : 0;

		utility::copy(Vertices, vertices);
		utility::copy(VertexCounts, vertex_counts);
		utility::copy(VertexIndices, vertex_indices);

		return wrap_owned(k3d::polyhedron::create(Mesh.wrapped(), vertices, vertex_counts, vertex_indices, material));
	}

/*
	static object validate(mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::polyhedron::validate(Primitive.wrapped()));
	}

	static object validate_const(const_mesh_primitive_wrapper& Primitive)
	{
		return wrap_owned(k3d::polyhedron::validate(Primitive.wrapped()));
	}
*/
};

void define_namespace_polyhedron()
{
	scope outer = class_<polyhedron>("polyhedron",
		"Provides functionality to create and manipulate polyhedron mesh primitives.", no_init)
		.def("create", &polyhedron::create,
			"Creates an empty polyhedron, returning an object that can be used to access all of its arrays.")
		.def("create", &polyhedron::create2,
			"Creates a polyhedron, populating it from a list of vertices, a list of per-face vertex counts, a list of per-face vertices, and an optional material node.")
		.staticmethod("create")
/*
		.def("validate", &polyhedron::validate)
		.def("validate", &polyhedron::validate_const)
		.staticmethod("validate")
*/
		;

	enum_<k3d::polyhedron::polyhedron_type>("polyhedron_type")
		.value("POLYGONS", k3d::polyhedron::POLYGONS)
		.value("CATMULL_CLARK", k3d::polyhedron::CATMULL_CLARK)
		.attr("__module__") = "k3d";

	class_<polyhedron::const_primitive::wrapper>("const_primitive", no_init)
		.def("first_faces", &polyhedron::const_primitive::first_faces)
		.def("face_counts", &polyhedron::const_primitive::face_counts)
		.def("polyhedron_types", &polyhedron::const_primitive::polyhedron_types)
		.def("face_first_loops", &polyhedron::const_primitive::face_first_loops)
		.def("face_loop_counts", &polyhedron::const_primitive::face_loop_counts)
		.def("face_selections", &polyhedron::const_primitive::face_selections)
		.def("face_materials", &polyhedron::const_primitive::face_materials)
		.def("loop_first_edges", &polyhedron::const_primitive::loop_first_edges)
		.def("edge_points", &polyhedron::const_primitive::edge_points)
		.def("clockwise_edges", &polyhedron::const_primitive::clockwise_edges)
		.def("edge_selections", &polyhedron::const_primitive::edge_selections)
		.def("constant_data", &polyhedron::const_primitive::constant_data)
		.def("uniform_data", &polyhedron::const_primitive::uniform_data)
		.def("face_varying_data", &polyhedron::const_primitive::face_varying_data)
		;

	class_<polyhedron::primitive::wrapper>("primitive", no_init)
		.def("first_faces", &polyhedron::primitive::first_faces)
		.def("face_counts", &polyhedron::primitive::face_counts)
		.def("polyhedron_types", &polyhedron::primitive::polyhedron_types)
		.def("face_first_loops", &polyhedron::primitive::face_first_loops)
		.def("face_loop_counts", &polyhedron::primitive::face_loop_counts)
		.def("face_selections", &polyhedron::primitive::face_selections)
		.def("face_materials", &polyhedron::primitive::face_materials)
		.def("loop_first_edges", &polyhedron::primitive::loop_first_edges)
		.def("edge_points", &polyhedron::primitive::edge_points)
		.def("clockwise_edges", &polyhedron::primitive::clockwise_edges)
		.def("edge_selections", &polyhedron::primitive::edge_selections)
		.def("constant_data", &polyhedron::primitive::constant_data)
		.def("uniform_data", &polyhedron::primitive::uniform_data)
		.def("face_varying_data", &polyhedron::primitive::face_varying_data)
		;
}

} // namespace python

} // namespace k3d

