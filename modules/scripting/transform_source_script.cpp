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
#include <k3dsdk/hints.h>
#include <k3dsdk/node.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/scripted_node.h>
#include <k3dsdk/transform_source.h>
#include <k3dsdk/type_registry.h>

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// transform_source_script

class transform_source_script :
	public k3d::scripted_node<k3d::transform_source<k3d::node > >
{
	typedef k3d::scripted_node<k3d::transform_source<k3d::node > > base;

public:
	transform_source_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(k3d::resource::get_string("/module/scripting/transform_source_script.py"));

		connect_script_changed_signal(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_matrix_slot()));
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
			"Script Transform",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	void on_update_matrix(k3d::matrix4& Output)
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		context["Output"] = k3d::identity3();

		execute_script(context);

		if(context["Output"].type() == typeid(k3d::matrix4))
		{
			Output = boost::any_cast<k3d::matrix4>(context["Output"]);
			return;
		}

		k3d::log() << error << "unsupported output type: " << k3d::demangle(context["output"].type()) << std::endl;
		Output = k3d::identity3();
	}
};

/////////////////////////////////////////////////////////////////////////////
// transform_source_script_factory

k3d::iplugin_factory& transform_source_script_factory()
{
	return transform_source_script::get_factory();
}

} // namespace scripting

} // namespace module


