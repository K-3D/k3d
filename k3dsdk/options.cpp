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
		\brief Declares k3d::ioptions, an abstract interface for global user options
		\author Tim Shead (tshead@k-3d.com)
*/

#include "fstream.h"
#include "log.h"
#include "options.h"
#include "options_policy.h"
#include "result.h"
#include "share.h"
#include "system.h"
#include "xml.h"

#include <boost/lexical_cast.hpp>

#include <iostream>

#ifdef K3D_PLATFORM_WIN32

	#define DEFAULT_BITMAP_VIEWER "mspaint \"%1%\""
	#define DEFAULT_HTML_VIEWER "firefox \"%1%\""
	#define DEFAULT_TEXT_EDITOR "notepad \"%1%\""

#else // K3D_PLATFORM_WIN32

	#define DEFAULT_BITMAP_VIEWER "display \"%1%\""
	#define DEFAULT_HTML_VIEWER "firefox \"%1%\""
	#define DEFAULT_TEXT_EDITOR "gedit \"%1%\""

#endif // !K3D_PLATFORM_WIN32

namespace k3d
{

namespace options
{

namespace detail
{

xml::element& version_element()
{
	return tree().safe_element("version");
}

xml::element& paths_element()
{
	return tree().safe_element("paths");
}

xml::element& path_element(const std::string PathType)
{
	const xml::element match("path", xml::attribute("type", PathType));
	const xml::element prototype("path", xml::attribute("type", PathType));
	return paths_element().safe_element(match, prototype);
}

xml::element& commands_element()
{
	return tree().safe_element("commands");
}

xml::element& command_element(const std::string CommandType)
{
	const xml::element match("command", xml::attribute("type", CommandType));
	const xml::element prototype("command", xml::attribute("type", CommandType));
	return commands_element().safe_element(match, prototype);
}

xml::element& default_render_engines_element()
{
	return tree().safe_element("default_render_engines");
}

xml::element& default_render_engine_element(const std::string& Type)
{
	const xml::element match("default_render_engine", xml::attribute("type", Type));
	const xml::element prototype("default_render_engine", xml::attribute("type", Type));
	return default_render_engines_element().safe_element(match, prototype);
}

xml::element& render_engines_element()
{
	return tree().safe_element("render_engines");
}

xml::element& render_engine_element(const std::string& Type, const std::string& Name)
{
	const xml::element match("render_engine", xml::attribute("type", Type), xml::attribute("name", Name));
	const xml::element prototype("render_engine", xml::attribute("type", Type), xml::attribute("name", Name));
	return render_engines_element().safe_element(match, prototype);
}

xml::element& render_engine_commands_element(const std::string& Type, const std::string& Name)
{
	return render_engine_element(Type, Name).safe_element("commands");
}

xml::element& render_engine_command_element(const std::string& Type, const std::string& Name, const std::string& CommandType)
{
	const xml::element match("command", xml::attribute("type", CommandType));
	const xml::element prototype("command", xml::attribute("type", CommandType));
	return render_engine_commands_element(Type, Name).safe_element(match, prototype);
}

void create_render_engine(
	const std::string& Type,
	const std::string& Name,
	const std::string& Label,
	const std::string& RenderCommand,
	const std::string& ShaderBinary = "",
	const std::string& ShaderCompileCommand = "")
{
	return_if_fail(!Type.empty());
	return_if_fail(!Name.empty());
	return_if_fail(!Label.empty());
	return_if_fail(!RenderCommand.empty());

	xml::element& engine = render_engine_element(Type, Name);
	if(xml::attribute_text(engine, "label").empty())
		xml::set_attribute(engine, xml::attribute("label", Label));

	xml::element& render_command = render_engine_command_element(Type, Name, "render");
	if(render_command.text.empty())
		render_command.text = RenderCommand;

	xml::element& shader_compile_command = render_engine_command_element(Type, Name, "compile_shader");
	if(!ShaderBinary.empty() && !ShaderCompileCommand.empty() && shader_compile_command.text.empty())
	{
		xml::set_attribute(shader_compile_command, xml::attribute("binary", ShaderBinary));
		shader_compile_command.text = ShaderCompileCommand;
	}
}

} // namespace detail

//////////////////////////////////////////////////////////////////////////////////////////
// file_storage::implementation

class file_storage::implementation
{
public:
	implementation(const filesystem::path& File) :
		m_file(File)
	{
		try
		{
			log() << info << "Loading options from " << m_file.native_console_string() << std::endl;
			filesystem::ifstream stream(m_file);
			stream >> m_tree;
		}
		catch(std::exception& e)
		{
			log() << error << "Exception loading options: " << e.what() << std::endl;
		}
		catch(...)
		{
			log() << error << "Unknown exception loading options" << std::endl;
		}
	}

	~implementation()
	{
		commit();
	}

	xml::element& tree()
	{
		return m_tree;
	}

