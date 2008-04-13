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
#include <k3dsdk/int32_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/scripted_node.h>
#include <k3dsdk/type_registry.h>

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// int32_source_script

class int32_source_script :
	public k3d::scripted_node<k3d::persistent<k3d::node> >,
	public k3d::int32_source<int32_source_script>
{
	typedef k3d::scripted_node<k3d::persistent<k3d::node> > base;

public:
	int32_source_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(k3d::resource::get_string("/module/scripting/int32_source_script.py"));

		connect_script_changed_signal(make_reset_int32_slot());
	}

	k3d::int32_t on_create_int32()
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		context["Output"] = 0;

		execute_script(context);

		if(context["Output"].type() == typeid(k3d::int32_t))
			return boost::any_cast<k3d::int32_t>(context["Output"]);

		k3d::log() << error << "unsupported output type: " << k3d::demangle(context["Output"].type()) << std::endl;

		return 0;
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<int32_source_script, k3d::interface_list<k3d::iint32_source> > factory(
			k3d::uuid(0x024b737a, 0xdf144dca, 0xb29e32b4, 0x319e5466),
			"Int32SourceScript",
			_("Uses a script to generate an output k3d::int32_t value."),
			"Scripting",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// int32_source_script_factory

k3d::iplugin_factory& int32_source_script_factory()
{
	return int32_source_script::get_factory();
}

} // namespace scripting

} // namespace module

