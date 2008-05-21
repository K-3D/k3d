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
#include <k3dsdk/node.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/scripted_node.h>

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// null_output_script

class null_output_script :
	public k3d::scripted_node<k3d::node >
{
	typedef k3d::scripted_node<k3d::node > base;

public:
	null_output_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(k3d::resource::get_string("/module/scripting/null_output_script.py"));

		connect_script_changed_signal(sigc::mem_fun(*this, &null_output_script::on_execute));
	}

	void on_execute(k3d::iunknown*)
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);

		execute_script(context);
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<null_output_script > factory(
			k3d::uuid(0x16b6b2f1, 0x383d4d2c, 0x9bea5eb3, 0x94a8783f),
			"NullOutputScript",
			_("Script node that executes whenever one of its inputs changes"),
			"Scripting",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// null_output_script_factory

k3d::iplugin_factory& null_output_script_factory()
{
	return null_output_script::get_factory();
}

} // namespace scripting

} // namespace module


