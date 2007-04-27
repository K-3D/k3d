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
#include <k3dsdk/transform_source.h>
#include <k3dsdk/scripted_node.h>

#define DEFAULT_SCRIPT "#python\n\n\
import k3d\n\n\
Output = k3d.identity3()\n\n"

namespace libk3dscripting
{

/////////////////////////////////////////////////////////////////////////////
// transform_source_script

class transform_source_script :
	public k3d::scripted_node<k3d::transform_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::scripted_node<k3d::transform_source<k3d::persistent<k3d::node> > > base;

public:
	transform_source_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(DEFAULT_SCRIPT);

		connect_script_changed_signal(make_reset_matrix_slot());
	}

	k3d::matrix4 on_create_matrix()
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = static_cast<k3d::iunknown*>(&document());
		context["Node"] = static_cast<k3d::iunknown*>(this);
		context["Output"] = k3d::identity3D();

		execute_script(context);

		return_val_if_fail(context["Output"].type() == typeid(k3d::matrix4), k3d::identity3D());

		return boost::any_cast<k3d::matrix4>(context["Output"]);
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<transform_source_script, k3d::interface_list<k3d::itransform_source> > factory(
			k3d::uuid(0x59da7a41, 0xe08c4704, 0xbf75055f, 0x043628cc),
			"TransformSourceScript",
			_("Transform source that uses a script to create the output value"),
			"Scripting Transform",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// transform_source_script_factory

k3d::iplugin_factory& transform_source_script_factory()
{
	return transform_source_script::get_factory();
}

} // namespace libk3dscripting

