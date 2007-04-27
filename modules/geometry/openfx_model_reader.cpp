// K-3D
// Copyright (c) 2004-2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\brief Implements the MFXReader, which reads .mfx files from OpenFX
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/color.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/idocument_read_format.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include "helpers.h"

#include <k3dsdk/fstream.h>

namespace libk3dgeometry
{

/////////////////////////////////////////////////////////////////////////////
// mfx_reader_implementation

class mfx_reader_implementation :
	public k3d::ifile_format,
	public k3d::idocument_read_format,
	public k3d::ideletable
{
public:
	unsigned long priority()
	{
		return 128;
	}

	bool query_can_handle(const k3d::filesystem::path& FilePath)
	{
		return k3d::filesystem::extension(FilePath).lowercase().raw() == ".mfx";
	}

	bool read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath);

	static k3d::iplugin_factory& get_factory()
	{
		static application_plugin_factory<mfx_reader_implementation, k3d::interface_list<k3d::idocument_read_format> > factory(
			k3d::uuid(0xfa7c6bb0, 0x41ab44b3, 0xae92f484, 0xdb0f1e9b),
			"MFXReader",
			"OpenFX Model ( .mfx )",
			"");

		return factory;
	}

private:
	unsigned char GetChar();
	unsigned short GetShort();
	unsigned long GetLong();
	double GetFloat();
	std::string GetString();
	std::string GetString(unsigned long size);
	unsigned long GetVariableLengthIndex();
	k3d::color GetColor();
	k3d::point3 GetVector();

	const std::string GetChunkId();
	bool GetChunkInfo();
	bool GetSubchunkInfo();
	void SkipChunk();
	void SkipSubchunk();

	/// Enumerates file formats
	typedef enum
	{
		OFXM,
		AAPO
	} file_format_t;

	bool load_model(k3d::idocument& Document);

	bool read_vertices();
	bool read_edges(bool short_form);
	bool read_faces(bool short_form);
	bool read_skeleton(unsigned int version);

	std::string m_CurrentChunkId;
	unsigned long m_ChunkLength;
	std::string m_CurrentSubchunkId;
	unsigned long m_SubchunkLength;

	unsigned char* m_Buf;
	unsigned long m_BufSize;
	unsigned char* m_Ptr;

	file_format_t m_CurrentFormat;

	// Mesh variables
	k3d::inode* m_current_mesh_object;
	k3d::inode* m_current_mesh_instance;
	k3d::legacy::mesh* m_current_mesh;
	k3d::imaterial* default_material;

	std::vector<k3d::legacy::point*> m_CurrentPointList;

	// Mesh functions
	bool create_mesh(k3d::idocument& Document)
	{
		// Create document object ...
		k3d::legacy::mesh* const mesh = detail::create_mesh(Document, "OpenFX mesh", m_current_mesh_object, m_current_mesh_instance);
		return_val_if_fail(mesh, false);
		m_current_mesh = mesh;

		return true;
	}
};

bool mfx_reader_implementation::read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
{
	k3d::log() << info << "Reading " << FilePath.native_console_string() << " with " << factory().name() << std::endl;

	// Allocate a buffer and load file
	k3d::filesystem::ifstream file(FilePath, std::ios::in | std::ios::binary);
	return_val_if_fail(file.good(), false);
	file.seekg(0, std::ios::end);
	m_BufSize = file.tellg();
	file.seekg(0);
	typedef unsigned char buffer_t;
	m_Buf = new buffer_t[m_BufSize];
	file.read(reinterpret_cast<char*>(m_Buf), m_BufSize);
	file.close();
	m_Ptr = m_Buf;

	// Get default material
	default_material = dynamic_cast<k3d::imaterial*>(k3d::default_material(Document));

	bool ret = load_model(Document);

	// Free buffer
	delete [] m_Buf;

	return ret;
}

const std::string mfx_reader_implementation::GetChunkId()
{
	unsigned long id = GetLong();

	std::string name("");

	unsigned long val = (id >> 24) & 0xFF;
	if(!val)
		return name;
	name += static_cast<char>(val);

	val = (id >> 16) & 0xFF;
	if(!val)
		return name;
	name += static_cast<char>(val);

	val = (id >> 8) & 0xFF;
	if(!val)
		return name;
	name += static_cast<char>(val);

	val = id & 0xFF;
	if(!val)
		return name;
	name += static_cast<char>(val);

	return name;
}

