#ifndef K3DSDK_ISCRIPT_ENGINE_H
#define K3DSDK_ISCRIPT_ENGINE_H

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
		\brief Declares iscript_engine, an abstract interface implemented by objects that can execute scripts written in a specific scripting language
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"

#include <boost/any.hpp>
#include <iosfwd>
#include <map>
#include <string>

namespace k3d
{

class icommand_node;
class iplugin_factory;

/// Abstract interface implemented by objects that can execute scripts written in a specific scripting language
/** \note: The Script arguments to can_execute() and execute() have bounced back-and-forth between
 * string and stream representations several times.  The original rationale for making them streams was
 * to avoid having to buffer the source code to a script in memory; the new rationale for making them strings
 * is that in practice you always end-up having to buffer the source code anyway, for one of two reasons:
 * first, only the k3dscript engine can use an input stream efficiently, the Python engine doesn't understand
 * C++ streams, so we had to buffer the source before passing it to the interpreter anyway; second, calling
 * can_execute() and execute() in sequence (the most common use-case) requires random access to the source stream,
 * which isn't available for compressed streams or socket-based streams.
*/

class iscript_engine :
	public virtual iunknown
{
public:
	/// Returns a reference to the factory that created this object
	virtual iplugin_factory& factory() = 0;
	/**	\brief Returns the human-readable name of the scripting language this engine implements
	*/
	virtual const std::string language() = 0;

	/**	\brief Tests to see if the supplied script is written in the scripting language implemented by this engine
		\param Script The complete source code of the script to be tested
		\return true, iff the supplied script can be executed by the engine
	*/
	virtual bool can_execute(const std::string& Script) = 0;

	/// Defines a collection of named objects to pass to a script that define its context (its execution environment) - how they are used is implementation-dependent (note that the names are merely suggestions, and may be changed or ignored at the whim of the implementation)
	typedef std::map<std::string, boost::any> context_t;

	/**	\brief Executes a script
		\param ScriptName A human readable identifier for the script, which should be used in error messages, etc.
		\param Script The complete source code of the script to be executed
		\param Context Optional collection of objects that define the context/execution environment of the script -  how they are used is implementation-dependent.  Note that the script engine may alter context objects before returning.
		\return true, iff the script was successfully executed without errors (either syntax or runtime)
	*/
	virtual bool execute(const std::string& ScriptName, const std::string& Script, context_t& Context) = 0;

	/**	\brief Requests a cancellation of all running scripts.
		\return true, iff script cancellation is supported by this engine.
		\note Cancellation may be asynchronous, i.e. scripts may still be running when the call returns, and may continue to run for an indefinite period before shutting down, if at all.
	*/
	virtual bool halt() = 0;

	/**	\brief Writes a token to a script stream so that it can be recognized by this engine
	*/
	virtual void bless_script(std::ostream& Script) = 0;

	/**	\brief Appends the given text to a script as a comment that will be ignored by this engine
		\param Comment A string to be appended to the script as a comment
		\note The comment may be single- or multi-line, and can contain any text.  The engine is responsible for ensuring that the comment text does not introduce syntactically-incorrect code.
	*/
	virtual void append_comment(std::ostream& Script, const std::string& Comment) = 0;

	/** Converts a command-node command into source code appropriate to this language and adds it to the given script
		\param CommandNode The command node executing the command to be appended
		\param Command The name of the command to be appended
		\param Arguments The command arguments to be appended
		\note The engine is responsible for ensuring that the appended command does not introduce syntactically-incorrect code, e.g. quoting issues or escaped characters with special meanings.
	*/
	virtual void append_command(std::ostream& Script, icommand_node& CommandNode, const std::string& Command, const std::string& Arguments) = 0;

protected:
	iscript_engine() {}
	iscript_engine(const iscript_engine& RHS) {}
	iscript_engine& operator = (const iscript_engine& RHS) { return *this; }
	virtual ~iscript_engine() {}
};

} // namespace k3d

#endif // K3DSDK_ISCRIPT_ENGINE_H

