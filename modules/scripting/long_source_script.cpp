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
		\brief Implements the RenderManScript K-3D object, which can insert scripted data into RenderMan output
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/long_source.h>
#include <k3dsdk/scripted_node.h>

#define DEFAULT_SCRIPT "#python\n\n\
Output = 0\n\n"

namespace libk3dscripting
{

/////////////////////////////////////////////////////////////////////////////
// long_source_script

class long_source_script :
	public k3d::scripted_node<k3d::long_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::scripted_node<k3d::long_source<k3d::persistent<k3d::node> > > base;

public:
	long_source_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(DEFAULT_SCRIPT);

		connect_script_changed_signal(make_reset_long_slot());
	}

	long on_create_long()
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = static_cast<k3d::iunknown*>(&document());
		context["Node"] = static_cast<k3d::iunknown*>(this);
		context["Output"] = 0;

		execute_script(context);

		if(context["Output"].type() == typeid(int))
			return boost::any_cast<int>(context["Output"]);

		if(context["Output"].type() == typeid(long))
			return boost::any_cast<long>(context["Output"]);

		return 0;
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<long_source_script, k3d::interface_list<k3d::ilong_source> > factory(
			k3d::uuid(0x024b737a, 0xdf144dca, 0xb29e32b4, 0x319e5466),
			"LongSourceScript",
			_("Long source that uses a script to create the output value"),
			"Scripting",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// long_source_script_factory

k3d::iplugin_factory& long_source_script_factory()
{
	return long_source_script::get_factory();
}

} // namespace libk3dscripting

