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

/** \file
		\brief Implements the LWO2Reader, which reads .lwo files (version 2) from LightWave 6
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/color.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/idocument_read_format.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/k3dilocation.h>
#include <k3dsdk/k3dimesh.h>
#include <k3dsdk/k3dipath.h>
#include <k3dsdk/k3dipathpointcollection.h>
#include <k3dsdk/k3dipoint.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <k3dsdk/fstream.h>

#include <iostream>

namespace
{

k3dIPoint* CreatePoint(k3dIMesh* const Mesh, const k3d::point3 Coords)
{
	// Sanity checks ...
	assert_warning(Mesh);

	k3dIPoint* const point = Mesh->CreatePoint();
	return_val_if_fail(point, 0);

	point->Location()->SetLocalXYZ(Coords);

	return point;
}

k3dIPath* CreatePath(k3dIMesh* const Mesh)
{
	// Sanity checks ...
	assert_warning(Mesh);

	k3dIPath* const path = Mesh->CreatePath();
	return_val_if_fail(path, 0);

	return path;
}

/////////////////////////////////////////////////////////////////////////////
// lwo2_reader_implementation

class lwo2_reader_implementation :
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
		const std::string ext = k3d::file_extension(FilePath);
		return "lwo" == ext || "lw" == ext;
	}

	bool read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath);

	static k3d::iplugin_factory& get_factory()
	{
		static application_plugin_factory<lwo2_reader_implementation, k3d::interface_list<k3d::idocument_read_format> > factory(
			k3d::uuid(0xee5d1140, 0x670843af, 0xba5fb9f3, 0x13d50f93),
			"LWO2Reader",
			_("LightWave 6 ( .lwo )"),
			"GeometryReader");

		return factory;
	}

private:
	unsigned short GetShort();
	unsigned long GetLong();
	double GetFloat();
	std::string GetString();
	unsigned long GetVariableLengthIndex();
	k3d::color GetColor();
	k3d::point3 GetVector();

	const std::string GetChunkId();
	bool GetChunkInfo();
	bool GetSubchunkInfo();
	void SkipChunk();
	void SkipSubchunk();

	/// Enumerates LightWave file formats
	typedef enum
	{
		LWOB,
		LWO2
	} LWOFormat;

	bool ReadLayer();

	bool LoadMesh(k3d::idocument& Document);

	bool ReadPNTS();
	bool ReadPOLSB(k3d::idocument& Document);
	bool ReadPOLS2(k3d::idocument& Document);
	bool ReadPTAG();
	bool ReadTAGS();
	bool ReadSurfaceDefinition();
	bool ReadVertexMapping();

	std::string m_CurrentChunkId;
	unsigned long m_ChunkLength;
	std::string m_CurrentSubchunkId;
	unsigned long m_SubchunkLength;

	unsigned char *m_Buf;
	unsigned long m_BufSize;
	unsigned char *m_Ptr;

	LWOFormat m_CurrentFormat;
	k3dIMesh* m_CurrentMesh;
	std::vector<k3dIPoint*> m_CurrentPointList;
};

bool lwo2_reader_implementation::read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
{
	k3d::log() << info << "Reading " << FilePath.native_string() << " with " << factory().name() << std::endl;
	
	// Allocate a buffer and load file
	k3d::filesystem::ifstream file(FilePath, std::ios::in | std::ios::binary);
	return_val_if_fail(file.good(), false);
	file.seekg(0, std::ios::end);
	m_BufSize = file.tellg();
	file.seekg(0);
	m_Buf = new (unsigned char)[m_BufSize];
	file.read(reinterpret_cast<char*>(m_Buf), m_BufSize);
	file.close();
	m_Ptr = m_Buf;

	bool ret = LoadMesh(Document);

	// Free buffer
	delete [] m_Buf;

	return ret;
}

const std::string lwo2_reader_implementation::GetChunkId()
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

bool lwo2_reader_implementation::GetChunkInfo()
{
	// Get chunk ID and chunk length
	if(m_Buf + m_BufSize - m_Ptr < 8)
		return false;

	m_CurrentChunkId = GetChunkId();
	m_ChunkLength = GetLong();
k3d::log() << debug << "Chunk ID : " << m_CurrentChunkId << ", length : " << m_ChunkLength << std::endl;

	return true;
}

bool lwo2_reader_implementation::GetSubchunkInfo()
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

void lwo2_reader_implementation::SkipChunk()
{
	if(m_Ptr + m_ChunkLength <= m_Buf + m_BufSize)
		m_Ptr += m_ChunkLength;
}

void lwo2_reader_implementation::SkipSubchunk()
{
	if(m_Ptr + m_SubchunkLength <= m_Buf + m_BufSize)
	{
		m_Ptr += m_SubchunkLength;
		m_ChunkLength -= m_SubchunkLength;
	}
}

bool lwo2_reader_implementation::LoadMesh(k3d::idocument& Document)
{
	// Check for LWO2 ID
	return_val_if_fail(GetChunkInfo() == true, false);
	return_val_if_fail(m_CurrentChunkId == "FORM", false);

	const std::string id = GetChunkId();
	return_val_if_fail(id == "LWO2" || id == "LWOB", false);

	m_CurrentFormat = lwo2_reader_implementation::LWO2;
	if(id == "LWOB")
		m_CurrentFormat = lwo2_reader_implementation::LWOB;

	// Create a new mesh object ...
	static k3d::iapplication_plugin_factory* const mesh_factory = dynamic_cast<k3d::iapplication_plugin_factory*>(k3d::plugin(k3d::classes::FrozenMesh()));
	return_val_if_fail(mesh_factory, false);

	k3d::inode* const object = mesh_factory->create_plugin(Document);
	return_val_if_fail(object, false);

	m_CurrentMesh = dynamic_cast<k3dIMesh*>(object);
	return_val_if_fail(m_CurrentMesh, false);

	object->set_name("LightWave object");

	Document.objects().add_objects(k3d::make_collection<k3d::inode_collection::objects_t>(object));

	// Loop through contained objects
	while(GetChunkInfo() == true)
		{
			if(m_CurrentChunkId == "LAYR")
				ReadLayer();
			else if(m_CurrentChunkId == "PNTS")
				ReadPNTS();
			else if(m_CurrentChunkId == "POLS")
				{
					switch(m_CurrentFormat)
						{
							case lwo2_reader_implementation::LWO2 :
								ReadPOLS2(Document);
							break;
							case lwo2_reader_implementation::LWOB :
								ReadPOLSB(Document);

							break;
							default:
								assert_not_reached();
						}
				}
			else if(m_CurrentChunkId == "PTAG")
				ReadPTAG();
			else if(m_CurrentChunkId == "TAGS")
				ReadTAGS();
			else if(m_CurrentChunkId == "SURF")
				ReadSurfaceDefinition();
			else if(m_CurrentChunkId == "VMAP")
				ReadVertexMapping();
			else
				{
					SkipChunk();
					k3d::log() << debug << "Skipping ..." << std::endl;
				}

		}

	return true;
}

unsigned short lwo2_reader_implementation::GetShort()
{
	assert_warning(k3d::little_endian());
	if(m_Ptr + 2 <= m_Buf + m_BufSize)
	{
		unsigned short ret = m_Ptr[1] + (m_Ptr[0]*0x100);
		m_Ptr += 2;

		m_ChunkLength -= 2;

		return ret;
	}
	else
		return 0;
}

unsigned long lwo2_reader_implementation::GetLong()
{
	assert_warning(k3d::little_endian());
	if(m_Ptr+4 <= m_Buf+m_BufSize)
	{
		unsigned long ret = m_Ptr[3] +
			(m_Ptr[2]*0x100) +
			(m_Ptr[1]*0x10000) +
			(m_Ptr[0]*0x1000000);
		m_Ptr += 4;

		m_ChunkLength -= 4;

		return ret;
	}
	else
		return 0;
}

double lwo2_reader_implementation::GetFloat()
{
	unsigned long raw = GetLong();
	float value = *((float*)(&raw));
	return static_cast<double>(value);
}

std::string lwo2_reader_implementation::GetString()
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

unsigned long lwo2_reader_implementation::GetVariableLengthIndex()
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

k3d::color lwo2_reader_implementation::GetColor()
{
	double red = GetFloat();
	double green = GetFloat();
	double blue = GetFloat();

	return k3d::color(red, green, blue);
}

k3d::point3 lwo2_reader_implementation::GetVector()
{
	double x = GetFloat();
	double y = GetFloat();
	double z = GetFloat();

	return k3d::point3(x, y, z);
}

bool lwo2_reader_implementation::ReadPNTS()
{
	unsigned long numv = m_ChunkLength / 4 / 3;

	// POLS indexes are defined within the latest PNTS list
	m_CurrentPointList.clear();

	for(unsigned long i = 0; i < numv; i++)
	{
		k3d::point3 coords;
		coords[0] = GetFloat();
		coords[1] = GetFloat();
		coords[2] = GetFloat();

		k3dIPoint* const point = CreatePoint(m_CurrentMesh, coords);
		return_val_if_fail(point, false);

		m_CurrentPointList.push_back(point);
	}

	return true;
}

bool lwo2_reader_implementation::ReadPOLSB(k3d::idocument& Document)
{
	while(m_ChunkLength > 0)
		{
			unsigned short numvert = GetShort();
			unsigned long facecount = static_cast<unsigned long>(numvert);

			// Create a path ...
			k3dIPath* const path = CreatePath(m_CurrentMesh);
			return_val_if_fail(path, false);

			for(unsigned long i = 0; i < facecount; i++)
				{
					unsigned short vid = GetShort();
					unsigned long index = static_cast<unsigned long>(vid);

					k3dIPathPoint* const pathpoint = path->Points()->AddPoint(m_CurrentPointList[index]);
					return_val_if_fail(pathpoint, false);
				}

			// Surface used by the polygon ...
			unsigned short surfid = GetShort();

			// All paths should have a material
			k3d::imaterial* const defaultmaterial = dynamic_cast<k3d::imaterial*>(k3d::default_material(Document));
			path->Materials()->set_material(defaultmaterial);
		}

	if(m_ChunkLength != 0)
		return false;

	return true;
}

bool lwo2_reader_implementation::ReadPOLS2(k3d::idocument& Document)
{
	const std::string chunk_id = GetChunkId();

	if(chunk_id != "FACE")
		{
			k3d::log() << debug << "Non face : " << chunk_id << std::endl;
		}

	while(m_ChunkLength > 0)
		{
			unsigned short flag = GetShort();

			flag &= 1024-1;
			unsigned long facecount = static_cast<unsigned long>(flag);

			// Create a path ...
			k3dIPath* const path = CreatePath(m_CurrentMesh);
			return_val_if_fail(path, false);

			for(unsigned long i = 0; i < facecount; i++)
				{
					unsigned short vid = GetShort();

					unsigned long index = static_cast<unsigned long>(vid);
					if(((vid >> 8) & 0xFF) == 0xFF)
						{
						index <<= 16;
						index += static_cast<unsigned long>(GetShort());
						}

					k3dIPathPoint* const pathpoint = path->Points()->AddPoint(m_CurrentPointList[index]);
					return_val_if_fail(pathpoint, false);
				}

			// All paths should have a material
			k3d::imaterial* const defaultmaterial = dynamic_cast<k3d::imaterial*>(k3d::default_material(Document));
			path->Materials()->set_material(defaultmaterial);
		}

	if(m_ChunkLength != 0)
		return false;

	return true;
}

bool lwo2_reader_implementation::ReadTAGS()
{
	k3d::log() << debug << "TAGS : " << std::endl;
	while(m_ChunkLength > 0)
		{
			std::string name = GetString();
			k3d::log() << debug << "   " << name << std::endl;
		}

	return true;
}

bool lwo2_reader_implementation::ReadPTAG()
{
	const std::string type = GetChunkId();

	if(type == "SURF")
	{
	}
	else if(type == "PART")
	{
	}
	else if(type == "SMPG")
	{
	}
	else
		assert_not_reached();

	while(m_ChunkLength > 0)
		{
			unsigned long poly = GetVariableLengthIndex();
			unsigned short tag = GetShort();
		}

	return true;
}

bool lwo2_reader_implementation::ReadVertexMapping()
{
	const std::string type = GetChunkId();

	if(type == "PICK")
		{
		}
	else if(type == "WGHT")
		{
		}
	else if(type == "MNVW")
		{
		}
	else if(type == "TXUV")
		{
		}
	else if(type == "RGB")
		{
		}
	else if(type == "RGBA")
		{
		}
	else if(type == "MORF")
		{
		}
	else if(type == "SPOT")
		{
		}
	else
		assert_not_reached();

	unsigned long dimension = static_cast<unsigned long>(GetShort());
	std::string name = GetString();
	k3d::log() << debug << "VMAP name : " << name << std::endl;

	while(m_ChunkLength > 0)
	{
		GetVariableLengthIndex();
		for(unsigned long i = 0; i < dimension; i++)
			GetFloat();
	}

	return true;
}

bool lwo2_reader_implementation::ReadSurfaceDefinition()
{
	std::string name = GetString();
	std::string source = GetString();

	k3d::log() << debug << "Name : " << name << ", source : " << source << std::endl;

	// Read sub-chunks
	while(GetSubchunkInfo())
	{
		if(m_CurrentSubchunkId == "COLR")
		{
			GetColor();
			GetVariableLengthIndex();
		}
		else if(m_CurrentSubchunkId == "SMAN")
		{
			double max_smoothing_angle = GetFloat();
		}
		else
		{
			k3d::log() << debug << "Unknown surface sub-chunk : skipping .." << std::endl;
			SkipSubchunk();
		}
	}

	return true;
}

bool lwo2_reader_implementation::ReadLayer()
{
	unsigned short number = GetShort();
	unsigned short flags = GetShort();
	k3d::point3 pivot = GetVector();
	std::string name = GetString();

	k3d::log() << debug << "Layer : " << name << std::endl;

	if(m_ChunkLength > 0)
	{
		unsigned short parent = GetShort();
	}
}

} // namespace

namespace libk3dgeometry
{

k3d::iplugin_factory& lwo2_reader_factory()
{
	return lwo2_reader_implementation::get_factory();
}

} // namespace libk3dgeometry


