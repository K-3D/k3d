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
#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/itime_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/resource/resource.h>
#include <k3dsdk/scripted_node.h>
#include <k3dsdk/transformable.h>

namespace module
{

namespace scripting
{

/////////////////////////////////////////////////////////////////////////////
// transform_modifier_script

class transform_modifier_script :
	public k3d::scripted_node<k3d::transformable<k3d::persistent<k3d::node> > >
{
	typedef k3d::scripted_node<k3d::transformable<k3d::persistent<k3d::node> > > base;

public:
	transform_modifier_script(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		set_script(k3d::resource::get_string("/module/scripting/transform_modifier_script.py"));
		
		connect_script_changed_signal(sigc::mem_fun(*this, &transform_modifier_script::reset_output_cache));
	}

	void reset_output_cache(k3d::iunknown* const Hint)
	{
		m_output_matrix.reset(Hint);
	}

	k3d::matrix4 matrix()
	{		
		// Create a new output matrix, ready for modification by the script ...
		const k3d::matrix4 input = m_input_matrix.pipeline_value();
		k3d::matrix4 output = input;

		k3d::iscript_engine::context_t context;
		context["Document"] = &document();
		context["Node"] = static_cast<k3d::inode*>(this);
		context["Input"] = input;
		context["Output"] = output;

		execute_script(context);

		return_val_if_fail(context["Output"].type() == typeid(k3d::matrix4), output);
		output = boost::any_cast<k3d::matrix4>(context["Output"]);

		return output;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<transform_modifier_script,
			k3d::interface_list<k3d::itransform_sink, k3d::interface_list<k3d::itransform_source> > > factory(
			k3d::uuid(0xacafcc85, 0xa0bf4d69, 0x99592c4f, 0x7cf9b35c),
			"TransformModifierScript",
			_("Transform modifier that uses a script to modify a transformation matrix"),
			"Scripting Transform",
			k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// transform_modifier_script_factory

k3d::iplugin_factory& transform_modifier_script_factory()
{
	return transform_modifier_script::get_factory();
}

} // namespace scripting

} // namespace module


