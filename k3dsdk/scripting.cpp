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

#include "iapplication_plugin_factory.h"
#include "iplugin_factory.h"
#include "iscript_engine.h"
#include "mime_types.h"
#include "plugins.h"
#include "result.h"
#include "scripting.h"
#include "uuid.h"

#include <boost/scoped_ptr.hpp>

#include <sstream>
#include <typeinfo>

namespace k3d
{

namespace script
{

namespace detail
{

/// Executes a script using the given plugin factory to create the script engine
bool execute_script(const code& Script, const string_t& ScriptName, iscript_engine::context_t& Context, const language& Language)
{
	return_val_if_fail(ScriptName.size(), false);
	return_val_if_fail(Language.factory(), false);

	// Get the requested scripting engine ...
	boost::scoped_ptr<iscript_engine> engine(plugin::create<iscript_engine>(*Language.factory()));
	return_val_if_fail(engine, false);

	// Run that bad-boy ...
	const bool result = engine->execute(ScriptName, Script.source(), Context);

	return result;
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////////
// code

code::code(const string_t& Source) :
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

const string_t& code::source() const
{
	return m_buffer;
}

///////////////////////////////////////////////////////////////////////////////
// language

language::language(const code& Script) :
	m_factory(0)
{
	// Get the MIME type of the code ...
	const mime::type mime_type = mime::type::lookup(Script.source());
	if(mime_type.empty())
		return;

	// Get the set of script engine factories that handle this MIME type ...
	const plugin::factory::collection_t factories = plugin::factory::lookup<iscript_engine>(mime_type);
	if(factories.size() != 1)
		return;

	m_factory = *factories.begin();
}

language::language(const uuid& Language) :
	m_factory(plugin::factory::lookup(Language))
{
}

language::language(iplugin_factory& Language) :
	m_factory(&Language)
{
}

iplugin_factory* language::factory() const
{
	return m_factory;
}

///////////////////////////////////////////////////////////////////////////////
// execute

bool execute(const code& Script, const string_t& ScriptName, iscript_engine::context_t& Context, const language& Language)
{
	return detail::execute_script(Script, ScriptName, Context, Language);
}

///////////////////////////////////////////////////////////////////////////////
// execute

void execute(const code& Script, const string_t& ScriptName, iscript_engine::context_t& Context, bool& Recognized, bool& Executed)
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

