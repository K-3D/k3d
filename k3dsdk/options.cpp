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

#include "fstream.h"
#include "k3d-platform-config.h"
#include "log.h"
#include "options.h"
#include "options_policy.h"
#include "result.h"
#include "share.h"
#include "system.h"
#include "xml.h"

#include <boost/lexical_cast.hpp>

#include <iostream>

#ifdef K3D_API_WIN32

	#define DEFAULT_BITMAP_VIEWER "mspaint \"%1%\""
	#define DEFAULT_TEXT_EDITOR "notepad \"%1%\""

#else // K3D_API_WIN32

	#define DEFAULT_BITMAP_VIEWER "display \"%1%\""
	#define DEFAULT_TEXT_EDITOR "gedit \"%1%\""

#endif // !K3D_API_WIN32

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

xml::element& path_element(const string_t PathType)
{
	const xml::element match("path", xml::attribute("type", PathType));
	const xml::element prototype("path", xml::attribute("type", PathType));
	return paths_element().safe_element(match, prototype);
}

xml::element& commands_element()
{
	return tree().safe_element("commands");
}

xml::element& command_element(const string_t CommandType)
{
	const xml::element match("command", xml::attribute("type", CommandType));
	const xml::element prototype("command", xml::attribute("type", CommandType));
	return commands_element().safe_element(match, prototype);
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
	if(get_command(command::text_editor()).empty())
		set_command(command::text_editor(), DEFAULT_TEXT_EDITOR);
}

const filesystem::path get_path(const string_t& PathType)
{
	return filesystem::native_path(ustring::from_utf8(detail::path_element(PathType).text));
}

void set_path(const string_t& PathType, const filesystem::path& Path)
{
	detail::path_element(PathType).text = Path.native_utf8_string().raw();
}

const string_t get_command(const string_t& CommandType)
{
	return detail::command_element(CommandType).text;
}

void set_command(const string_t& CommandType, const string_t& Command)
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

