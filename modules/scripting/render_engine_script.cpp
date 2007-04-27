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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/istill_render_engine.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/scripted_node.h>

#define DEFAULT_SCRIPT "#python\n\n\
\n\n"

namespace libk3dscripting
{

/////////////////////////////////////////////////////////////////////////////
// render_engine_script

class render_engine_script :
	public k3d::scripted_node<k3d::persistent<k3d::node> >,
	public k3d::istill_render_engine
{
	typedef k3d::scripted_node<k3d::persistent<k3d::node> > base;

public:
	render_engine_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(DEFAULT_SCRIPT);
	}

	bool render_frame(const k3d::filesystem::path& OutputImage, const bool ViewImage)
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = static_cast<k3d::iunknown*>(&document());
		context["Node"] = static_cast<k3d::iunknown*>(this);
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
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// render_engine_script_factory

k3d::iplugin_factory& render_engine_script_factory()
{
	return render_engine_script::get_factory();
}

} // namespace libk3dscripting

