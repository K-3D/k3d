#ifndef GRAPH_MODIFIER_H
#define GRAPH_MODIFIER_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/data.h>
#include <k3dsdk/graph.h>
#include <k3dsdk/property_collection.h>

namespace module
{

namespace ngui
{

namespace pipeline
{

class graph_modifier :
	public k3d::property_collection
{
public:
	graph_modifier() :
		m_input(init_owner(*this) + init_name("input_graph") + init_label(_("Input Graph")) + init_description("Input graph") + init_value<k3d::graph*>(0)),
		m_output(init_owner(*this) + init_name("output_graph") + init_label(_("Output Graph")) + init_description("Output graph"))
	{
		m_input.changed_signal().connect(make_reset_graph_slot());

		m_output.set_initialize_slot(sigc::mem_fun(*this, &graph_modifier::initialize_graph));
		m_output.set_update_slot(sigc::mem_fun(*this, &graph_modifier::update_graph));
	}

	k3d::iproperty& input()
	{
		return m_input;
	}

	k3d::iproperty& output()
	{
		return m_output;
	}

	sigc::slot<void, k3d::iunknown*> make_reset_graph_slot()
	{
		return m_output.make_reset_slot();
	}

	sigc::slot<void, k3d::iunknown*> make_update_graph_slot()
	{
		return m_output.make_update_slot();
	}

protected:
	k3d_data(k3d::graph*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_input;
	k3d_data(k3d::graph*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::pointer_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_output;

private:
	void initialize_graph(k3d::graph& Output)
	{
		if(const k3d::graph* const input = m_input.pipeline_value())
		{
			on_initialize_graph(*input, Output);
			on_update_graph(*input, Output);
		}
	}

	void update_graph(k3d::graph& Output)
	{
		if(const k3d::graph* const input = m_input.pipeline_value())
		{
			on_update_graph(*input, Output);
		}
	}

	virtual void on_initialize_graph(const k3d::graph& Input, k3d::graph& Output) = 0;
	virtual void on_update_graph(const k3d::graph& Input, k3d::graph& Output) = 0;
};

} // namespace pipeline

} // namespace ngui

} // namespace module

#endif // !GRAPH_MODIFIER

