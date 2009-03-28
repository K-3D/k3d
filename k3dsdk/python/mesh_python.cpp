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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "attribute_arrays_python.h"
#include "const_attribute_arrays_python.h"
#include "const_typed_array_python.h"
#include "instance_wrapper_python.h"
#include "mesh_python.h"
#include "typed_array_python.h"
#include "utility_python.h"

#include <k3dsdk/color.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/type_registry.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////
// wrap_const_object

template<typename return_type, typename value_type>
object wrap_const_object(const pipeline_data<value_type>& Data)
{
	if(!Data)
		return object();

	return object(return_type(*Data));
}

////////////////////////////////////////////////////////////////////////////////////////
// wrap_non_const_object

template<typename return_type, typename value_type>
object wrap_non_const_object(pipeline_data<value_type>& Data)
{
	if(!Data)
		return object();

	return object(return_type(Data.writable()));
}

////////////////////////////////////////////////////////////////////////////////////////
// create_object

template<typename return_type, typename value_type>
object create_object(pipeline_data<value_type>& Data)
{
	return object(return_type(Data.create()));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// wrap_const_array

template<typename array_type>
object wrap_const_array(const pipeline_data<array_type>& Data)
{
	if(!Data)
		return object();

	return object(instance_wrapper<const array_type>(*Data));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// wrap_non_const_array

template<typename array_type>
object wrap_non_const_array(pipeline_data<array_type>& Data)
{
	if(!Data)
		return object();

