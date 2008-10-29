#ifndef K3DSDK_FSTREAM_H
#define K3DSDK_FSTREAM_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

#include "path.h"
#include <fstream>

namespace k3d
{

namespace filesystem
{

/// Convenience wrapper for standard fstream that opens a k3d::filesystem::path in binary mode
template< class charT, class traits = std::char_traits<charT> >
class basic_ifstream : public std::basic_ifstream<charT,traits>
{
public:
	basic_ifstream()
	{
	}

	explicit basic_ifstream(const path& file, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::binary) :
		std::basic_ifstream<charT,traits>(file.native_filesystem_string().c_str(), mode)
	{
	}

	virtual ~basic_ifstream()
	{
	}

	void open(const path& file, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::binary)
	{
		std::basic_ifstream<charT,traits>::open(file.native_filesystem_string().c_str(), mode);
	}
};

/// Convenience wrapper for standard fstream that opens a k3d::filesystem::path in binary mode
typedef basic_ifstream<char> ifstream;
// typedef basic_ifstream<wchar_t> wifstream; // Why do you want to use this?  It isn't portable, and you should be using UTF-8!

/// Convenience wrapper for standard fstream that opens a k3d::filesystem::path in binary mode
template < class charT, class traits = std::char_traits<charT> >
class basic_ofstream : public std::basic_ofstream<charT,traits>
{
public:
	basic_ofstream()
	{
	}

	explicit basic_ofstream(const path& file, std::ios_base::openmode mode = std::ios_base::out | std::ios_base::binary) :
		std::basic_ofstream<charT,traits>(file.native_filesystem_string().c_str(), mode)
	{
	}

	virtual ~basic_ofstream()
	{
	}

	void open(const path& file, std::ios_base::openmode mode = std::ios_base::out | std::ios_base::binary)
	{
		std::basic_ofstream<charT,traits>::open(file.native_filesystem_string().c_str(), mode);
	}
};

/// Convenience wrapper for standard fstream that opens a k3d::filesystem::path in binary mode
typedef basic_ofstream<char> ofstream;
// typedef basic_ofstream<wchar_t> wofstream; // Why do you want to use this?  It isn't portable, and you should be using UTF-8!

/// Convenience wrapper for standard fstream that opens a k3d::filesystem::path in binary mode
template < class charT, class traits = std::char_traits<charT> >
class basic_fstream : public std::basic_fstream<charT,traits>
{
public:
	basic_fstream()
	{
	}

	explicit basic_fstream(const path& file, std::ios_base::openmode mode = std::ios_base::in|std::ios_base::out | std::ios_base::binary) :
		std::basic_fstream<charT,traits>(file.native_filesystem_string(), mode)
	{
	}

	virtual ~basic_fstream()
	{
	}

	void open(const path& file, std::ios_base::openmode mode = std::ios_base::in|std::ios_base::out | std::ios_base::binary)
	{
		std::basic_fstream<charT,traits>::open(file.native_filesystem_string().c_str(), mode);
	}
};

/// Convenience wrapper for standard fstream that opens a k3d::filesystem::path in binary mode
typedef basic_fstream<char> fstream;
// typedef basic_fstream<wchar_t> wfstream; // Why do you want to use this?  It isn't portable, and you should be using UTF-8!

} // namespace filesystem

} // namespace k3d

#endif // !K3DSDK_FSTREAM_H
