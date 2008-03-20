#ifndef K3DSDK_SCRIPTED_NODE_H
#define K3DSDK_SCRIPTED_NODE_H

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

#include "data.h"
#include "k3d-i18n-config.h"
#include "plugins.h"
#include "scripting.h"
#include "user_property_changed_signal.h"

#include <boost/scoped_ptr.hpp>

namespace k3d
{

/// Uses parameterized inheritance to provide a boilerplate implementation for nodes that are controlled by scripts
template<typename base_t>
class scripted_node :
	public base_t
{
public:
	scripted_node(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_script(init_owner(*this) + init_name("script") + init_label(_("Script")) + init_description(_("Script source code")) + init_value<string_t>("")),
		m_user_property_changed_signal(*this),
		m_executing(false)
	{
		m_script.changed_signal().connect(sigc::mem_fun(*this, &scripted_node::on_script_changed));
	}

protected:
	void set_script(const string_t& Script)
	{
		m_script.set_value(Script);
	}

	void connect_script_changed_signal(const sigc::slot<void, iunknown*>& Slot)
	{
		m_script.changed_signal().connect(Slot);
		m_user_property_changed_signal.connect(Slot);
	}

	bool execute_script(iscript_engine::context_t& Context)
	{
		if(m_executing) // prevent recursion when writing properties in the script (infinite loop!)
			return true;

		execute_lock lock(m_executing);

		const script::code code(m_script.pipeline_value());
		if(!m_script_engine)
		{
			const script::language language(code);
			return_val_if_fail(language.factory(), false);

			m_script_engine.reset(plugin::create<iscript_engine>(language.factory()->factory_id()));
			return_val_if_fail(m_script_engine, false);
		}

		return m_script_engine->execute(base_t::name() + "Script", code.source(), Context);
	}

private:
	/// RAII helper class that keeps track of whether we're executing, so we can avoid recursive loops.
	class execute_lock
	{
	public:
		execute_lock(bool_t& Executing) :
			executing(Executing)
		{
			executing = true;
		}

		~execute_lock()
		{
			executing = false;
		}

	private:
		bool_t& executing;
	};

	void on_script_changed(iunknown* hint)
	{
		m_script_engine.reset();
	}

	k3d_data(string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, script_property, with_serialization) m_script;
	boost::scoped_ptr<iscript_engine> m_script_engine;
	user_property_changed_signal m_user_property_changed_signal;
	bool m_executing;
};

} // namespace k3d

#endif // !K3DSDK_SCRIPTED_NODE_H

