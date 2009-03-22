#ifndef MODULES_FREETYPE2_FREETYPE_H
#define MODULES_FREETYPE2_FREETYPE_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/path.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace module
{

namespace freetype2
{

/// Encapsulates the freetype FT_Library struct to provide RAII behavior
class library
{
public:
	library() :
		m_initialized(0 == FT_Init_FreeType(&m_library))
	{
	}

	~library()
	{
		if(m_initialized)
			FT_Done_FreeType(m_library);
	}

	operator bool()
	{
		return m_initialized;
	}

	operator FT_Library()
	{
		return m_library;
	}

private:
	const bool m_initialized;
	FT_Library m_library;
};

/// Encapsulates the freetype FT_Face struct to provide RAII behavior
class face
{
public:
	face(FT_Library Library, const k3d::filesystem::path& Path) :
		m_initialized(0 == FT_New_Face(Library, Path.native_filesystem_string().c_str(), 0, &m_face))
	{
	}

	~face()
	{
		if(m_initialized)
			FT_Done_Face(m_face);
	}

	k3d::bool_t is_scalable() const
	{
		return m_initialized && FT_IS_SCALABLE(m_face);
	}

	operator k3d::bool_t()
	{
		return m_initialized;
	}

	FT_Face& operator->()
	{
		return m_face;
	}

	operator FT_Face()
	{
		return m_face;
	}

private:
	const bool m_initialized;
	FT_Face m_face;
};

} // namespace freetype2

} // namespace module

#endif // !MODULES_FREETYPE2_FREETYPE_H