	void commit()
	{
		try
		{
			log() << info << "Saving options to " << m_file.native_console_string() << std::endl;
			filesystem::ofstream stream(m_file);
			stream << xml::declaration() << m_tree;
		}
		catch(std::exception& e)
		{
			log() << error << "Exception saving options: " << e.what() << std::endl;
		}
		catch(...)
		{
			log() << error << "Unknown exception saving options" << std::endl;
		}
	}

private:
	filesystem::path m_file;
	xml::element m_tree;
};

//////////////////////////////////////////////////////////////////////////////////////////
// file_storage

file_storage::file_storage(const filesystem::path& File) :
	m_implementation(new implementation(File))
{
}

file_storage::~file_storage()
{
	delete m_implementation;
}

xml::element& file_storage::tree()
{
	return m_implementation->tree();
}

void file_storage::commit()
{
	m_implementation->commit();
}

//////////////////////////////////////////////////////////////////////////////////////////
// set_storage

/// Storage for global user options
istorage* g_option_storage = 0;

void set_storage(istorage& Storage)
{
	g_option_storage = &Storage;

	// Defines the version of the data stored in the options tree ... incrementing it will force user options to be reset
	const unsigned long current_version = 4;
	if(detail::version_element().text.empty() || (boost::lexical_cast<unsigned long>(detail::version_element().text) < current_version))
	{
		log() << info << "Resetting options data to version " << current_version << std::endl;

		tree().children.clear();
		detail::version_element().text = k3d::string_cast(current_version);
	}

	// Default external paths
	if(detail::path_element(path::render_farm()).text.empty())
		detail::path_element(path::render_farm()).text = system::get_temp_directory().native_utf8_string().raw();
	if(detail::path_element(path::scripts()).text.empty())
		detail::path_element(path::scripts()).text = (share_path() / filesystem::generic_path("scripts")).native_utf8_string().raw();
	if(detail::path_element(path::documents()).text.empty())
		detail::path_element(path::documents()).text = system::get_home_directory().native_utf8_string().raw();
	if(detail::path_element(path::bitmaps()).text.empty())
		detail::path_element(path::bitmaps()).text = system::get_home_directory().native_utf8_string().raw();
	if(detail::path_element(path::tutorials()).text.empty())
		detail::path_element(path::tutorials()).text = system::get_home_directory().native_utf8_string().raw();
	if(detail::path_element(path::render_frame()).text.empty())
		detail::path_element(path::render_frame()).text = system::get_home_directory().native_utf8_string().raw();
	if(detail::path_element(path::render_animation()).text.empty())
		detail::path_element(path::render_animation()).text = system::get_home_directory().native_utf8_string().raw();
	if(detail::path_element(path::displacement_shaders()).text.empty())
		detail::path_element(path::displacement_shaders()).text = (share_path() / filesystem::generic_path("shaders/displacement")).native_utf8_string().raw();
	if(detail::path_element(path::imager_shaders()).text.empty())
		detail::path_element(path::imager_shaders()).text = (share_path() / filesystem::generic_path("shaders/imager")).native_utf8_string().raw();
	if(detail::path_element(path::light_shaders()).text.empty())
		detail::path_element(path::light_shaders()).text = (share_path() / filesystem::generic_path("shaders/light")).native_utf8_string().raw();
	if(detail::path_element(path::surface_shaders()).text.empty())
		detail::path_element(path::surface_shaders()).text = (share_path() / filesystem::generic_path("shaders/surface")).native_utf8_string().raw();
	if(detail::path_element(path::transformation_shaders()).text.empty())
		detail::path_element(path::transformation_shaders()).text = (share_path() / filesystem::generic_path("shaders/transformation")).native_utf8_string().raw();
	if(detail::path_element(path::volume_shaders()).text.empty())
		detail::path_element(path::volume_shaders()).text = (share_path() / filesystem::generic_path("shaders/volume")).native_utf8_string().raw();

	// Default external commands
	if(get_command(command::bitmap_viewer()).empty())
		set_command(command::bitmap_viewer(), DEFAULT_BITMAP_VIEWER);
	if(get_command(command::html_viewer()).empty())
		set_command(command::html_viewer(), DEFAULT_HTML_VIEWER);
	if(get_command(command::text_editor()).empty())
		set_command(command::text_editor(), DEFAULT_TEXT_EDITOR);

	// Default RenderMan engine types ...
	detail::create_render_engine("ri", "aqsis", "Aqsis", "aqsis -shaders=\"%2%\" \"%1%\"", "%1%.slx", "aqsl -I\"%4%\" -I\"%5%\" -o \"%2%\" \"%1%\"");
	detail::create_render_engine("ri", "pixie", "Pixie", "rndr \"%1%\"", "%1%.sdr", "sdrc -I\"%4%\" -I\"%5%\" -o \"%2%\" \"%1%\"");
	detail::create_render_engine("ri", "bmrt", "BMRT", "rendrib \"%1%\"", "%1%.slc", "slc \"%1%\" -o \"%2%\"");
	detail::create_render_engine("ri", "prman", "PRman", "prman -progress \"%1%\"", "%1%.slo", "slcomp \"%1%\"");
	detail::create_render_engine("ri", "netprman", "NETPRman", "netrender -progress -f \"%1%\"", "%1%.slo", "slcomp \"%1%\"");
	detail::create_render_engine("ri", "3delight", "3Delight", "renderdl \"%1%\"", "%1%.sdl", "shaderdl -d \"%3%\" --dont-keep-cpp-file --dont-keep-c++-file \"%1%\"");
	detail::create_render_engine("ri", "rdc", "Render Dot C", "renderdc \"%1%\"", "%1%.so", "shaderdc \"%1%\"");
	detail::create_render_engine("ri", "air", "AIR", "air \"%1%\"", "%1%.slb", "shaded \"%1%\"");
	detail::create_render_engine("ri", "povman", "POVMan", "povman \"%1%\"", "%1%.slp", "povslc -o \"%2%\" \"%1%\"");
	detail::create_render_engine("ri", "editrib", "Edit RIB File", "gvim \"%1%\"");
	detail::create_render_engine("ri", "saverib", "Save RIB File", "file=`Xdialog --stdout --title \"Save RIB File As:\" --fselect ~/world.rib 0 0`; if [ -n \"$file\" ]; then cp \"%1%\" $file; fi");

	// Default YAFRAY engine type ...
	detail::create_render_engine("yafray", "yafray", "YafRay", "yafray \"%1%\"");

	// Default GraphViz engine types ...
	detail::create_render_engine("graphviz", "dot", "Dot", "dot -Tps \"%1%\" -o world.ps");
	detail::create_render_engine("graphviz", "neato", "Neato", "neato -Tps \"%1%\" -o world.ps");
	detail::create_render_engine("graphviz", "twopi", "TwoPI", "twopi -Tps \"%1%\" -o world.ps");

	// Default POV engine types ...
	detail::create_render_engine("pov", "povray", "POV-Ray", "povray \"%1%\"");

	if(default_render_engine("ri").empty())
		set_default_render_engine("ri", "aqsis");
	if(default_render_engine("yafray").empty())
		set_default_render_engine("yafray", "yafray");
	if(default_render_engine("graphviz").empty())
		set_default_render_engine("graphviz", "dot");
	if(default_render_engine("pov").empty())
		set_default_render_engine("pov", "povray");
}

const render_engines_t render_engines()
{
	render_engines_t results;

	xml::element& engines = detail::render_engines_element();
	for(xml::element::elements_t::iterator engine = engines.children.begin(); engine != engines.children.end(); ++engine)
	{
		if(engine->name != "render_engine")
			continue;

		const std::string type = xml::attribute_text(*engine, "type");
		if(type.empty())
		{
			log() << warning << "empty render engine type!" << std::endl;
			continue;
		}

		const std::string name = xml::attribute_text(*engine, "name");
		if(name.empty())
		{
			log() << warning << "Empty render engine name!" << std::endl;
			continue;
		}

		const std::string label = attribute_text(*engine, "label");
		if(label.empty())
		{
			log() << warning << "Empty render engine label!" << std::endl;
			continue;
		}

		std::string render_command;
		std::string shader_binary;
		std::string shader_compile_command;

		xml::element& commands = engine->safe_element("commands");
		for(xml::element::elements_t::iterator command = commands.children.begin(); command != commands.children.end(); ++command)
		{
			if(command->name != "command")
				continue;

			const std::string command_type = xml::attribute_text(*command, "type");
			if(command_type == "render")
			{
				render_command = command->text;
			}
			else if(command_type == "compile_shader")
			{
				shader_binary = xml::attribute_text(*command, "binary");
				shader_compile_command = command->text;
			}
		}

		results.push_back(render_engine(type, name, label, render_command, shader_binary, shader_compile_command));
	}

	return results;
}

const std::string default_render_engine(const std::string& Type)
{
	return detail::default_render_engine_element(Type).text;
}

void set_default_render_engine(const std::string& Type, const std::string& Name)
{
	return_if_fail(!Name.empty());
	detail::default_render_engine_element(Type).text = Name;
}

const filesystem::path get_path(const std::string& PathType)
{
	return filesystem::native_path(ustring::from_utf8(detail::path_element(PathType).text));
}

void set_path(const std::string& PathType, const filesystem::path& Path)
{
	detail::path_element(PathType).text = Path.native_utf8_string().raw();
}

const std::string get_command(const std::string& CommandType)
{
	return detail::command_element(CommandType).text;
}

void set_command(const std::string& CommandType, const std::string& Command)
{
	detail::command_element(CommandType).text = Command;
}

xml::element& tree()
{
	// If this fires, you forgot to call set_storage() at program startup!!!
	assert(g_option_storage);
	return g_option_storage->tree();
}

void commit()
{
	// If this fires, you forgot to call set_storage() at program startup!!!
	assert(g_option_storage);
	g_option_storage->commit();
}

} // namespace options

} // namespace k3d

