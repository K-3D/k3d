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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/irender_frame.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/scripted_node.h>

#define DEFAULT_SCRIPT "#python\n\n\
# Sample RenderEngineScript input\n\
#\n\
# Use the following context variables for rendering:\n\
#\n\
# \"Document\" - a reference to the owning document.\n\
# \"Node\" - a reference to the owning node.\n\
# \"VisibleNodes\" - the collection of nodes that should be\n\
#                  rendered, if possible.\n\
# \"OutputImage\" - string path to the user-selected output file.\n\
# \"ViewImage\" - boolean indicating whether the output should\n\
#               be displayed after rendering is complete.\n\
\n\
# This trivial example \"renders\" the document by writing\n\
# the name of each visible node to a text file.  The set of\n\
# visible nodes is chosen by the user at runtime via the\n\
# \"Visible Nodes\" property.\n\
\n\
import k3d\n\
k3d.check_node_environment(locals(), \"RenderEngineScript\")\n\
\n\
output = open(OutputImage, \"w\")\n\
\n\
for node in VisibleNodes:\n\
	output.write(k3d.dynamic_cast(node, \"iproperty_collection\").name + \"\\n\")\n\
\n\
output.close()\n\
\n\n"

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// render_engine_script\n\

class render_engine_script :
	public k3d::scripted_node<k3d::persistent<k3d::node> >,
	public k3d::irender_frame
{
	typedef k3d::scripted_node<k3d::persistent<k3d::node> > base;

public:
	render_engine_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_visible_nodes(init_owner(*this) + init_name("visible_nodes") + init_label(_("Visible Nodes")) + init_description(_("Visible Nodes")) + init_value(std::vector<k3d::inode*>()))
	{
		set_script(DEFAULT_SCRIPT);
	}

	bool render_frame(const k3d::filesystem::path& OutputImage, const bool ViewImage)
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		context["VisibleNodes"] = m_visible_nodes.property_value();
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


