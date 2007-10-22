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
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/scripted_node.h>
#include <k3dsdk/vector3_source.h>

#define DEFAULT_SCRIPT "#python\n\n\
import k3d\n\n\
Output = k3d.vector3()\n\n"

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// vector3_source_script

class vector3_source_script :
	public k3d::scripted_node<k3d::vector3_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::scripted_node<k3d::vector3_source<k3d::persistent<k3d::node> > > base;

public:
	vector3_source_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(DEFAULT_SCRIPT);

		connect_script_changed_signal(make_reset_vector3_slot());
	}

	k3d::vector3 on_create_vector3()
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		context["Output"] = k3d::vector3(0, 0, 0);

		execute_script(context);

		return_val_if_fail(context["Output"].type() == typeid(k3d::vector3), k3d::vector3(0, 0, 0));

		return boost::any_cast<k3d::vector3>(context["Output"]);
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<vector3_source_script, k3d::interface_list<k3d::ivector3_source> > factory(
			k3d::uuid(0xa4535c6e, 0xe19f4d7d, 0x8af30edb, 0x6e194f78),
			"Vector3SourceScript",
			_("Vector3 source that uses a script to create the output value"),
			"Scripting",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// vector3_source_script_factory

k3d::iplugin_factory& vector3_source_script_factory()
{
	return vector3_source_script::get_factory();
}

} // namespace scripting

} // namespace module


