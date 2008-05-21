// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>

namespace module
{

namespace test
{

/////////////////////////////////////////////////////////////////////////////
// pipeline_profiler

class pipeline_profiler :
	public k3d::node
{
	typedef k3d::node base;

public:
	pipeline_profiler(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_records(init_owner(*this) + init_name("records") + init_label(_("Records")) + init_description(_("Contains records storing the execution time for individual node tasks.")) + init_value(records_t()))
	{
		Document.pipeline_profiler().connect_node_execution_signal(sigc::mem_fun(*this, &pipeline_profiler::on_node_execution));
		Document.nodes().rename_node_signal().connect(sigc::mem_fun(*this, &pipeline_profiler::on_node_renamed));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<pipeline_profiler> factory(
			k3d::uuid(0x418d0935, 0x12495e73, 0xbcd046a2, 0x92bf7fb1),
			"PipelineProfiler",
			_("Records data profiling execution of the visualization pipeline"),
			"Test",
			k3d::iplugin_factory::EXPERIMENTAL
			);

		return factory;
	}

private:
	/// Called by the signal system when profile data arrives
	void on_node_execution(k3d::inode& Node, const k3d::string_t& Task, double Time)
	{
		records_t& records = m_records.internal_value();
		records[&Node][Task] = Time;

		// Because we're modifying the internal state of the property by-reference, we have to call the "changed" signal explicitly ...
		m_records.changed_signal().emit(0);
	}
	
	/// Called by the signal system anytime a node is renamed
	void on_node_renamed(k3d::inode*)
	{
	}

	/// Stores profiling events
	typedef std::map<k3d::inode*, std::map<k3d::string_t, k3d::double_t> > records_t;
	k3d_data(records_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_records;
};

/////////////////////////////////////////////////////////////////////////////
// pipeline_profiler_factory

k3d::iplugin_factory& pipeline_profiler_factory()
{
	return pipeline_profiler::get_factory();
}

} // namespace test

} // namespace module

