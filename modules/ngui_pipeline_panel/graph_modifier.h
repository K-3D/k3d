#ifndef MODULES_NGUI_PIPELINE_PANEL_GRAPH_MODIFIER_H
#define MODULES_NGUI_PIPELINE_PANEL_GRAPH_MODIFIER_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
#include <k3dsdk/hints.h>
#include <k3dsdk/pointer_demand_storage.h>
#include <k3dsdk/property_collection.h>

namespace k3d
{

class graph_modifier :
	public property_collection
{
public:
	graph_modifier() :
		m_input(init_owner(*this) + init_name("input_graph") + init_label(_("Input Graph")) + init_description("Input graph") + init_value<graph::undirected*>(0)),
		m_output(init_owner(*this) + init_name("output_graph") + init_label(_("Output Graph")) + init_description("Output graph"))
	{
		m_output.set_update_slot(sigc::mem_fun(*this, &graph_modifier::execute));

		m_input.changed_signal().connect(hint::converter<
			hint::convert<hint::any, hint::unchanged> >(m_output.make_slot()));
	}

	iproperty& input()
	{
		return m_input;
	}

	iproperty& output()
	{
		return m_output;
	}

	sigc::slot<void, ihint*> make_update_graph_slot()
	{
		return m_output.make_slot();
	}

protected:
	k3d_data(graph::undirected*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input;
	k3d_data(graph::undirected*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, read_only_property, no_serialization) m_output;

private:
	void execute(const std::vector<ihint*>& Hints, graph::undirected& Output)
	{
		if(const graph::undirected* const input = m_input.pipeline_value())
		{
			bool_t update_topology = false;
			bool_t update_attributes = false;

			for(uint_t i = 0; i != Hints.size(); ++i)
			{
				// Update attributes ...
				if(dynamic_cast<hint::bitmap_pixels_changed*>(Hints[i]))
				{
					update_attributes = true;
				}
				// In every other case (graph_topology_changed, unknown hint, or no hint),
				// we assume the worst and recreate everything from scratch ...
				else
				{
					update_topology = true;
					update_attributes = true;
					break;
				}
			}

			if(update_topology)
			{
				on_update_graph_topology(*input, Output);
			}

			if(update_attributes)
			{
				on_update_graph_attributes(*input, Output);
			}
		}
	}

	virtual void on_update_graph_topology(const graph::undirected& Input, graph::undirected& Output) = 0;
	virtual void on_update_graph_attributes(const graph::undirected& Input, graph::undirected& Output) = 0;
};

} // namespace k3d

#endif // !MODULES_NGUI_PIPELINE_PANEL_GRAPH_MODIFIER_H

