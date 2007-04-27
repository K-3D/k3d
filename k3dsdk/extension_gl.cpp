// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "extension_gl.h"
#include "gl.h"
#include "result.h"

#include <boost/tokenizer.hpp>

#include <set>

namespace k3d
{

namespace gl
{

namespace extension
{

namespace detail
{

/// Returns the set of available extensions reported by OpenGL
std::set<std::string>& extensions()
{
	static std::set<std::string> results;
	static bool initialized = false;

	if(!initialized)
	{
		initialized = true;

		const char* const extension_c_string = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
		const std::string extension_string = extension_c_string ? extension_c_string : "";

		boost::char_separator<char> separator(" ");
		boost::tokenizer<boost::char_separator<char> > tokenizer(extension_string, separator);
		std::copy(tokenizer.begin(), tokenizer.end(), std::inserter(results, results.end()));
	}

	return results;
}

/// Returns the set of explicitly-disabled extensions
std::set<std::string>& disabled()
{
	static std::set<std::string> results;
	static bool initialized = false;

	if(!initialized)
	{
		initialized = true;
		// add default list of disabled extensions here
		//results.insert("GL_ARB_vertex_buffer_object");
	}

	return results;
}

/// Returns the set of explicitly-enabled extensions
std::set<std::string>& enabled()
{
	static std::set<std::string> results;
	return results;
}

} // namespace detail

void disable(const std::string& Extension)
{
	detail::disabled().insert(Extension);
	detail::enabled().erase(Extension);
}

void enable(const std::string& Extension)
{
	detail::disabled().erase(Extension);
	detail::enabled().insert(Extension);
}

const bool query(const std::string& Extension)
{ 
	bool result = (detail::enabled().count(Extension) || detail::extensions().count(Extension)) && !detail::disabled().count(Extension);
	#ifdef K3D_PLATFORM_WIN32
		// extra glew check on win32, to be sure the function pointer is initialised
		if (glewGetContext()) 
			return_val_if_fail(glewIsSupported(Extension.c_str()), false);
	#endif
	return result;
}

bool query_vbo()
{
	#ifdef K3D_PLATFORM_WIN32
	if (glewGetContext())
		return GLEW_VERSION_1_5 || query("GL_ARB_vertex_buffer_object");
	#endif
	return query("GL_ARB_vertex_buffer_object");
}

} // namespace extension

} // namespace gl

} // namespace k3d

