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
		m_type(init_owner(*this) + init_name("type") + init_label(_("Type")) + init_description(_("Boolean operation (intersection, union, difference, reverse difference)")) + init_value(BOOLEAN_INTERSECTION) + init_enumeration(boolean_values())),
		m_left(init_owner(*this) + init_name("left") + init_label(_("Left Mesh")) + init_description(_("Left operand mesh for the boolean operation")) + init_value<k3d::mesh*>(0)),
		m_right(init_owner(*this) + init_name("right") + init_label(_("Right Mesh")) + init_description(_("Right operand mesh for the boolean operation")) + init_value<k3d::mesh*>(0)),
		m_left_matrix(init_owner(*this) + init_name("left_matrix") + init_label(_("Left Matrix")) + init_description("Transformation matrix to apply to left operand") + init_value(k3d::identity3D())),
		m_right_matrix(init_owner(*this) + init_name("right_matrix") + init_label(_("Right Matrix")) + init_description("Transformation matrix to apply to right operand") + init_value(k3d::identity3D()))	
	{
		m_type.changed_signal().connect(make_topology_changed_slot());
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
		
		CGAL::set_error_handler(k3d_failure_handler);
		boost::shared_ptr<Nef_polyhedron> left_nef;
		boost::shared_ptr<Nef_polyhedron> right_nef;
		try
		{
			left_nef = to_nef(*left, m_left_matrix.pipeline_value());
			right_nef = to_nef(*right, m_right_matrix.pipeline_value());
		}
		catch (std::exception& e)
		{
			k3d::log() << error << "CGALBoolean: error converting input" << std::endl;
			return;
		}
				
		try
		{
			Nef_polyhedron bool_result;
			switch(m_type.pipeline_value())
			{
				case BOOLEAN_INTERSECTION:
					bool_result = (*left_nef) * (*right_nef);
					break;
	
				case BOOLEAN_UNION:
					bool_result = (*left_nef) + (*right_nef);
					break;
	
				case BOOLEAN_DIFFERENCE:
					bool_result = (*left_nef) - (*right_nef);
					break;
	
				case BOOLEAN_REVERSE_DIFFERENCE:
					bool_result = (*right_nef) - (*left_nef);
					break;
			}
	    
	    to_mesh(bool_result, Mesh, dynamic_cast<k3d::imaterial*>(k3d::default_material(document())));
		}
		catch (std::exception& e)
		{
			k3d::log() << error << "CGALBoolean: error executing boolean operation" << std::endl;
		}
	}
	
	void on_update_mesh_geometry(k3d::mesh& Mesh) {}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cgal_boolean, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x0d88fe28, 0xea4fdf20, 0xae858297, 0x487f857b),
			"CGALBoolean",
			_("3D mesh boolean operations"),
			"Boolean",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
private:

	typedef enum
	{
		BOOLEAN_INTERSECTION,
		BOOLEAN_UNION,
		BOOLEAN_DIFFERENCE,
		BOOLEAN_REVERSE_DIFFERENCE
	} boolean_t;

	static const k3d::ienumeration_property::enumeration_values_t& boolean_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Intersection"), "intersection", _("Render intersecting volumes")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Union"), "union", _("Render the union of two volumnes")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Difference"), "difference", _("Render the difference of two volumes")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Reverse Difference"), "reverse_difference", _("Render the difference of two volumes")));
		}

		return values;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const boolean_t& Value)
	{
		switch(Value)
		{
			case BOOLEAN_UNION:
				Stream << "union";
				break;
			case BOOLEAN_INTERSECTION:
				Stream << "intersection";
				break;
			case BOOLEAN_DIFFERENCE:
				Stream << "difference";
				break;
			case BOOLEAN_REVERSE_DIFFERENCE:
				Stream << "reverse_difference";
				break;
		}

		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, boolean_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "union")
			Value = BOOLEAN_UNION;
		else if(text == "intersection")
			Value = BOOLEAN_INTERSECTION;
		else if(text == "difference")
			Value = BOOLEAN_DIFFERENCE;
		else if(text == "reverse_difference")
			Value = BOOLEAN_REVERSE_DIFFERENCE;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

		return Stream;
	}

	k3d_data(boolean_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
	k3d_data(k3d::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_left;
	k3d_data(k3d::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_right;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, with_serialization) m_left_matrix;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, with_serialization) m_right_matrix;
};

k3d::iplugin_factory& cgal_boolean_factory()
{
	return cgal_boolean::get_factory();
}

}
