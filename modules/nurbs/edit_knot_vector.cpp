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

#include "nurbs_curves.h"

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/ireset_properties.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <vector>
#include <sstream>

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

namespace detail
{
	k3d::bool_t validate_knot_vector(const k3d::mesh::knots_t& Knots, const k3d::uint_t Order)
	{
		const k3d::uint_t knot_count = Knots.size();
		for(k3d::uint_t i = 0; i != (knot_count-1); ++i)
		{
			return_val_if_fail(Knots[i+1] >= Knots[i], false);
			return_val_if_fail(multiplicity(Knots, Knots[i], 1, knot_count-2) < Order, false);
		}
		return true;
	}
} // namespace detail

class edit_knot_vector :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >,
	public k3d::ireset_properties
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	edit_knot_vector(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_knot_vector(init_owner(*this) + init_name("knot_vector") + init_label(_("Knot Vector")) + init_description(_("Enter a new knot vector containing knot values separated with spaces.")) + init_value(k3d::mesh::knots_t()))
	{
		m_knot_vector.set_metadata_value(k3d::metadata::key::role(), k3d::metadata::value::nurbs_knot_vector_role());
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_knot_vector.changed_signal().connect(make_update_mesh_slot());
	}

	void reset_properties()
	{
		k3d::log() << debug << "resetting knot vector property value" << std::endl;
		const k3d::mesh* input_mesh = m_input_mesh.pipeline_value();
		if(!input_mesh)
			return;
		k3d::mesh input_with_selections = *input_mesh;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), input_with_selections);
		k3d::double_t order;
		visit_selected_curves(input_with_selections, max_order_calculator(order));
		k3d::mesh elevated_mesh = input_with_selections;
		modify_selected_curves(input_with_selections, elevated_mesh, degree_elevator(order));
		k3d::mesh::knots_t knots;
		visit_selected_curves(elevated_mesh, knot_vector_calculator(knots));

		m_knot_vector.set_value(knots);
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}


	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		// Get the maximum order
		k3d::double_t order;
		visit_selected_curves(Output, max_order_calculator(order));

		// Elevate all selected curves to the maximal order
		k3d::mesh elevated_mesh = Output;
		modify_selected_curves(Input, elevated_mesh, degree_elevator(order));

		// Validate the replacement knot vector
		const k3d::mesh::knots_t knots = m_knot_vector.pipeline_value();
		return_if_fail(detail::validate_knot_vector(knots, order));

		// Apply the new knot vector to all selected curves
		modify_selected_curves(elevated_mesh, Output, knot_vector_merger(knots, order));
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
	k3d::metadata::property< k3d_data(k3d::mesh::knots_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, array_serialization) > m_knot_vector;
};

k3d::iplugin_factory& edit_knot_vector_factory()
{
	return edit_knot_vector::get_factory();
}

} //namespace nurbs

} //namespace module

