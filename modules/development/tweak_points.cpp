// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Bart Janssens (bart.janssens@lid.kviv.be)
 */

#include <k3dsdk/algebra.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/icommand_node.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/xml.h>

#include <list>

namespace libk3ddevelopment
{

	/////////////////////////////////////////////////////////////////////////////
// tweak_points

class tweak_points :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::persistent<k3d::node> > >,
	public k3d::icommand_node
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::persistent<k3d::node> > > base;
	typedef std::map<size_t, k3d::point3> tweaks_t;
public:
	tweak_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_matrix(init_owner(*this) + init_name("matrix") + init_label(_("Matrix")) + init_description(_("Tweaks matrix")) + init_value(k3d::identity3D())),
		m_center(init_owner(*this) + init_name("center") + init_label(_("Transformation Center")) + init_description(_("Center around which scaling or rotation happen")) + init_value(k3d::point3(0.0,0.0,0.0))),
		m_selected_points(init_owner(*this) + init_name("selected_points") + init_label(_("Selected points")) + init_description(_("Indices of points being transformed")) + init_value(0)),
		m_tweaks(init_owner(*this) + init_name("tweaks") + init_label(_("Tweaks")) + init_description(_("Map containing the difference vectors for all transformations performed by this node")) + init_value(tweaks_t())),
		m_selection_copied(false)
	{
		m_matrix.changed_signal().connect(sigc::mem_fun(*this, &tweak_points::on_matrix_changed));
		m_selected_points.changed_signal().connect(sigc::mem_fun(*this, &tweak_points::on_selected_points_changed));
		m_tweaks.changed_signal().connect(sigc::mem_fun(*this, &tweak_points::on_tweaks_changed));
	}
	
	void on_matrix_changed(k3d::iunknown* Hint)
	{
		m_hint.transformation_matrix = m_matrix.value();
		m_output_mesh.update(&m_hint);
	}

	/// If the mesh was reset, see if we can apply tweaks and if not clear them
	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::make_unique(Output.points);
		m_input_points = Input.points;
		m_output_points = Output.points;
		
