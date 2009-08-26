// K-3D
// Copyright (c) 2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\author Romain Behar (romainbehar@yahoo.com)
		\author Timothy M. Shead (tshead@k-3d.com)
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include "mesh_modifier.h"
#include <k3dsdk/node.h>
#include <k3dsdk/properties.h>

namespace module
{

namespace deformation
{

/////////////////////////////////////////////////////////////////////////////
// tweak_points

class tweak_points :
	public _k3d::mesh_modifier<k3d::node>
{
	typedef _k3d::mesh_modifier<k3d::node> base;
	typedef std::pair<k3d::mesh::indices_t, k3d::mesh::points_t> tweaks_t;

public:
	tweak_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_tweaks(init_owner(*this) + init_name("tweaks") + init_label(_("Tweaks")) + init_description(_("A pair of indices_t and points_t arrays, indicating which positions to set")) + init_value(tweaks_t()))
	{
		m_tweaks.changed_signal().connect(k3d::hint::converter<
				k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_update_mesh_slot()));
	}

	void on_execute(const std::vector<k3d::ihint*>& Hints, const k3d::mesh& Input, k3d::mesh& Output)
	{
		k3d::bool_t reset = Hints.empty();
		for(k3d::uint_t i = 0; i != Hints.size(); ++i)
		{
			if(!dynamic_cast<k3d::hint::mesh_geometry_changed*>(Hints[i]) && !dynamic_cast<k3d::hint::selection_changed*>(Hints[i]))
				reset = true;
		}

		if(reset)
		{
			Output = Input;
			if(!m_tweaked_points || m_tweaked_points->size() != Output.points->size())
				m_tweaked_points = Output.points;
		}

		k3d::mesh::points_t& output_points = m_tweaked_points.writable();

		const tweaks_t tweaks = m_tweaks.pipeline_value();
		const k3d::uint_t tweaks_begin = 0;
		const k3d::uint_t tweaks_end = tweaks.first.size();
		const k3d::uint_t point_count = output_points.size();
		return_if_fail(tweaks_end == tweaks.second.size());
		k3d::state_change_set* change_set = document().state_recorder().current_change_set();
		if(change_set)
		{
			k3d::mesh::points_t old_positions;
			for(k3d::uint_t i = tweaks_begin; i != tweaks_end; ++i)
			{
				const k3d::uint_t point_idx = tweaks.first[i];
				if(point_idx < point_count)
					old_positions.push_back(output_points[point_idx]);
			}
			// If undo/redo is being recorded, we need to store the old positions at the new tweak indices for the old state:
			change_set->record_old_state(new tweaks_container(m_tweaks, tweaks.first, old_positions));
			// The new state is simply the new tweak indices and the new positions:
			change_set->record_new_state(new tweaks_container(m_tweaks, tweaks.first, tweaks.second));
		}

		for(k3d::uint_t i = tweaks_begin; i != tweaks_end; ++i)
		{
			const k3d::uint_t point_idx = tweaks.first[i];
			if(point_idx < point_count)
				output_points[point_idx] = tweaks.second[i];
		}
		Output.points = m_tweaked_points;
	}

	void save(k3d::xml::element& Element, const k3d::ipersistent::save_context& Context)
	{
		base::save(Element, Context);

		k3d::xml::element& tweaks = Element.append(k3d::xml::element("tweaks"));

		if(!m_input_mesh.pipeline_value())
			return;

		const k3d::mesh::points_t& input_points = *m_input_mesh.pipeline_value()->points;
		const k3d::mesh::points_t& output_points = *m_output_mesh.pipeline_value()->points;

		for(k3d::uint_t point = 0; point != input_points.size(); ++point)
		{
			// Note: exact compare should work here, since we copied from the original array
			if(input_points[point] != output_points[point])
				tweaks.append(k3d::xml::element("tweak", k3d::xml::attribute("index", point), k3d::xml::attribute("value", output_points[point])));
		}
	}

	void load(k3d::xml::element& Element, const k3d::ipersistent::load_context& Context)
	{
		base::load(Element, Context);

		k3d::xml::element* const tweaks = k3d::xml::find_element(Element, "tweaks");
		return_if_fail(tweaks);

		tweaks_t loaded_tweaks;
		size_t count = 0;
		for(k3d::xml::element::elements_t::iterator element = tweaks->children.begin(); element != tweaks->children.end(); ++element)
		{
			if(element->name == "tweak")
			{
				size_t index = k3d::xml::attribute_value<size_t>(*element, "index", count);
				k3d::point3 value = k3d::xml::attribute_value<k3d::point3>(*element, "value", k3d::point3(0, 0, 0));
				if (value != k3d::point3(0,0,0)) // Skip zeros from old file format
				{
					loaded_tweaks.first.push_back(index);
					loaded_tweaks.second.push_back(value);
				}
				++count;
			}
			else
			{
				k3d::log() << warning << k3d_file_reference << ": unknown element \"" << element->name << "\" will be ignored ... " << std::endl;
			}
		}
		m_tweaks.set_value(loaded_tweaks);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<tweak_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xed302b87, 0x49bf4fe6, 0x99064963, 0x17ec12d9),
				"TweakPoints",
				_("Sets the point positions at the given indices"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(tweaks_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_tweaks;
	/// Stores the cumulative result of all the tweaks
	k3d::pipeline_data<k3d::mesh::points_t> m_tweaked_points;

	class tweaks_container :
		public k3d::istate_container
	{
	public:
		tweaks_container(k3d::iproperty& Tweaks, const k3d::mesh::indices_t& StoredIndices, const k3d::mesh::points_t& StoredPoints) :
			m_tweaks(Tweaks),
			m_stored_indices(StoredIndices),
			m_stored_points(StoredPoints)
		{
		}

		void restore_state()
		{
			k3d::property::set_internal_value(m_tweaks, std::make_pair(m_stored_indices, m_stored_points));
		}

	private:
		k3d::iproperty& m_tweaks;
		const k3d::mesh::indices_t m_stored_indices;
		const k3d::mesh::points_t m_stored_points;
	};
};

/////////////////////////////////////////////////////////////////////////////
// tweak_points_factory

k3d::iplugin_factory& tweak_points_factory()
{
	return tweak_points::get_factory();
}

} // namespace deformation

} // namespace module



