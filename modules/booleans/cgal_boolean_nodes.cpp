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
		\brief Plugin to perform 3D boolean operations using CGAL
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "conversion.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/user_property_changed_signal.h>

namespace module
{

namespace booleans
{

/////////////////////////////////////////////////////////////////////////////
// cgal_boolean

class cgal_boolean :
	public k3d::imulti_mesh_sink,
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	cgal_boolean(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_type(init_owner(*this) + init_name("type") + init_label(_("Type")) + init_description(_("Boolean operation (intersection, union, difference, reverse difference)")) + init_value(BOOLEAN_INTERSECTION) + init_enumeration(boolean_values())),
		m_user_property_changed_signal(*this)	
	{
		m_type.changed_signal().connect(make_topology_changed_slot());
		m_user_property_changed_signal.connect(make_topology_changed_slot());
	}
	
	virtual ~cgal_boolean() {}
	
	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		CGAL::set_error_handler(k3d_failure_handler);
		
		Nef_polyhedron result;
		
		const k3d::iproperty_collection::properties_t properties = k3d::property::user_properties(*static_cast<k3d::iproperty_collection*>(this));
		try
		{
			if (m_type.pipeline_value() == BOOLEAN_REVERSE_DIFFERENCE)
			{
				bool started = false;
				for(k3d::iproperty_collection::properties_t::const_reverse_iterator prop = properties.rbegin(); prop != properties.rend(); ++prop)
				{
					k3d::iproperty& property = **prop;
					if(property.property_type() == typeid(k3d::mesh*))
					{
						const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(property));
						return_if_fail(mesh);
						return_if_fail(k3d::validate_polyhedra(*mesh));
						return_if_fail(k3d::is_solid(*mesh));
						
						boost::shared_ptr<Nef_polyhedron> operand = to_nef(*mesh);
						return_if_fail(operand.get());
						if (!started)
						{
							result = *operand;
							started = true;
						}
						else
						{
							result -= *operand;
						}
					}
				}
			}
			else
			{
				bool started = false;
				for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
				{
					k3d::iproperty& property = **prop;
					if(property.property_type() == typeid(k3d::mesh*))
					{
						const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(property));
						return_if_fail(mesh);
						return_if_fail(k3d::validate_polyhedra(*mesh));
						return_if_fail(k3d::is_solid(*mesh));
						
						boost::shared_ptr<Nef_polyhedron> operand = to_nef(*mesh);
						return_if_fail(operand.get());
						if (!started)
						{
							result = *operand;
							started = true;
						}
						else
						{
							switch(m_type.pipeline_value())
							{
								case BOOLEAN_INTERSECTION:
									result *= *operand;
									break;
					
								case BOOLEAN_UNION:
									result += *operand;
									break;
					
								case BOOLEAN_DIFFERENCE:
									result -= *operand;
									break;
							}
						}
					}
				}
			}
		}
		catch (...)
		{
			k3d::log() << error << "CGALBoolean: error executing boolean operation" << std::endl;
		}

		to_mesh(result, Mesh, static_cast<k3d::imaterial*>(0));
	}
	
	void on_update_mesh_geometry(k3d::mesh& Mesh) {}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cgal_boolean, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imulti_mesh_sink> > > factory(
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
	k3d::user_property_changed_signal m_user_property_changed_signal;
};

k3d::iplugin_factory& cgal_boolean_factory()
{
	return cgal_boolean::get_factory();
}

} // namespace booleans

} // namespace module
