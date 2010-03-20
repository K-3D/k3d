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
#include <k3dsdk/hints.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/value_demand_storage.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace test
{

/////////////////////////////////////////////////////////////////////////////
// check_point_attributes

class check_point_attributes :
	public k3d::node,
	public k3d::imesh_sink
{
	typedef k3d::node base;

public:
	check_point_attributes(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_ulps(init_owner(*this) + init_name("difference") + init_label(_("Difference")) + init_description(_("Difference in Units in the Last Place.")) + init_value(static_cast<k3d::uint64_t>(0))),
		m_equal(init_owner(*this) + init_name("equal") + init_label(_("Equal")) + init_description(_("True iff all input meshes are completely equivalent.")) + init_value(true)),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<k3d::mesh*>(0))
	{
		m_input_mesh.changed_signal().connect(k3d::hint::converter<
				k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(sigc::mem_fun(*this, &check_point_attributes::update)));

		m_ulps.set_update_slot(sigc::mem_fun(*this, &check_point_attributes::execute_ulps));
		m_equal.set_update_slot(sigc::mem_fun(*this, &check_point_attributes::execute_equal));
	}

	k3d::iproperty& mesh_sink_input()
	{
		return m_input_mesh;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<check_point_attributes,
			k3d::interface_list<k3d::imesh_sink > > factory(
				k3d::uuid(0x8ddbc83f, 0xe64e3850, 0x2503928c, 0x4d30760f),
				"CheckPointAttributes",
				_("Looks for k3d::point3 attibute arrays, and compares the contents to the corresponding point positions."),
				"Test",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::uint64_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_ulps;
	k3d_data(k3d::bool_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_equal;
	k3d_data(k3d::mesh*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_input_mesh;

	boost::optional<k3d::difference::accumulator> m_accumulator;

	void update(k3d::ihint*)
	{
		m_accumulator = boost::none;

		m_equal.update();
		m_ulps.update();
	}

	k3d::difference::accumulator accumulator()
	{
		if(!m_accumulator)
		{
			m_accumulator = k3d::difference::accumulator();
			const k3d::mesh* const input_mesh = m_input_mesh.pipeline_value();
			if(input_mesh)
			{
				const k3d::string_t table_name = "points";

				const k3d::mesh::points_t* point_attributes = input_mesh->point_attributes.lookup<k3d::mesh::points_t>(table_name);
				if(point_attributes)
					input_mesh->points->difference(*point_attributes, *m_accumulator);

				for(k3d::mesh::primitives_t::const_iterator p = input_mesh->primitives.begin(); p != input_mesh->primitives.end(); ++p)
				{
					const k3d::mesh::primitive& primitive = **p;

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

					for(k3d::mesh::named_tables_t::const_iterator attribute = primitive.attributes.begin(); attribute != primitive.attributes.end(); ++attribute)
					{
						const k3d::string_t& attribute_name = attribute->first;
						if(attribute_name != "vertex" && attribute_name != "edge")
							continue;

						const k3d::uint_t attribute_count = k3d::component_size(primitive, attribute_name);
						if(0 == attribute_count)
							continue;

						const k3d::table& attribute_table = attribute->second;

						k3d::mesh::points_t reference_points;

						for(k3d::uint_t i = 0; i != attribute_count; ++i)
							reference_points.push_back(input_mesh->points->at(indices->at(i)));

						const k3d::mesh::points_t* array = attribute_table.lookup<k3d::mesh::points_t>(table_name);
						if(array)
							reference_points.difference(*array, *m_accumulator);
					}

					// Specific cases for NURBS parameter attributes
					boost::scoped_ptr<k3d::nurbs_curve::const_primitive> curve_prim(k3d::nurbs_curve::validate(*input_mesh, primitive));
					if(curve_prim)
					{
						const k3d::uint_t curves_begin = 0;
						const k3d::uint_t curves_end = curve_prim->curve_first_points.size();
						k3d::mesh::points_t ref_points;
						const k3d::mesh::points_t* array = curve_prim->parameter_attributes.lookup<k3d::mesh::points_t>(table_name);
						if(!array)
							continue;
						for(k3d::uint_t curve = curves_begin; curve != curves_end; ++curve)
						{
							const k3d::uint_t first_point = curve_prim->curve_first_points[curve];
							const k3d::uint_t last_point = curve_prim->curve_point_counts[curve] + first_point - 1;
							ref_points.push_back(input_mesh->points->at(curve_prim->curve_points[first_point]));
							ref_points.push_back(input_mesh->points->at(curve_prim->curve_points[last_point]));
						}
						ref_points.difference(*array, *m_accumulator);
						continue;
					}

					boost::scoped_ptr<k3d::nurbs_patch::const_primitive> patch_prim(k3d::nurbs_patch::validate(*input_mesh, primitive));
					if(patch_prim)
					{
						const k3d::uint_t patches_begin = 0;
						const k3d::uint_t patches_end = patch_prim->patch_first_points.size();
						k3d::mesh::points_t ref_points;
						const k3d::mesh::points_t* array = patch_prim->parameter_attributes.lookup<k3d::mesh::points_t>(table_name);
						if(!array)
							continue;
						for(k3d::uint_t patch = patches_begin; patch != patches_end; ++patch)
						{
							// Point indices of the patch corners
							const k3d::uint_t u_count = patch_prim->patch_u_point_counts[patch];
							const k3d::uint_t v_count = patch_prim->patch_v_point_counts[patch];
							const k3d::uint_t c1 = patch_prim->patch_first_points[patch];
							const k3d::uint_t c2 = c1 + u_count-1;
							const k3d::uint_t c3 = c1 + u_count*(v_count-1);
							const k3d::uint_t c4 = c3 + u_count - 1;
							ref_points.push_back(input_mesh->points->at(c1));
							ref_points.push_back(input_mesh->points->at(c2));
							ref_points.push_back(input_mesh->points->at(c3));
							ref_points.push_back(input_mesh->points->at(c4));
						}
						ref_points.difference(*array, *m_accumulator);
					}
				}
			}
		}
		return *m_accumulator;
	}

	void execute_equal(const std::vector<k3d::ihint*>& Hints, k3d::bool_t& Output)
	{
		Output = boost::accumulators::min(accumulator().exact);
	}

	void execute_ulps(const std::vector<k3d::ihint*>& Hints, k3d::uint64_t& Output)
	{
		Output = boost::accumulators::max(accumulator().ulps);
	}
};

/////////////////////////////////////////////////////////////////////////////
// check_point_attributes_factory

k3d::iplugin_factory& check_point_attributes_factory()
{
	return check_point_attributes::get_factory();
}

} // namespace test

} // namespace module