		const tweaks_t tweaks = m_tweaks.internal_value();
		if (tweaks.empty())
			return;
		tweaks_t::const_iterator last = tweaks.end();
		--last;
		size_t max_index = last->first;
		if (!Output.points || max_index > Output.points->size())
		{
			m_tweaks.set_value(tweaks_t());
			return;
		}
		on_tweaks_changed(update_hint());
	}
	
	/// Mesh modifier implementation
	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(!Input.points)
			return;
		if(!Output.points)
			return;
		return_if_fail(Input.points->size() == Output.points->size());
		
		if (!m_selection_copied)
		{
			replace_selection(m_mesh_selection.value(), Output);
			m_selection_copied = true;
		}
		return_if_fail(Output.point_selection);
		return_if_fail(Output.point_selection->size() == Output.points->size());

		const k3d::mesh::points_t& input_points = *Input.points;
		k3d::mesh::points_t& output_points = *(const_cast<k3d::mesh::points_t*>(Output.points.get()));
		m_input_points = Input.points;
		m_output_points = Output.points;
		
		const k3d::mesh::indices_t selected_points = m_selected_points.value();
		m_hint.transformation_matrix = m_matrix.value();
		k3d::point3 center = m_center.value();
		for (size_t i = 0; i != selected_points.size(); ++i)
		{
			k3d::point3 position = input_points[selected_points[i]] + m_selected_tweaks[i];
			output_points[selected_points[i]] = m_hint.transformation_matrix * (position - center) + center;
		}
	}
	
	virtual iunknown* hint() const
	{
		return k3d::hint::mesh_address_changed();
	}
	
	/// Update affected mesh components in the hint. This needs to be done only when the selection changed
	void on_selected_points_changed(k3d::iunknown* Hint)
	{
		const k3d::mesh::indices_t selected_points = m_selected_points.value();
		const size_t selected_count = selected_points.size();

		// reset hint
		m_hint.changed_points.clear();

		// Update hint
		for (size_t point_index = 0; point_index != selected_count; ++point_index)
		{
			m_hint.changed_points.push_back(selected_points[point_index]);
		}
		m_selection_copied = false;

		update_selected_tweaks();
	}

	/// Needed for undo
	void on_tweaks_changed(k3d::iunknown* Hint)
	{
		if (!m_input_points)
			return;
		if (!m_output_points)
			return;
		const tweaks_t tweaks = m_tweaks.internal_value();
		const k3d::mesh::points_t& input_points = *m_input_points;
		k3d::mesh::points_t& output_points = *const_cast<k3d::mesh::points_t*>(m_output_points.get());
		return_if_fail(output_points.size() == input_points.size());
		if (!dynamic_cast<internal_update_hint*>(Hint)) // external change to hints: restore all point positions
		{
			for (size_t point = 0; point != output_points.size(); ++point)
				output_points[point] = input_points[point];
		}
		for (tweaks_t::const_iterator tweak = tweaks.begin(); tweak != tweaks.end(); ++tweak)
			output_points[tweak->first] = input_points[tweak->first] + tweak->second;

		update_selected_tweaks();
		
		if (!dynamic_cast<internal_update_hint*>(Hint))
		{
			k3d::log() << debug << "TweakPoints: Emitting 0 hint!" << std::endl;
			m_output_mesh.update(0);
		}
	}
		
	/// k3d::comand_node implementation
	const result execute_command(const std::string& Command, const std::string& Arguments)
	{
		// Handle errors
		if (Command != "stop_drag")
			return k3d::icommand_node::RESULT_UNKNOWN_COMMAND;
		if (!m_input_points)
			return k3d::icommand_node::RESULT_ERROR;
		if (!m_output_points)
			return k3d::icommand_node::RESULT_ERROR;
		
		const k3d::mesh::indices_t selected_points = m_selected_points.value();
		const size_t selection_count = selected_points.size();
		tweaks_t tweaks = m_tweaks.value();
		const k3d::mesh::points_t& input_points = *m_input_points;
		const k3d::mesh::points_t& output_points = *m_output_points;
		m_selected_tweaks.clear();
		for (size_t i = 0; i != selection_count; ++i) // update tweaks map
		{
 			return_val_if_fail(selected_points[i] < output_points.size(), RESULT_ERROR);
 			return_val_if_fail(selected_points[i] < input_points.size(), RESULT_ERROR);
			tweaks_t::iterator tweak = tweaks.find(selected_points[i]);
			if (tweak == tweaks.end())
				tweak = (tweaks.insert(std::make_pair(selected_points[i], k3d::point3(0.0,0.0,0.0)))).first;
			tweak->second = output_points[selected_points[i]] - input_points[selected_points[i]];
			m_selected_tweaks.push_back(tweak->second);
		}
		m_tweaks.set_value(tweaks, update_hint());
		return k3d::icommand_node::RESULT_CONTINUE;
	}

	/// Saves new file format
	void save(k3d::xml::element& Element, const k3d::ipersistent::save_context& Context)
	{
		base::save(Element, Context);

		k3d::xml::element& tweaks = Element.append(k3d::xml::element("tweaks"));

		const tweaks_t& tweak_array = m_tweaks.internal_value();
		for(tweaks_t::const_iterator tweak_i = tweak_array.begin(); tweak_i != tweak_array.end(); ++tweak_i)
		{
			tweaks.append(k3d::xml::element("tweak", k3d::xml::attribute("index", tweak_i->first), k3d::xml::attribute("value", tweak_i->second)));
		}
	}

	/// Loads both old and new fileformats
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
					loaded_tweaks.insert(std::make_pair(index, value));
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
				k3d::uuid(0xb9c5bae2, 0x01df44d4, 0x86c395e9, 0x34a11c31),
		"NewTweakPoints",
		_("Tweaks selected points using the given matrix"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
	
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_matrix;
	k3d_data(k3d::point3, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_center;
	k3d_data(k3d::mesh::indices_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_selected_points; // no undo to ensure synchronisation with the actual selection
	k3d_data(tweaks_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_tweaks;
	
private:
	k3d::hint::mesh_geometry_changed_t m_hint;
	boost::shared_ptr<const k3d::mesh::points_t> m_input_points; // cached for access in on_drag_changed
	boost::shared_ptr<const k3d::mesh::points_t> m_output_points; // cached for access in on_drag_changed
	std::vector<k3d::point3> m_selected_tweaks; // Cache for fast access to tweaks needed during drag motion
	bool m_selection_copied;

	/// Used to pass along the fact that we are doing an internal update on the tweaks
	class internal_update_hint : public k3d::iunknown {};
	/// Convenience function to get a static instance of the above hint
	internal_update_hint* update_hint()
	{
		static internal_update_hint hint;
		return &hint;
	}
	
	/// Update fast selected tweaks cache
	void update_selected_tweaks()
	{
		const k3d::mesh::indices_t selected_points = m_selected_points.value();
		const size_t selected_count = selected_points.size();
		m_selected_tweaks.clear();
		const tweaks_t& tweaks = m_tweaks.internal_value();
		for (size_t point_index = 0; point_index != selected_count; ++point_index)
		{
			tweaks_t::const_iterator tweak = tweaks.find(selected_points[point_index]);
			if (tweak != tweaks.end())
			{
				m_selected_tweaks.push_back(tweak->second);
			}
			else
			{
				m_selected_tweaks.push_back(k3d::point3(0.0,0.0,0.0));
			}
		}
		m_matrix.set_value(k3d::identity3D());
	}
};

/////////////////////////////////////////////////////////////////////////////
// tweak_points_factory

k3d::iplugin_factory& tweak_points_factory()
{
	return tweak_points::get_factory();
}

} // namespace libk3ddevelopment
