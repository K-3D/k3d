// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/imeta_object.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/scripted_plugin.h>

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// meta_object_script

class meta_object_script :
	public k3d::scripted_plugin<k3d::property_collection>,
	public k3d::imeta_object
{
public:
	meta_object_script()
	{
		set_script(k3d::resource::get_string("/module/scripting/meta_object_script.py"));
	}

	virtual const boost::any execute(const k3d::string_t& Command, const std::vector<boost::any>& Arguments)
	{
		k3d::iscript_engine::context context;
		context["command"] = Command;
//		context["arguments"] = Arguments;
		execute_script(context);

		return boost::any();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<meta_object_script > factory(
			k3d::uuid(0xb754daf5, 0x3a401b87, 0xdd144ba2, 0x00395dde),
			"MetaObjectScript",
			_("Scripted node that acts as a meta-object (provides dynamic runtime execution of arbitrary commands)."),
			"Script",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// meta_object_script_factory

k3d::iplugin_factory& meta_object_script_factory()
{
	return meta_object_script::get_factory();
}

} // namespace scripting

} // namespace module


