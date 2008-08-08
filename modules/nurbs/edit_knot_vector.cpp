// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\author Carsten Haubold (CarstenHaubold@web.de)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/ireset_properties.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/metadata.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/data.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/shared_pointer.h>

#include <iostream>
#include <vector>
#include <sstream>

#include "nurbs_curve_modifier.h"

namespace k3d
{
	namespace data
	{
		/// Serialization policy for data containers that can be serialized as XML
		template<typename value_t, class property_policy_t>
		class array_serialization :
					public property_policy_t,
					public ipersistent
		{
				// This policy only works for arrays
				BOOST_STATIC_ASSERT((boost::is_base_and_derived<array, typename boost::remove_pointer<value_t>::type>::value));

			public:
				void save(xml::element& Element, const ipersistent::save_context& Context)
				{
					value_t& Array = property_policy_t::internal_value();
					typename value_t::const_iterator item = Array.begin();
					const typename value_t::const_iterator end = Array.end();

					std::ostringstream buffer;

					if (item != end)
						buffer << *item++;
					for (; item != end; ++item)
						buffer << " " << *item;

					xml::element Storage("property", buffer.str(), xml::attribute("name", property_policy_t::name()));
					Element.append(Storage);
				}

				void load(xml::element& Element, const ipersistent::load_context& Context)
				{
					typename value_t::value_type value;
					value_t Array;

					std::istringstream buffer(Element.text);
					while (true)
					{
						buffer >> value;

						if (!buffer)
							break;

						Array.push_back(value);
					}

					property_policy_t::set_value(Array);
				}

			protected:
				template<typename init_t>
				array_serialization(const init_t& Init) :
						property_policy_t(Init)
				{
					Init.persistent_collection().enable_serialization(Init.name(), *this);
				}
		};
	}
}

namespace module
{

	namespace nurbs
	{

		class edit_knot_vector :
					public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >,
					public k3d::ireset_properties
		{
				typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
			public:
				edit_knot_vector(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
						base(Factory, Document),
						m_knot_vector(init_owner(*this) + init_name("knot_vector") + init_label(_("Knot Vector")) + init_description(_("Enter a new knot vector containing knot values separated with spaces.")) + init_value(k3d::mesh::knots_t()) )
				{
					m_knot_vector.set_metadata("k3d:property-type", "k3d:nurbs-knot-vector");
					m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
					m_knot_vector.changed_signal().connect(make_update_mesh_slot());
				}

				void reset_properties()
				{
					m_knot_vector.set_value(extract_knots(*m_input_mesh.pipeline_value(), m_curve));
				}

				void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
				{
					Output = Input;

					if (!k3d::validate_nurbs_curve_groups(Output))
						return;

					merge_selection(m_mesh_selection.pipeline_value(), Output);

					nurbs_curve_modifier mod(Output);

					m_curve = mod.selected_curve();

					if (m_curve < 0)
					{
						k3d::log() << error << "More than one curve or no curve selected! " << m_curve << std::endl;
						return;
					}

					const k3d::mesh::knots_t& knots = m_knot_vector.pipeline_value();

					if (!insert_knots(knots, Output, m_curve))
						k3d::log() << error << "Invalid Knot Vector on curve " << m_curve << std::endl;
				}


				void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
				{
					Output = Input;

					if (!k3d::validate_nurbs_curve_groups(Output))
						return;

					merge_selection(m_mesh_selection.pipeline_value(), Output);

					const k3d::mesh::knots_t& knots = m_knot_vector.pipeline_value();

					nurbs_curve_modifier mod(Output);

					m_curve = mod.selected_curve();

					if (m_curve < 0)
					{
						k3d::log() << error << "More than one curve or no curve selected! " << m_curve << std::endl;
						return;
					}

					if (!insert_knots(knots, Output, m_curve))
						k3d::log() << error << "Invalid Knot Vector on curve " << m_curve << std::endl;
				}

				static k3d::iplugin_factory& get_factory()
				{
					static k3d::document_plugin_factory<edit_knot_vector, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
					    k3d::uuid(0x8ff0922c, 0xf6412c8d, 0x2cf9c5a3, 0x54bdd064),
					    "NurbsEditCurveKnotVector",
					    _("Edit the knot vector of a NURBS curve"),
					    "NURBS",
					    k3d::iplugin_factory::EXPERIMENTAL);

					return factory;
				}

			private:
				k3d::metadata_property < k3d_data(k3d::mesh::knots_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, array_serialization) > m_knot_vector;
				int m_curve;

				k3d::mesh::knots_t extract_knots(const k3d::mesh& Output, int curve)
				{
					const k3d::mesh::nurbs_curve_groups_t& groups = *Output.nurbs_curve_groups;
					const k3d::mesh::knots_t& knots = *groups.curve_knots;
					k3d::mesh::knots_t curve_knots;

					const size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
					const size_t curve_knots_end = curve_knots_begin + (*groups.curve_point_counts)[curve] + (*groups.curve_orders)[curve];

					for (size_t i = curve_knots_begin; i < curve_knots_end; i++)
						curve_knots.push_back( knots[i] );

					return curve_knots;
				}

				bool insert_knots(const k3d::mesh::knots_t& curve_knots, k3d::mesh& Output, int curve)
				{
					k3d::mesh::nurbs_curve_groups_t& groups = *k3d::make_unique(Output.nurbs_curve_groups);
					k3d::mesh::knots_t& knots = *k3d::make_unique(groups.curve_knots);

					const size_t curve_knots_begin = (*groups.curve_first_knots)[curve];
					const size_t curve_knots_end = curve_knots_begin + (*groups.curve_point_counts)[curve] + (*groups.curve_orders)[curve];

					if (curve_knots.size() != curve_knots_end - curve_knots_begin)
						return false;

					for (size_t i = curve_knots_begin; i < curve_knots_end; i++)
						knots[i] = curve_knots[i-curve_knots_begin];

					return true;
				}


		};

		k3d::iplugin_factory& edit_knot_vector_factory()
		{
			return edit_knot_vector::get_factory();
		}

	} //namespace nurbs

} //namespace module