bool mfx_reader_implementation::GetChunkInfo()
{
	if(m_ChunkLength)
		{
			k3d::log() << debug << "Previous chunk not empty, remaining : " << m_ChunkLength << std::endl;
		}

	// Get chunk ID and chunk length
	if(m_Buf + m_BufSize - m_Ptr < 8)
		return false;

	m_CurrentChunkId = GetChunkId();
	m_ChunkLength = GetLong();
	k3d::log() << debug << "Chunk ID : " << m_CurrentChunkId << ", length : " << m_ChunkLength << std::endl;

	return true;
}

bool mfx_reader_implementation::GetSubchunkInfo()
{
	// Get sub-chunk ID and chunk length
	if(m_Buf + m_BufSize - m_Ptr < 6)
		return false;

	if(m_ChunkLength == 0)
		return false;

	m_CurrentSubchunkId = GetChunkId();
	m_SubchunkLength = static_cast<unsigned long>(GetShort());
	k3d::log() << debug << "Sub-chunk " << m_CurrentSubchunkId << ", length = " << m_SubchunkLength << std::endl;

	return true;
}

void mfx_reader_implementation::SkipChunk()
{
	if(m_Ptr + m_ChunkLength <= m_Buf + m_BufSize)
		m_Ptr += m_ChunkLength;

	m_ChunkLength = 0;
}

void mfx_reader_implementation::SkipSubchunk()
{
	if(m_Ptr + m_SubchunkLength <= m_Buf + m_BufSize)
	{
		m_Ptr += m_SubchunkLength;
		m_ChunkLength -= m_SubchunkLength;
	}
}

bool mfx_reader_implementation::load_model(k3d::idocument& Document)
{
	// Check for header
	return_val_if_fail(GetChunkInfo(), false);
	return_val_if_fail(m_CurrentChunkId == "FORM", false);

	const std::string id = GetChunkId();
	return_val_if_fail(id == "OFXM" || id == "AAPO", false);

	m_CurrentFormat = mfx_reader_implementation::OFXM;
	if(id == "AAPO")
		m_CurrentFormat = mfx_reader_implementation::AAPO;

	// Create a new mesh object ...
	return_val_if_fail(create_mesh(Document), false);

	// Loop through contained objects
	while(GetChunkInfo() == true)
		{
			if(m_CurrentChunkId == "VERT")
				read_vertices();
			else if(m_CurrentChunkId == "edge")
				read_edges(true);
			else if(m_CurrentChunkId == "EDGE")
				read_edges(false);
			else if(m_CurrentChunkId == "SFCE")
				read_faces(true);
			else if(m_CurrentChunkId == "LFCE")
				read_faces(false);
			else if(m_CurrentChunkId == "SKEL")
				read_skeleton(0);
			else if(m_CurrentChunkId == "SK01")
				read_skeleton(1);
			else if(m_CurrentChunkId == "SK02")
				read_skeleton(2);
			else
				{
					k3d::log() << debug << "Skipping .mfx chunk " << m_CurrentChunkId << std::endl;
					SkipChunk();
				}

		}

	return true;
}

unsigned char mfx_reader_implementation::GetChar()
{
	if(m_Ptr + 1 <= m_Buf + m_BufSize)
	{
		unsigned char ret = m_Ptr[0];
		m_Ptr += 1;

		m_ChunkLength -= 1;

		return ret;
	}

	return 0;
}

unsigned short mfx_reader_implementation::GetShort()
{
	assert_warning(k3d::little_endian());
	if(m_Ptr + 2 <= m_Buf + m_BufSize)
	{
		unsigned short ret = m_Ptr[1] + (m_Ptr[0]*0x100);
		m_Ptr += 2;

		m_ChunkLength -= 2;

		return ret;
	}

	return 0;
}

unsigned long mfx_reader_implementation::GetLong()
{
	/** \todo implement for big endian */
	assert_warning(k3d::little_endian());

	if(m_Ptr+4 <= m_Buf+m_BufSize)
		{
			unsigned long ret = static_cast<unsigned long>(m_Ptr[3]) & 0xff;
			ret |= (static_cast<unsigned long>(m_Ptr[2]) & 0xff) << 8;
			ret |= (static_cast<unsigned long>(m_Ptr[1]) & 0xff) << 16;
			ret |= (static_cast<unsigned long>(m_Ptr[0]) & 0xff) << 24;

			m_Ptr += 4;

			m_ChunkLength -= 4;

			return ret;
		}
	else
		{
			k3d::log() << k3d::error << "Unexpected end of file." << std::endl;
			return 0;
		}
}

