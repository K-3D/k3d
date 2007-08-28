// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
//
// As a special exception, you have permission to link this program
// with the CGAL (http://www.cgal.org) library and distribute executables, as long as you
// follow the requirements of the GNU GPL in regard to all of the
// software in the executable aside from CGAL.

/** \file
		\brief Nodes to perform 3D boolean operations using CGAL
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "conversion.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dbooleans
{

/////////////////////////////////////////////////////////////////////////////
// cgal_boolean

class cgal_boolean :
	public k3d::material_client<k3d::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	cgal_boolean(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_left(init_owner(*this) + init_name("left") + init_label(_("Left Mesh")) + init_description(_("Left operand mesh for the boolean operation")) + init_value<k3d::mesh*>(0)),
		m_right(init_owner(*this) + init_name("right") + init_label(_("Right Mesh")) + init_description(_("Right operand mesh for the boolean operation")) + init_value<k3d::mesh*>(0)),
		m_left_matrix(init_owner(*this) + init_name("left_matrix") + init_label(_("Left Matrix")) + init_description("Transformation matrix to apply to left operand") + init_value(k3d::identity3D())),
		m_right_matrix(init_owner(*this) + init_name("right_matrix") + init_label(_("Right Matrix")) + init_description("Transformation matrix to apply to right operand") + init_value(k3d::identity3D()))	
	{
		m_left.changed_signal().connect(make_topology_changed_slot());
		m_right.changed_signal().connect(make_topology_changed_slot());
		m_left_matrix.changed_signal().connect(make_topology_changed_slot());
		m_right_matrix.changed_signal().connect(make_topology_changed_slot());
	}
	
	virtual ~cgal_boolean() {}
	
	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		const k3d::mesh* left = m_left.pipeline_value();
		const k3d::mesh* right = m_right.pipeline_value();
		
		if (!left || !right || !k3d::validate_polyhedra(*left) || !k3d::validate_polyhedra(*right))
			return;
		
		//CGAL::set_pretty_mode(std::cerr);
		//CGAL_NEF_SETDTHREAD(19*43*131*509);
		boost::shared_ptr<Nef_polyhedron> left_nef = to_nef(*left, m_left_matrix.pipeline_value());
		boost::shared_ptr<Nef_polyhedron> right_nef = to_nef(*right, m_right_matrix.pipeline_value());
    
    Nef_polyhedron bool_result = do_boolean(*left_nef, *right_nef);
    to_mesh(bool_result, Mesh, dynamic_cast<k3d::imaterial*>(k3d::default_material(document())));
	}
	
	void on_update_mesh_geometry(k3d::mesh& Mesh) {}
	
private:

	virtual Nef_polyhedron do_boolean(const Nef_polyhedron& Left, const Nef_polyhedron& Right) = 0;
	
	k3d_data(k3d::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_left;
	k3d_data(k3d::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_right;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, with_serialization) m_left_matrix;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, with_serialization) m_right_matrix;
};

////////////// boolean_difference ///////////////

class boolean_difference : public cgal_boolean
{
public:
	boolean_difference(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		cgal_boolean(Factory, Document)
	{}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<boolean_difference, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x0d88fe28, 0xea4fdf20, 0xae858297, 0x487f857b),
			"BooleanDifference",
			_("Boolean difference operation"),
			"Boolean",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	Nef_polyhedron do_boolean(const Nef_polyhedron& Left, const Nef_polyhedron& Right)
	{
		return Left - Right;
	}
};

k3d::iplugin_factory& boolean_difference_factory()
{
	return boolean_difference::get_factory();
}

////////////// boolean_union ///////////////

class boolean_union : public cgal_boolean
{
public:
	boolean_union(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		cgal_boolean(Factory, Document)
	{}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<boolean_union, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x4e6fe9fc, 0x4147d863, 0xc5f0229b, 0x4ed9d5fe),
			"BooleanUnion",
			_("Boolean union operation"),
			"Boolean",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	Nef_polyhedron do_boolean(const Nef_polyhedron& Left, const Nef_polyhedron& Right)
	{
		return Left + Right;
	}
};

k3d::iplugin_factory& boolean_union_factory()
{
	return boolean_union::get_factory();
}

////////////// boolean_intersection ///////////////

class boolean_intersection : public cgal_boolean
{
public:
	boolean_intersection(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		cgal_boolean(Factory, Document)
	{}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<boolean_intersection, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0xf7f8c245, 0x5d4e7f4b, 0xe1d08585, 0x78852435),
			"BooleanIntersection",
			_("Boolean intersection operation"),
			"Boolean",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	Nef_polyhedron do_boolean(const Nef_polyhedron& Left, const Nef_polyhedron& Right)
	{
		return Left * Right;
	}
};

k3d::iplugin_factory& boolean_intersection_factory()
{
	return boolean_intersection::get_factory();
}

}
