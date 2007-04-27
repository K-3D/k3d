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
#include <k3dsdk/scalar_source.h>
#include <k3dsdk/scripted_node.h>

#define DEFAULT_SCRIPT "#python\n\n\
Output = 0.0\n\n"

namespace libk3dscripting
{

/////////////////////////////////////////////////////////////////////////////
// scalar_source_script

class scalar_source_script :
	public k3d::scripted_node<k3d::scalar_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::scripted_node<k3d::scalar_source<k3d::persistent<k3d::node> > > base;

public:
	scalar_source_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(DEFAULT_SCRIPT);

		connect_script_changed_signal(make_reset_scalar_slot());
	}

	double on_create_scalar()
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = static_cast<k3d::iunknown*>(&document());
		context["Node"] = static_cast<k3d::iunknown*>(this);
		context["Output"] = 0.0;

		execute_script(context);

		return_val_if_fail(context["Output"].type() == typeid(double), 0.0);

		return boost::any_cast<double>(context["Output"]);
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<scalar_source_script, k3d::interface_list<k3d::iscalar_source> > factory(
			k3d::uuid(0xed110740, 0x7c2e4215, 0xbc8a4c1a, 0x3c1736ba),
			"ScalarSourceScript",
			_("Scalar source that uses a script to create the output value"),
			"Scripting Scalar",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// scalar_source_script_factory

k3d::iplugin_factory& scalar_source_script_factory()
{
	return scalar_source_script::get_factory();
}

} // namespace libk3dscripting