double mfx_reader_implementation::GetFloat()
{
	unsigned long raw = GetLong();
	float value = *((float*)(&raw));
	return static_cast<double>(value);
}

std::string mfx_reader_implementation::GetString()
{
	std::string s;

	char c;
	while((c = (char)(*m_Ptr++)))
		s += c;

	// Strings always have even length
	if((s.length()+1) % 2 == 1)
		m_Ptr++;

	m_ChunkLength -= s.size() + 1;
	if((s.size() + 1) % 2 == 1)
		m_ChunkLength--;

	return s;
}

std::string mfx_reader_implementation::GetString(unsigned long size)
{
	std::string s;

	bool string_ok = true;
	for(unsigned long i = 0; i < size; i++)
		{
			char c = (char)(*m_Ptr++);
			if(c == 0)
				string_ok = false;

			if(string_ok)
				s += c;
		}

	m_ChunkLength -= size;

	return s;
}

unsigned long mfx_reader_implementation::GetVariableLengthIndex()
{
	assert_warning(k3d::little_endian());
	assert_warning(m_Ptr + 2 <= m_Buf + m_BufSize);
	if(m_Ptr + 2 <= m_Buf + m_BufSize)
	{
		if(m_Ptr[0] != 0xFF)
			return static_cast<unsigned long>(GetShort());
		else
			return GetLong() & 0x00FFFFFF;
	}
	else
		return 0;
}

k3d::color mfx_reader_implementation::GetColor()
{
	double red = static_cast<double>(GetChar()) / 255;
	double green = static_cast<double>(GetChar()) / 255;
	double blue = static_cast<double>(GetChar()) / 255;

	return k3d::color(red, green, blue);
}

k3d::point3 mfx_reader_implementation::GetVector()
{
	double x = GetFloat();
	double y = GetFloat();
	double z = GetFloat();

	return k3d::point3(x, y, z);
}

bool mfx_reader_implementation::read_vertices()
{
	unsigned long new_vertices = m_ChunkLength / 12;
	if(new_vertices == 0)
		return true;

	return_val_if_fail(m_current_mesh, false);

	// POLS indexes are defined within the latest PNTS list
	m_CurrentPointList.clear();

	for(unsigned long i = 0; i < new_vertices; i++)
	{
		k3d::point3 coords;
		coords[0] = static_cast<double>(static_cast<signed long>(GetLong())) * 1.0e-4;
		coords[2] = static_cast<double>(static_cast<signed long>(GetLong())) * 1.0e-4;
		coords[1] = static_cast<double>(static_cast<signed long>(GetLong())) * 1.0e-4;

		k3d::legacy::point* const point = new k3d::legacy::point(coords);
		return_val_if_fail(point, false);

		m_CurrentPointList.push_back(point);
		m_current_mesh->points.push_back(point);
	}

	return true;
}

bool mfx_reader_implementation::read_edges(bool short_form)
{
	unsigned long new_edges = m_ChunkLength;
	if(short_form)
		new_edges /= 4;
	else
		new_edges /= 8;

	if(new_edges == 0)
		return true;

	for(unsigned long i = 0; i < new_edges; i++)
		{
			unsigned long e1;
			unsigned long e2;

			if(short_form)
				{
					e1 = static_cast<unsigned long>(GetShort());
					e2 = static_cast<unsigned long>(GetShort());
				}
			else
				{
					e1 = GetLong();
					e2 = GetLong();
				}
		}

	return true;
}

