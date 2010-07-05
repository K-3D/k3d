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

#include <k3dsdk/gl.h>
#include <k3dsdk/gl/context.h>
#include <k3dsdk/gl/extension.h>
#include <k3dsdk/result.h>

#include <boost/tokenizer.hpp>

#include <set>

namespace k3d
{

namespace gl
{

namespace extension
{

/// Returns the set of explicitly-disabled extensions
static std::set<string_t>& disabled()
{
	static std::set<string_t> results;
	return results;
}

/// Returns the set of explicitly-enabled extensions
static std::set<string_t>& enabled()
{
	static std::set<string_t> results;
	return results;
}

/// Returns the set of available extensions reported by OpenGL
static std::set<string_t>& extensions()
{
	static std::set<string_t> results;
	static bool initialized = false;

	if(!context::current())
	{
		log() << error << "Cannot retrieve OpenGL extensions without a current OpenGL context." << std::endl;
		return results;
	}

	if(!initialized)
	{
		initialized = true;

		const char* const extension_c_string = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
		if(!extension_c_string)
		{
			log() << warning << "GL extension query executed before context creation" << std::endl;
			return results;
		}
		const string_t extension_string = extension_c_string;

		boost::char_separator<char> separator(" ");
		boost::tokenizer<boost::char_separator<char> > tokenizer(extension_string, separator);
		std::copy(tokenizer.begin(), tokenizer.end(), std::inserter(results, results.end()));
	}

	return results;
}

const std::set<string_t> list()
{
	return extensions();
}

void disable(const string_t& Extension)
{
	enabled().erase(Extension);
	disabled().insert(Extension);
}

void enable(const string_t& Extension)
{
	disabled().erase(Extension);
	enabled().insert(Extension);
}

bool_t query(const string_t& Extension)
{
	if(disabled().count(Extension))
		return false;

	if(enabled().count(Extension))
		return true;

	return extensions().count(Extension) ? true : false;
}

bool_t query_vbo()
{
	return query("GL_ARB_vertex_buffer_object");
}

} // namespace extension

} // namespace gl

} // namespace k3d

