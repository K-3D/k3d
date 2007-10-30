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
std::set<k3d::string_t>& extensions()
{
	static std::set<k3d::string_t> results;
	static bool initialized = false;

	if(!initialized)
	{
		const char* const extension_c_string = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
		
		if (!extension_c_string)
		{
			k3d::log() << warning << "GL extension query executed before context creation" << std::endl;
			return results;
		}
		
		const k3d::string_t extension_string = extension_c_string;
		
		initialized = true;

		boost::char_separator<char> separator(" ");
		boost::tokenizer<boost::char_separator<char> > tokenizer(extension_string, separator);
		std::copy(tokenizer.begin(), tokenizer.end(), std::inserter(results, results.end()));
	}

	return results;
}

/// Returns the set of explicitly-disabled extensions
std::set<k3d::string_t>& disabled()
{
	static std::set<k3d::string_t> results;
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
std::set<k3d::string_t>& enabled()
{
	static std::set<k3d::string_t> results;
	return results;
}

} // namespace detail

const std::set<k3d::string_t> list()
{
	return detail::extensions();
}

void disable(const k3d::string_t& Extension)
{
	detail::disabled().insert(Extension);
	detail::enabled().erase(Extension);
}

void enable(const k3d::string_t& Extension)
{
	detail::disabled().erase(Extension);
	detail::enabled().insert(Extension);
}

const bool query(const k3d::string_t& Extension)
{ 
	bool result = (detail::enabled().count(Extension) || detail::extensions().count(Extension)) && !detail::disabled().count(Extension);
	// extra glew check for glew, to be sure the function pointer is initialised
	if (glewGetContext()) 
		return_val_if_fail(glewIsSupported(Extension.c_str()), false);
	return result;
}

bool query_vbo()
{
	if (glewGetContext())
		return GLEW_VERSION_1_5 || query("GL_ARB_vertex_buffer_object");
	return query("GL_ARB_vertex_buffer_object");
}

} // namespace extension

} // namespace gl

} // namespace k3d

