#ifndef MODULES_STL_IO_BINARY_STL_H
#define MODULES_STL_IO_BINARY_STL_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-version-config.h>
#include <k3dsdk/types.h>

#include <fstream>

namespace module
{

namespace stl
{

namespace io
{

/// Binary encapsulation of a single facet
struct facet
{
	k3d::float_t normal[3];
	k3d::float_t v0[3];
	k3d::float_t v1[3];
	k3d::float_t v2[3];
	k3d::uint16_t color;
};

/// Encapsulates the STL binary data format
struct binary_stl
{
	binary_stl()
	{
		k3d::string_t headstr =  k3d::string_t("K-3D ") + k3d::string_t(K3D_VERSION) + k3d::string_t(" STL writer");
		for(k3d::uint_t i = 0 ; i != headstr.size(); ++i)
			header[i] = headstr[i];
	}
	/// Header containing file comment
	char header[80];
	std::vector<facet> facets;

	void write(std::ostream& Output)
	{
		Output.write(header, 80);
		const k3d::int32_t nfacets = facets.size();
		Output.write(reinterpret_cast<const char*>(&nfacets), sizeof(k3d::int32_t));
		for(k3d::uint_t triangle = 0; triangle != nfacets; ++triangle)
		{
			Output.write(reinterpret_cast<char*>(&facets[triangle]), 50);
		}
	}

	void read(std::istream& Input)
	{
		Input.read(header, 80);
		k3d::int32_t nfacets = facets.size();
		Input.read(reinterpret_cast<char*>(&nfacets), sizeof(k3d::int32_t));
		facets.resize(nfacets);
		for(k3d::uint_t triangle = 0; triangle != nfacets; ++triangle)
		{
			Input.read(reinterpret_cast<char*>(&facets[triangle]), 50);
		}
	}
};

} // namespace io

} // namespace stl

} // namespace module


#endif /* MODULES_STL_IO_BINARY_STL_H */
