// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/component.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace test
{

/////////////////////////////////////////////////////////////////////////////
// add_point_attributes

class add_point_attributes :
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;

public:
	add_point_attributes(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		const k3d::string_t table_name = "points";


		const k3d::uint_t attribute_count = Output.points ? Output.points->size() : 0;
		k3d::mesh::points_t& array = Output.point_attributes.create(table_name, new k3d::mesh::points_t(attribute_count));
		for(k3d::uint_t i = 0; i != attribute_count; ++i)
			array[i] = Output.points->at(i);

		for(k3d::mesh::primitives_t::iterator p = Output.primitives.begin(); p != Output.primitives.end(); ++p)
		{
			k3d::mesh::primitive& primitive = p->writable();

			const k3d::mesh::indices_t* indices = 0;
			for(k3d::mesh::named_tables_t::const_iterator structure = primitive.structure.begin(); structure != primitive.structure.end(); ++structure)
			{
				for(k3d::mesh::table_t::const_iterator array = structure->second.begin(); array != structure->second.end(); ++array)
				{
					if(array->second->get_metadata_value(k3d::metadata::key::domain()) == k3d::metadata::value::point_indices_domain())
						indices = dynamic_cast<const k3d::mesh::indices_t*>(array->second.get());
				}
			}

			if(!indices)
				continue;

			for(k3d::mesh::named_tables_t::iterator attribute = primitive.attributes.begin(); attribute != primitive.attributes.end(); ++attribute)
			{
				const k3d::string_t& attribute_name = attribute->first;
				if(attribute_name != "vertex" && attribute_name != "edge")
					continue;

				const k3d::uint_t attribute_count = k3d::component_size(primitive, attribute_name);
				if(0 == attribute_count)
					continue;

				k3d::table& attribute_table = attribute->second;

				k3d::mesh::points_t& array = attribute_table.create(table_name, new k3d::mesh::points_t(attribute_count));
				for(k3d::uint_t i = 0; i != attribute_count; ++i)
					array[i] = Output.points->at(indices->at(i));
			}

			// Specific cases for NURBS parameter attributes
			boost::scoped_ptr<k3d::nurbs_curve::primitive> curve_prim(k3d::nurbs_curve::validate(Output, *p));
			if(curve_prim)
			{
				const k3d::uint_t curves_begin = 0;
				const k3d::uint_t curves_end = curve_prim->curve_first_points.size();
				k3d::mesh::points_t& array = curve_prim->parameter_attributes.create(table_name, new k3d::mesh::points_t());
				for(k3d::uint_t curve = curves_begin; curve != curves_end; ++curve)
				{
					const k3d::uint_t first_point = curve_prim->curve_first_points[curve];
					const k3d::uint_t last_point = curve_prim->curve_point_counts[curve] + first_point - 1;
					array.push_back(Output.points->at(curve_prim->curve_points[first_point]));
					array.push_back(Output.points->at(curve_prim->curve_points[last_point]));
				}
				continue;
			}

			boost::scoped_ptr<k3d::nurbs_patch::primitive> patch_prim(k3d::nurbs_patch::validate(Output, *p));
			if(patch_prim)
			{
				const k3d::uint_t patches_begin = 0;
				const k3d::uint_t patches_end = patch_prim->patch_first_points.size();
				k3d::mesh::points_t& array = patch_prim->parameter_attributes.create(table_name, new k3d::mesh::points_t());
				for(k3d::uint_t patch = patches_begin; patch != patches_end; ++patch)
				{
					// Point indices of the patch corners
					const k3d::uint_t u_count = patch_prim->patch_u_point_counts[patch];
					const k3d::uint_t v_count = patch_prim->patch_v_point_counts[patch];
					const k3d::uint_t c1 = patch_prim->patch_first_points[patch];
					const k3d::uint_t c2 = c1 + u_count-1;
					const k3d::uint_t c3 = c1 + u_count*(v_count-1);
					const k3d::uint_t c4 = c3 + u_count - 1;
					array.push_back(Output.points->at(c1));
					array.push_back(Output.points->at(c2));
					array.push_back(Output.points->at(c3));
					array.push_back(Output.points->at(c4));
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<add_point_attributes,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x4fd40bc0, 0xe740db8c, 0x0566faad, 0x19093e0f),
				"AddPointAttributes",
				_("Copies point coordinates to the relevant attribute tables in a mesh, primarily for testing purposes."),
				"Test",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// add_point_attributes_factory

k3d::iplugin_factory& add_point_attributes_factory()
{
	return add_point_attributes::get_factory();
}

} // namespace test

} // namespace module

