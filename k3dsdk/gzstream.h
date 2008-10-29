#ifndef K3DSDK_GZSTREAM_H
#define K3DSDK_GZSTREAM_H

// ============================================================================
// gzstream, C++ iostream classes wrapping the zlib compression library.
// Copyright (C) 2001  Deepak Bandyopadhyay, Lutz Kettner
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// ============================================================================
//
// File          : gzstream.h
// Revision      : $Revision: 1.3 $
// Revision_date : $Date: 2007/02/17 01:35:57 $
// Author(s)     : Deepak Bandyopadhyay, Lutz Kettner
//
// Standard streambuf implementation following Nicolai Josuttis, "The
// Standard C++ Library".
// ============================================================================

// Integrated into the K-3D SDK by Tim Shead, April 2006

#include <iostream>

namespace k3d
{

namespace filesystem
{

class gzstreambuf;
class path;

class gzstreambase :
	virtual public std::ios
{
protected:
	gzstreambuf* const buf;

public:
	gzstreambase();
	gzstreambase(const filesystem::path& name, int open_mode);
	~gzstreambase();

	void open(const filesystem::path& name, int open_mode);
	void close();
	std::streambuf* rdbuf();
};

/// ifstream replacement that can read files with gzip compression
/** \todo Implement this using boost::iostreams */
class igzstream :
	public gzstreambase,
	public std::istream
{
public:
	igzstream();
	igzstream(const filesystem::path& name);

	std::streambuf* rdbuf();
	void open(const filesystem::path& name);
};

/// ofstream replacement that writes files with gzip compression
/** \todo Implement this using boost::iostreams */
class ogzstream :
	public gzstreambase,
	public std::ostream
{
public:
	ogzstream();
	ogzstream(const filesystem::path& name);

	std::streambuf* rdbuf();
	void open(const filesystem::path& name);
};

} // namespace filesystem

} // namespace k3d

#endif // !K3DSDK_GZSTREAM_H

