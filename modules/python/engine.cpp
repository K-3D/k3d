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
		\brief Implements Python engine, an implementation of k3d::iscript_engine that supports the Python language
		\author Anders Dahnielson (anders@dahnielson.com)
		\author Romain Behar (romainbehar@yahoo.com)
		\author Adam Hupp (hupp@cs.wisc.edu)
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/command_node.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/module.h>
#include <k3dsdk/python/object_model_python.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_modifiers.h>

#include <boost/assign/list_of.hpp>
#include <boost/python/dict.hpp>

namespace module
{

namespace python
{

/// Magic token used to identify Python scripts
const std::string magic_token("#python");

/// Ensures that Py_Initialize() is called before any other Python API functions
class initialize_python
{
public:
	initialize_python()
	{
		if(!Py_IsInitialized())
		{
			Py_Initialize();

			try
			{
				initk3d();
			}
			catch(std::exception& e)
			{
				k3d::log() << error << e.what() << std::endl;
			}
			catch(...)
			{
				k3d::log() << error << "Unknown exception" << std::endl;
			}
		}
	}
};

class engine :
	public k3d::iscript_engine
{
public:
	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<engine, k3d::interface_list<k3d::iscript_engine> > factory(
			k3d::classes::PythonEngine(),
			"Python",
			_("Python scripting engine"),
			"ScriptEngine",
			k3d::iplugin_factory::STABLE,
			boost::assign::map_list_of("k3d:mime-types", "text/x-python"));

		return factory;
	}

	const std::string language()
	{
		return "Python";
	}

	bool execute(const std::string& ScriptName, const std::string& Script, context_t& Context)
	{
		try
		{
			k3d::python::set_context(Context, m_local_dict);

			// The embedded python interpreter cannot handle DOS line-endings, see http://sourceforge.net/tracker/?group_id=5470&atid=105470&func=detail&aid=1167922
			std::string script = Script;
			script.erase(std::remove(script.begin(), script.end(), '\r'), script.end());

			PyDict_Update(m_local_dict.ptr(), PyObject_GetAttrString(PyImport_AddModule("__main__"), "__dict__"));
			
			PyObject* const result = PyRun_String(const_cast<char*>(script.c_str()), Py_file_input, m_local_dict.ptr(), m_local_dict.ptr());
			if(result)
			{
				Py_DECREF(result);
				if(Py_FlushLine())
					PyErr_Clear();
			}
			else
			{
				PyErr_Print();
			}

			k3d::python::get_context(m_local_dict, Context);
			
			return result ? true : false;
		}
		catch(std::exception& e)
		{
			k3d::log() << error << k3d_file_reference << ": " << e.what() << std::endl;
			return false;
		}
		catch(...)
		{
			k3d::log() << error << k3d_file_reference << ": " << "Unknown exception" << std::endl;
			return false;
		}
	}

	bool halt()
	{
		return false;
	}

	void bless_script(std::ostream& Script)
	{
		Script << magic_token << "\n\n";
		Script << "import k3d\n\n";
	}

	void append_comment(std::ostream& Script, const std::string& Comment)
	{
		// Ensure that the comment doesn't contain any newlines ...
		std::string comment(Comment);
		std::replace(comment.begin(), comment.end(), '\r', ' ');
		std::replace(comment.begin(), comment.end(), '\n', ' ');

		Script << "# " << comment << "\n";
	};

	void append_command(std::ostream& Script, k3d::icommand_node& CommandNode, const std::string& Command, const std::string& Arguments)
	{
		Script << "k3d.get_command_node(\"";
		Script << k3d::command_node::path(CommandNode);
		Script << "\").execute_command(\"";
		Script << Command;
		Script << "\", \"";
		Script << k3d::replace_all("\"", "\\\"", Arguments); // Make sure arguments are properly escaped
		Script << "\")\n";
	}

private:
	initialize_python m_initialize_python;
	boost::python::dict m_local_dict;
};

} // namespace python

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::python::engine::get_factory());
K3D_MODULE_END

