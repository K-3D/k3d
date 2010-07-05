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

	/// Sort using colors, to support the Hexpress mesher from Numeca
	bool operator<(const facet& b) const
	{
		return color < b.color;
	}
};

/// Encapsulates the STL binary data format
struct binary_stl
{
	/// Default constructor, write K-3D version info into the header
	binary_stl()
	{
		k3d::string_t headstr =  k3d::string_t("K-3D ") + k3d::string_t(K3D_VERSION) + k3d::string_t(" STL writer");
		for(k3d::uint_t i = 0 ; i != headstr.size(); ++i)
			header[i] = headstr[i];
	}

	/// Magics format constructor, writes material info into the header
	binary_stl(const k3d::color& Color, const k3d::color& Diffuse, const k3d::color& Specular, const k3d::color& Ambient)
	{
		const k3d::uint8_t color[] = {Color.red*255, Color.green*255, Color.blue*255, 0};
		const k3d::uint8_t diffuse[] = {Diffuse.red*255, Diffuse.green*255, Diffuse.blue*255, 0};
		const k3d::uint8_t specular[] = {Specular.red*255, Specular.green*255, Specular.blue*255, 0};
		const k3d::uint8_t ambient[] = {Ambient.red*255, Ambient.green*255, Ambient.blue*255, 0};
		const k3d::string_t color_str(reinterpret_cast<const char*>(color));
		const k3d::string_t diffuse_str(reinterpret_cast<const char*>(diffuse));
		const k3d::string_t specular_str(reinterpret_cast<const char*>(specular));
		const k3d::string_t ambient_str(reinterpret_cast<const char*>(ambient));
		k3d::string_t headstr =  k3d::string_t("K-3D ") + k3d::string_t(K3D_VERSION) + k3d::string_t(" STL writer COLOR=")
															+ color_str
															+ ",MATERIAL="
															+ diffuse_str
															+ specular_str
															+ ambient_str;
		std::copy(headstr.begin(), headstr.end(), header);
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

/// Switches the order of the two bytes that make up N
inline k3d::uint16_t switch_bytes(k3d::uint16_t N)
{
	// switch byte order
	k3d::uint8_t* result_array = reinterpret_cast<k3d::uint8_t*>(&N);
	k3d::uint8_t a = result_array[0];
	result_array[0] = result_array[1];
	result_array[1] = a;
	return N;
}

} // namespace io

} // namespace stl

} // namespace module


#endif // !MODULES_STL_IO_BINARY_STL_H