bool mfx_reader_implementation::read_faces(bool short_form)
{
	unsigned long new_faces = m_ChunkLength;
	if(short_form)
		new_faces /= 18;
	else
		new_faces /= 24;

	if(new_faces == 0)
		return true;

	k3d::legacy::polyhedron* const polyhedron = new k3d::legacy::polyhedron();
	return_val_if_fail(polyhedron, false);
	m_current_mesh->polyhedra.push_back(polyhedron);

	for(unsigned long i = 0; i < new_faces; i++)
		{
			unsigned long f1;
			unsigned long f2;
			unsigned long f3;

			if(short_form)
				{
					f1 = static_cast<unsigned long>(GetShort());
					f2 = static_cast<unsigned long>(GetShort());
					f3 = static_cast<unsigned long>(GetShort());
				}
			else
				{
					f1 = GetLong();
					f2 = GetLong();
					f3 = GetLong();
				}

			k3d::color color = GetColor();
			k3d::color matcolor = GetColor();
			int attrib = static_cast<int>(GetChar());
			int param = static_cast<int>(GetChar());
			int map = static_cast<int>(GetChar());
			int shader = static_cast<int>(GetChar());
			int axis = static_cast<int>(GetChar());
			int pad = static_cast<int>(GetChar());
//std::cout << " face params : " << attrib << " " << param << " " << map << " " << shader << " " << axis << pad << " | " << color << " , " << matcolor << std::endl;

			// Create a triangle ...
			std::vector<k3d::legacy::split_edge*> edges;
			assert_warning(f1 < m_CurrentPointList.size());
			assert_warning(f2 < m_CurrentPointList.size());
			assert_warning(f3 < m_CurrentPointList.size());
			edges.push_back(new k3d::legacy::split_edge(m_CurrentPointList[f1]));
			edges.push_back(new k3d::legacy::split_edge(m_CurrentPointList[f2]));
			edges.push_back(new k3d::legacy::split_edge(m_CurrentPointList[f3]));

			k3d::legacy::loop_edges(edges.begin(), edges.end());

			k3d::legacy::face* const face = new k3d::legacy::face(*edges.begin(), default_material);
			return_val_if_fail(face, false);

			polyhedron->faces.push_back(face);

			// Set color
			face->uniform_data["Cs"] = color;
		}

	// Set companions
	k3d::legacy::set_companions(*polyhedron);

	return true;
}

bool mfx_reader_implementation::read_skeleton(unsigned int version)
{
	unsigned long n = GetLong();
	if(n == 0)
		return true;

	unsigned long x = GetLong();
	unsigned long y = GetLong();
	unsigned long z = GetLong();
	unsigned short id = GetShort();

	if(version > 0)
		{
			// Load name
			std::string name = GetString(16);
			k3d::log() << debug << "Skeleton root : " << name << std::endl;
		}

	std::vector<double> u;
	std::vector<double> v;
	std::vector<double> w;
	std::vector<k3d::point3> bx;
	if(version > 1)
		{
			for(unsigned long j = 0; j < 3; ++j)
				{
					unsigned long value = GetLong();
					u.push_back(static_cast<double>(value) * 1.0e-8);
				}
			for(unsigned long j = 0; j < 3; ++j)
				{
					unsigned long value = GetLong();
					v.push_back(static_cast<double>(value) * 1.0e-8);
				}
			for(unsigned long j = 0; j < 3; ++j)
				{
					unsigned long value = GetLong();
					w.push_back(static_cast<double>(value) * 1.0e-8);
				}

			for(unsigned long j = 0; j < 8; ++j)
				{
					unsigned long value_x = GetLong();
					unsigned long value_y = GetLong();
					unsigned long value_z = GetLong();
					bx.push_back(k3d::point3(value_x, value_y, value_z));
				}
		}

	for(unsigned long i = 1; i < n; i++)
		{
			unsigned long x = GetLong();
			unsigned long y = GetLong();
			unsigned long z = GetLong();
			unsigned short id = GetShort();

			if(version > 0)
				{
					// Load name
					std::string name = GetString(16);
					k3d::log() << debug << "Skeleton part : " << name << std::endl;
				}

			std::vector<double> u;
			std::vector<double> v;
			std::vector<double> w;
			std::vector<k3d::point3> bx;
			if(version > 1)
				{
					for(unsigned long j = 0; j < 3; ++j)
						{
							unsigned long value = GetLong();
							u.push_back(static_cast<double>(value) * 1.0e-8);
						}
					for(unsigned long j = 0; j < 3; ++j)
						{
							unsigned long value = GetLong();
							v.push_back(static_cast<double>(value) * 1.0e-8);
						}
					for(unsigned long j = 0; j < 3; ++j)
						{
							unsigned long value = GetLong();
							w.push_back(static_cast<double>(value) * 1.0e-8);
						}

					for(unsigned long j = 0; j < 8; ++j)
						{
							unsigned long value_x = GetLong();
							unsigned long value_y = GetLong();
							unsigned long value_z = GetLong();
							bx.push_back(k3d::point3(value_x, value_y, value_z));
						}
				}
		}

	n = GetLong();
	if(n != m_CurrentPointList.size())
		{
			for(unsigned long i = 0; i < n; i++)
				GetShort();
		}
	else
		{
			// Which point to which skeleton part
			for(unsigned long i = 0; i < n; i++)
				{
					unsigned short dummy = GetShort();
				}
		}

	return true;
}

k3d::iplugin_factory& mfx_reader_factory()
{
	return mfx_reader_implementation::get_factory();
}

} // namespace libk3dgeometry

