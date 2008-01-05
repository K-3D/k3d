#ifndef K3DSDK_SCRIPTED_PLUGIN_H
#define K3DSDK_SCRIPTED_PLUGIN_H

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
#include "plugin.h"
#include "scripting.h"

namespace k3d
{

/// Convenience class for application plugins that need to call scripts
template<typename base_t>
class scripted_plugin :
	public base_t
{
public:
	scripted_plugin() :
		m_script(init_owner(*this) + init_name("script") + init_label(_("Script")) + init_description(_("Script source code")) + init_value<std::string>("")),
		m_script_engine(0)
	{
	}

	~scripted_plugin()
	{
		delete dynamic_cast<ideletable*>(m_script_engine);
	}

protected:
	void set_script(const std::string& Script)
	{
		m_script.set_value(Script);
	}

	bool execute_script(iscript_engine::context_t& Context)
	{
		// Examine the script to determine the correct language ...
		const script::code code(m_script.pipeline_value());
		const script::language language(code);

		return_val_if_fail(language.factory(), false);

		// If the current script engine is for the wrong language, lose it ...
		if(m_script_engine && (m_script_engine->factory().factory_id() != language.factory()->factory_id()))
		{
			delete dynamic_cast<ideletable*>(m_script_engine);
			m_script_engine = 0;
		}

		// Create our script engine as-needed ...
		if(!m_script_engine)
			m_script_engine = plugin::create<iscript_engine>(language.factory()->factory_id());

		// No script engine?  We're outta here ...
		return_val_if_fail(m_script_engine, false);

		// Execute that bad-boy!
		return m_script_engine->execute("Script", code.source(), Context);
	}

private:
	k3d_data(std::string, immutable_name, change_signal, no_undo, local_storage, no_constraint, script_property, no_serialization) m_script;
	iscript_engine* m_script_engine;
};

} // namespace k3d

#endif // !K3DSDK_SCRIPTED_PLUGIN_H

