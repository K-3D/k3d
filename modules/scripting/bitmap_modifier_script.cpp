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
#include <k3dsdk/bitmap_modifier.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/scripted_node.h>

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// bitmap_modifier_script

class bitmap_modifier_script :
	public k3d::scripted_node<k3d::node >,
	public k3d::bitmap_modifier<bitmap_modifier_script>
{
	typedef k3d::scripted_node<k3d::node > base;

public:
	bitmap_modifier_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(k3d::resource::get_string("/module/scripting/bitmap_modifier_script.py"));

		connect_script_changed_signal(make_reset_bitmap_slot());
	}

	void on_create_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		context["Input"] = &Input;
		context["Output"] = &Output;

		execute_script(context);
	}

	void on_update_bitmap(const k3d::bitmap& Input, k3d::bitmap& Output)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bitmap_modifier_script,
			k3d::interface_list<k3d::ibitmap_sink, k3d::interface_list<k3d::ibitmap_source> > > factory(
			k3d::uuid(0xe6ca5ca4, 0xcfa8452f, 0x8318b2aa, 0xe253956e),
			"BitmapModifierScript",
			_("Bitmap modifier that uses a script to modify an input bitmap"),
			"Script Bitmap",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bitmap_modifier_script_factory

k3d::iplugin_factory& bitmap_modifier_script_factory()
{
	return bitmap_modifier_script::get_factory();
}

} // namespace scripting

} // namespace module



