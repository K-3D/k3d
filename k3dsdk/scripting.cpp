// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Implements convenience methods for working with scripts
		\author Tim Shead (tshead@k-3d.com)
*/

#include "create_plugins.h"
#include "iapplication_plugin_factory.h"
#include "ideletable.h"
#include "iplugin_factory.h"
#include "iscript_engine.h"
#include "plugins.h"
#include "result.h"
#include "scripting.h"
#include "uuid.h"

#include <sstream>
#include <typeinfo>

namespace k3d
{

namespace script
{

namespace detail
{

/// Returns the set of all available scripting languages (cached)
const factories_t& script_engine_factories()
{
	static factories_t factories;
	if(factories.empty())
		factories = plugins<iscript_engine>();

	return factories;
}

/// Executes a script using the given plugin factory to create the script engine
bool execute_script(const code& Script, const std::string& ScriptName, iscript_engine::context_t& Context, const language& Language)
{
	return_val_if_fail(ScriptName.size(), false);
	return_val_if_fail(Language.factory(), false);

	// Get the requested scripting engine ...
	iscript_engine* const engine = dynamic_cast<iscript_engine*>(create_plugin(*Language.factory()));
	return_val_if_fail(engine, false);

	// Run that bad-boy ...
	const bool result = engine->execute(ScriptName, Script.source(), Context);

	// Done with the engine ...
	delete dynamic_cast<ideletable*>(engine);

	return result;
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////////
// code

code::code(const std::string& Source) :
	m_buffer(Source)
{
}

code::code(std::istream& Source)
{
	std::stringstream temp;
	temp << Source.rdbuf();

	m_buffer = temp.str();
}

code::code(std::istringstream& Source) :
	m_buffer(Source.str())
{
}

code::code(std::stringstream& Source) :
	m_buffer(Source.str())
{
}

const std::string& code::source() const
{
	return m_buffer;
}

///////////////////////////////////////////////////////////////////////////////
// language

language::language(const code& Script) :
	m_factory(0)
{
	// Get the set of all script engine factories ...
	const factories_t& factories(detail::script_engine_factories());

	// Look for a scripting engine that can execute the script ...
	for(factories_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
	{
		iscript_engine* const script_engine = dynamic_cast<iscript_engine*>(create_plugin(**factory));
		if(!script_engine)
			continue;

		const bool can_execute = script_engine->can_execute(Script.source());
		delete dynamic_cast<ideletable*>(script_engine);
	
		if(can_execute)
		{
			m_factory = *factory;
			break;
		}
	}
}

language::language(const uuid& Language) :
	m_factory(0)
{
	// Get the set of all script engine factories ...
	const factories_t& factories(detail::script_engine_factories());

	// Look for a factory that matches ...
	for(factories_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
	{
		if((*factory)->factory_id() != Language)
			continue;

		m_factory = *factory;
		break;
	}
}

language::language(iplugin_factory* Language) :
	m_factory(Language)
{
}

iplugin_factory* language::factory() const
{
	return m_factory;
}

bool execute(const code& Script, const std::string& ScriptName, iscript_engine::context_t& Context, const language& Language)
{
	return detail::execute_script(Script, ScriptName, Context, Language);
}

void execute(const code& Script, const std::string& ScriptName, iscript_engine::context_t& Context, bool& Recognized, bool& Executed)
{
	// Starting state ...
	Recognized = false;
	Executed = false;

	// See which language it's written in ...
	language detected_language(Script);

	// If the language wasn't recognized, we're done ...
	Recognized = detected_language.factory() ? true : false;
	if(!Recognized)
		return;

	// Execute that puppy ...
	Executed = detail::execute_script(Script, ScriptName, Context, detected_language);
}

} // namespace script

} // namespace k3d