	return object(instance_wrapper<array_type>(Data.writable()));
}

////////////////////////////////////////////////////////////////////////////////////////////////
// create_array

template<typename array_type>
object create_array(pipeline_data<array_type>& Data)
{
	return object(instance_wrapper<array_type>(Data.create()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_polyhedra

class const_polyhedra :
	public instance_wrapper<const k3d::mesh::polyhedra_t>
{
	typedef instance_wrapper<const k3d::mesh::polyhedra_t> base;
public:
	const_polyhedra() :
		base()
	{
	}

	const_polyhedra(const k3d::mesh::polyhedra_t* Polyhedra) :
		base(Polyhedra)
	{
	}

	const_polyhedra(const k3d::mesh::polyhedra_t& Polyhedra) :
		base(Polyhedra)
	{
	}

	object first_faces() { return wrap_const_array(wrapped().first_faces); }
	object face_counts() { return wrap_const_array(wrapped().face_counts); }
	object types() { return wrap_const_array(wrapped().types); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object face_first_loops() { return wrap_const_array(wrapped().face_first_loops); }
	object face_loop_counts() { return wrap_const_array(wrapped().face_loop_counts); }
	object face_selection() { return wrap_const_array(wrapped().face_selection); }
	object face_materials() { return wrap_const_array(wrapped().face_materials); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object loop_first_edges() { return wrap_const_array(wrapped().loop_first_edges); }
	object edge_points() { return wrap_const_array(wrapped().edge_points); }
	object clockwise_edges() { return wrap_const_array(wrapped().clockwise_edges); }
	object edge_selection() { return wrap_const_array(wrapped().edge_selection); }
	object face_varying_data() { return wrap(wrapped().face_varying_data); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// polyhedra

class polyhedra :
	public instance_wrapper<k3d::mesh::polyhedra_t>
{
	typedef instance_wrapper<k3d::mesh::polyhedra_t> base;
public:
	polyhedra() :
		base()
	{
	}

	polyhedra(k3d::mesh::polyhedra_t* Polyhedra) :
		base(Polyhedra)
	{
	}

	polyhedra(k3d::mesh::polyhedra_t& Polyhedra) :
		base(Polyhedra)
	{
	}

	object first_faces() { return wrap_const_array(wrapped().first_faces); }
	object face_counts() { return wrap_const_array(wrapped().face_counts); }
	object types() { return wrap_const_array(wrapped().types); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object face_first_loops() { return wrap_const_array(wrapped().face_first_loops); }
	object face_loop_counts() { return wrap_const_array(wrapped().face_loop_counts); }
	object face_selection() { return wrap_const_array(wrapped().face_selection); }
	object face_materials() { return wrap_const_array(wrapped().face_materials); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object loop_first_edges() { return wrap_const_array(wrapped().loop_first_edges); }
	object edge_points() { return wrap_const_array(wrapped().edge_points); }
	object clockwise_edges() { return wrap_const_array(wrapped().clockwise_edges); }
	object edge_selection() { return wrap_const_array(wrapped().edge_selection); }
	object face_varying_data() { return wrap(wrapped().face_varying_data); }

	object writable_first_faces() { return wrap_non_const_array(wrapped().first_faces); }
	object writable_face_counts() { return wrap_non_const_array(wrapped().face_counts); }
	object writable_types() { return wrap_non_const_array(wrapped().types); }
	object writable_constant_data() { return wrap(wrapped().constant_data); }
	object writable_face_first_loops() { return wrap_non_const_array(wrapped().face_first_loops); }
	object writable_face_loop_counts() { return wrap_non_const_array(wrapped().face_loop_counts); }
	object writable_face_selection() { return wrap_non_const_array(wrapped().face_selection); }
	object writable_face_materials() { return wrap_non_const_array(wrapped().face_materials); }
	object writable_uniform_data() { return wrap(wrapped().uniform_data); }
	object writable_loop_first_edges() { return wrap_non_const_array(wrapped().loop_first_edges); }
	object writable_edge_points() { return wrap_non_const_array(wrapped().edge_points); }
	object writable_clockwise_edges() { return wrap_non_const_array(wrapped().clockwise_edges); }
	object writable_edge_selection() { return wrap_non_const_array(wrapped().edge_selection); }
	object writable_face_varying_data() { return wrap(wrapped().face_varying_data); }

	object create_first_faces() { return create_array(wrapped().first_faces); }
	object create_face_counts() { return create_array(wrapped().face_counts); }
	object create_types() { return create_array(wrapped().types); }
	object create_face_first_loops() { return create_array(wrapped().face_first_loops); }
	object create_face_loop_counts() { return create_array(wrapped().face_loop_counts); }
	object create_face_selection() { return create_array(wrapped().face_selection); }
	object create_face_materials() { return create_array(wrapped().face_materials); }
	object create_loop_first_edges() { return create_array(wrapped().loop_first_edges); }
	object create_edge_points() { return create_array(wrapped().edge_points); }
	object create_clockwise_edges() { return create_array(wrapped().clockwise_edges); }
	object create_edge_selection() { return create_array(wrapped().edge_selection); }
};

} // namespace detail

///////////////////////////////////////////////////////////////////////////////////////////////////
// mesh

mesh::mesh() :
	base()
{
}

mesh::mesh(k3d::mesh* Mesh) :
	base(Mesh)
{
}

void mesh::copy(const mesh& RHS)
{
	wrapped() = RHS.wrapped();
}

object mesh::create_point_selection() { return detail::create_array(wrapped().point_selection); } 
object mesh::create_points() { return detail::create_array(wrapped().points); }
object mesh::create_polyhedra() { return detail::create_object<detail::polyhedra, k3d::mesh::polyhedra_t>(wrapped().polyhedra); } 
object mesh::point_selection() { return detail::wrap_const_array(wrapped().point_selection); } 
object mesh::points() { return detail::wrap_const_array(wrapped().points); } 
object mesh::polyhedra() { return detail::wrap_const_object<detail::const_polyhedra>(wrapped().polyhedra); } 
object mesh::vertex_data() { return wrap(wrapped().vertex_data); } 
object mesh::writable_point_selection() { return detail::wrap_non_const_array(wrapped().point_selection); } 
object mesh::writable_points() { return detail::wrap_non_const_array(wrapped().points); } 
object mesh::writable_polyhedra() { return detail::wrap_non_const_object<detail::polyhedra>(wrapped().polyhedra); } 
object mesh::writable_vertex_data() { return wrap(wrapped().vertex_data); } 

static object mesh_primitives(mesh& Self)
{
	return wrap(Self.wrapped().primitives);
}

const string_t mesh::repr()
{
	std::ostringstream buffer;
	buffer << wrapped();
	return buffer.str();
}

const string_t mesh::str()
{
	std::ostringstream buffer;
	buffer << "<k3d.mesh object wrapping mesh " << &wrapped() << ">";
	return buffer.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_mesh_primitive 

static const string_t const_mesh_primitive_get_type(const_mesh_primitive_wrapper& Self)
{
	return Self.wrapped().type;
}

static object const_mesh_primitive_get_structure(const_mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().structure);
}

static object const_mesh_primitive_get_attributes(const_mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().attributes);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// mesh_primitive 

static const string_t mesh_primitive_get_type(mesh_primitive_wrapper& Self)
{
	return Self.wrapped().type;
}

static void mesh_primitive_set_type(mesh_primitive_wrapper& Self, const string_t& Type)
{
	Self.wrapped().type = Type;
}

static object mesh_primitive_get_structure(mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().structure);
}

static object mesh_primitive_get_attributes(mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().attributes);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_mesh_primitives_t

typedef instance_wrapper<const k3d::mesh::primitives_t> const_mesh_primitives_t_wrapper;

static object const_mesh_primitives_t_get_item(const_mesh_primitives_t_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item).get());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// mesh_primitives_t

typedef instance_wrapper<k3d::mesh::primitives_t> mesh_primitives_t_wrapper;

static object mesh_primitives_t_get_item(mesh_primitives_t_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item).get());
}

static object mesh_primitives_t_create(mesh_primitives_t_wrapper& Self, const string_t& Type)
{
	return wrap(Self.wrapped().create(Type));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// define_class_mesh 

void define_class_mesh()
{
	class_<detail::const_polyhedra>("const_polyhedra",
		"Stores an immutable (read-only) collection of polyhedron primitives.")
		.def("first_faces", &detail::const_polyhedra::first_faces)
		.def("face_counts", &detail::const_polyhedra::face_counts)
		.def("types", &detail::const_polyhedra::types)
		.def("constant_data", &detail::const_polyhedra::constant_data)
		.def("face_first_loops", &detail::const_polyhedra::face_first_loops)
		.def("face_loop_counts", &detail::const_polyhedra::face_loop_counts)
		.def("face_selection", &detail::const_polyhedra::face_selection)
		.def("face_materials", &detail::const_polyhedra::face_materials)
		.def("uniform_data", &detail::const_polyhedra::uniform_data)
		.def("loop_first_edges", &detail::const_polyhedra::loop_first_edges)
		.def("edge_points", &detail::const_polyhedra::edge_points)
		.def("clockwise_edges", &detail::const_polyhedra::clockwise_edges)
		.def("edge_selection", &detail::const_polyhedra::edge_selection)
		.def("face_varying_data", &detail::const_polyhedra::face_varying_data);

	class_<detail::polyhedra>("polyhedra",
		"Stores a mutable (read-write) collection of polyhedron primitives.")
		.def("first_faces", &detail::polyhedra::first_faces)
		.def("face_counts", &detail::polyhedra::face_counts)
		.def("types", &detail::polyhedra::types)
		.def("constant_data", &detail::polyhedra::constant_data)
		.def("face_first_loops", &detail::polyhedra::face_first_loops)
		.def("face_loop_counts", &detail::polyhedra::face_loop_counts)
		.def("face_selection", &detail::polyhedra::face_selection)
		.def("face_materials", &detail::polyhedra::face_materials)
		.def("uniform_data", &detail::polyhedra::uniform_data)
		.def("loop_first_edges", &detail::polyhedra::loop_first_edges)
		.def("edge_points", &detail::polyhedra::edge_points)
		.def("clockwise_edges", &detail::polyhedra::clockwise_edges)
		.def("edge_selection", &detail::polyhedra::edge_selection)
		.def("face_varying_data", &detail::polyhedra::face_varying_data)

		.def("writable_first_faces", &detail::polyhedra::writable_first_faces)
		.def("writable_face_counts", &detail::polyhedra::writable_face_counts)
		.def("writable_types", &detail::polyhedra::writable_types)
		.def("writable_constant_data", &detail::polyhedra::writable_constant_data)
		.def("writable_face_first_loops", &detail::polyhedra::writable_face_first_loops)
		.def("writable_face_loop_counts", &detail::polyhedra::writable_face_loop_counts)
		.def("writable_face_selection", &detail::polyhedra::writable_face_selection)
		.def("writable_face_materials", &detail::polyhedra::writable_face_materials)
		.def("writable_uniform_data", &detail::polyhedra::writable_uniform_data)
		.def("writable_loop_first_edges", &detail::polyhedra::writable_loop_first_edges)
		.def("writable_edge_points", &detail::polyhedra::writable_edge_points)
		.def("writable_clockwise_edges", &detail::polyhedra::writable_clockwise_edges)
		.def("writable_edge_selection", &detail::polyhedra::writable_edge_selection)
		.def("writable_face_varying_data", &detail::polyhedra::writable_face_varying_data)

		.def("create_first_faces", &detail::polyhedra::create_first_faces)
		.def("create_face_counts", &detail::polyhedra::create_face_counts)
		.def("create_types", &detail::polyhedra::create_types)
		.def("create_face_first_loops", &detail::polyhedra::create_face_first_loops)
		.def("create_face_loop_counts", &detail::polyhedra::create_face_loop_counts)
		.def("create_face_selection", &detail::polyhedra::create_face_selection)
		.def("create_face_materials", &detail::polyhedra::create_face_materials)
		.def("create_loop_first_edges", &detail::polyhedra::create_loop_first_edges)
		.def("create_edge_points", &detail::polyhedra::create_edge_points)
		.def("create_clockwise_edges", &detail::polyhedra::create_clockwise_edges)
		.def("create_edge_selection", &detail::polyhedra::create_edge_selection);

	enum_<k3d::mesh::polyhedra_t::polyhedron_type>("polyhedron_type")
		.value("polygons", k3d::mesh::polyhedra_t::POLYGONS)
		.value("catmull_clark", k3d::mesh::polyhedra_t::CATMULL_CLARK)
		.attr("__module__") = "k3d";

	scope outer = class_<mesh>("mesh", 
		"Stores a heterogeneous collection of geometric mesh primitives.", no_init)
		.def("vertex_data", &mesh::vertex_data,
			"Returns a L{const_named_arrays} object containing a collection of immutable (read-only) per-vertex data, or None.")
		.def("point_selection", &mesh::point_selection,
			"Returns an immutable (read-only) L{const_double_array} object containing the selection state of every vertex in the mesh, or None.")
		.def("points", &mesh::points,
			"Returns an immutable (read-only) L{const_point3_array} object containing the geometric coordinates of every vertex in the mesh, or None.")
		.def("polyhedra", &mesh::polyhedra,
			"Returns a L{const_polyhedra} object containing a collection of immutable (read-only) polyhedron primitives, or None.")
		.def("writable_vertex_data", &mesh::writable_vertex_data,
			"Returns a L{named_arrays} object containing a collection of mutable (read-write) per-vertex data, or None.")
		.def("writable_point_selection", &mesh::writable_point_selection,
			"Returns a mutable (read-write) L{double_array} object containing the selection state of every vertex in the mesh, or None.")
		.def("writable_points", &mesh::writable_points,
			"Returns a mutable (read-write) L{point3_array} object containing the geometric coordinates of every vertex in the mesh, or None.")
		.def("writable_polyhedra", &mesh::writable_polyhedra,
			"Returns a L{polyhedra} object containing a mutable (read-write) collection of polyhedron primitives, or None.")
		.def("copy", &mesh::copy,
			"Store a shallow copy of the given L{mesh}.")
		.def("create_point_selection", &mesh::create_point_selection,
			"Creates and returns a new L{double_array} object used to store the selection state of every vertex in the mesh.")
		.def("create_points", &mesh::create_points,
			"Creates and returns a new L{point3_array} object used to store the geometric coordinates of every vertex in the mesh.")
		.def("create_polyhedra", &mesh::create_polyhedra,
			"Create and returns a new L{polyhedra} object for storing polyhedron primitives.")
		.def("primitives", &mesh_primitives,
			"Returns the set of L{const_primitive} objects in the mesh.")
		.def("__repr__", &mesh::repr)
		.def("__str__", &mesh::str);

	class_<const_mesh_primitive_wrapper>("const_primitive", no_init)
		.def("type", &const_mesh_primitive_get_type)
		.def("structure", &const_mesh_primitive_get_structure)
		.def("attributes", &const_mesh_primitive_get_attributes)
		;

	class_<mesh_primitive_wrapper>("primitive", no_init)
		.def("type", &mesh_primitive_get_type)
		.def("structure", &mesh_primitive_get_structure)
		.def("attributes", &mesh_primitive_get_attributes)
		;

	class_<const_mesh_primitives_t_wrapper>("const_primitives_t", no_init)
		.def("__len__", &utility::wrapped_len<const_mesh_primitives_t_wrapper>)
		.def("__getitem__", &const_mesh_primitives_t_get_item)
		;

	class_<mesh_primitives_t_wrapper>("primitives_t", no_init)
		.def("__len__", &utility::wrapped_len<mesh_primitives_t_wrapper>)
		.def("__getitem__", &mesh_primitives_t_get_item)
		.def("create", &mesh_primitives_t_create)
		;
}

} // namespace python

} // namespace k3d

