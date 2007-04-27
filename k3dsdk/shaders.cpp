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

#include "file_helpers.h"
#include "options.h"
#include "path.h"
#include "result.h"
#include "shader_cache.h"
#include "shaders.h"
#include "share.h"
#include "string_modifiers.h"
#include "system.h"

#include <boost/format.hpp>
#include <boost/static_assert.hpp>

#include <iostream>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// compile_shader

bool compile_shader(const filesystem::path& SourceFile, const std::string& RenderEngineType, const std::string& RenderEngineName)
{
	// Sanity checks ...
	return_val_if_fail(!RenderEngineType.empty(), false);
	return_val_if_fail(!RenderEngineName.empty(), false);

	if(!filesystem::exists(SourceFile))
	{
		k3d::log() << error << "shader source file: [" << SourceFile.native_console_string() << "] does not exist!" << std::endl;
		return false;
	}

	// Get the set of available render engines
	const options::render_engines_t engines = options::render_engines();

	try
	{
		// For each engine ...
		for(options::render_engines_t::const_iterator engine = engines.begin(); engine != engines.end(); ++engine)
		{
			// If it's the wrong type, skip it ...
			if(engine->type != RenderEngineType)
				continue;

			// If it's the wrong engine, skip it ...
			if(engine->name != RenderEngineName)
				continue;

			// OK, first let's make sure the shader binary cache directory exists ...
			filesystem::create_directories(shader_cache_path());
			return_val_if_fail(filesystem::exists(shader_cache_path()), false);

			// Figure out what the compiled shader filename will be ...
			boost::format shader_binary_file(engine->shader_binary);
			shader_binary_file % SourceFile.leaf().substr(0, SourceFile.leaf().rfind('.')).raw();

			// Figure out the full path to the source and compiled shaders ...
			const filesystem::path shader_source_path(SourceFile);
			const filesystem::path shader_binary_path = shader_cache_path() / filesystem::native_path(ustring::from_utf8(shader_binary_file.str()));

			// Get the last modification time of the shader source ...
			time_t source_modified = 0;
			return_val_if_fail(system::file_modification_time(shader_source_path, source_modified), false);

			// Get the last modification time of the shader binary (if it exists) ...
			time_t binary_modified = 0;
			system::file_modification_time(shader_binary_path, binary_modified);

			// If the binary is up-to-date, we're done ...
			if(source_modified <= binary_modified)
				return true;

			// Create a command line that can compile the shader ...
			boost::format command_line(engine->shader_compile_command);
			command_line.exceptions(boost::io::all_error_bits ^ boost::io::too_many_args_bit);

			command_line % shader_source_path.native_filesystem_string(); // %1% = shader source file
			command_line % shader_binary_path.native_filesystem_string(); // %2% = shader binary file
			command_line % shader_cache_path().native_filesystem_string(); // %3% = shader cache directory
			command_line % shader_source_path.branch_path().native_filesystem_string(); // %4% = shader source directory
			command_line % (k3d::share_path() / filesystem::generic_path("shaders")).native_filesystem_string(); // %5% = top-level shader share directory

			// Make it happen ...
			return_val_if_fail(system::spawn_sync(command_line.str()), false);

			return true;
		}
	}
	catch(std::exception& e)
	{
		log() << error << "Caught exception: " << e.what() << std::endl;
	}
	catch(...)
	{
		log() << error << "Caught unknown exception" << std::endl;
	}

	return false;
}

} // namespace k3d

