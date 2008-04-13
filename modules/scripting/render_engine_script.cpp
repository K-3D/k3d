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
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/inode_collection_sink.h>
#include <k3dsdk/irender_frame.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/scripted_node.h>

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// render_engine_script\n\

class render_engine_script :
	public k3d::scripted_node<k3d::persistent<k3d::node> >,
	public k3d::inode_collection_sink,
	public k3d::irender_frame
{
	typedef k3d::scripted_node<k3d::persistent<k3d::node> > base;

public:
	render_engine_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_visible_nodes(init_owner(*this) + init_name("visible_nodes") + init_label(_("Visible Nodes")) + init_description(_("Visible Nodes")) + init_value(std::vector<k3d::inode*>()))
	{
		set_script(k3d::resource::get_string("/module/scripting/render_engine_script.py"));
	}

	const k3d::inode_collection_sink::properties_t node_collection_properties()
	{
		return k3d::inode_collection_sink::properties_t(1, &m_visible_nodes);
	}

	bool render_frame(const k3d::filesystem::path& OutputImage, const bool ViewImage)
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		context["VisibleNodes"] = m_visible_nodes.property_internal_value();
		context["OutputImage"] = OutputImage;
		context["ViewImage"] = ViewImage;

		execute_script(context);

		return true;
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<render_engine_script > factory(
			k3d::uuid(0x1f85ec59, 0xb348e0d0, 0x6381b4a6, 0x938947f0),
			"RenderEngineScript",
			_("Script node that acts as a render engine"),
			"Scripting",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::inode_collection_property::nodes_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, node_collection_serialization) m_visible_nodes;
};

/////////////////////////////////////////////////////////////////////////////
// render_engine_script_factory

k3d::iplugin_factory& render_engine_script_factory()
{
	return render_engine_script::get_factory();
}

} // namespace scripting

} // namespace module


