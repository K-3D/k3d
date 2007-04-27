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
		\brief Implements the MaxReader K-3D object, which reads .3ds files from 3D Studio Max
		\author Ben Campbell (bcampbell@ntlworld.com)
*/

// STATUS (27Aug2001)
//
// Basically loads points and faces but not much else.
//
/** \todo  Reconstruct object hierarchy - sort out normal issues - add UVs to mesh - material handling - craps out on big models */

#include "maxids.h"

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/idocument_read_format.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/k3dilocation.h>
#include <k3dsdk/k3dimesh.h>
#include <k3dsdk/k3dipath.h>
#include <k3dsdk/k3dipathpointcollection.h>
#include <k3dsdk/k3dipoint.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <k3dsdk/fstream.h>

#include <iostream>

namespace
{

/////////////////////////////////////////////////////////////////////////////
// max_reader_implementation

class max_reader_implementation :
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
		return "3ds" == k3d::file_extension(FilePath);
	}

	bool read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath);

	static k3d::iplugin_factory& get_factory()
	{
		static application_plugin_factory<max_reader_implementation, k3d::interface_list<k3d::idocument_read_format> > factory(
			k3d::uuid(0x014cecbf, 0x0c0643aa, 0xa89e9ae5, 0x33646763),
			"3DSReader",
			_("3D Studio Max ( .3ds )"),
			"GeometryReader");

		return factory;
	}

private:
	unsigned short GetShort();
	unsigned long GetLong();
	float GetFloat();
	std::string GetString();

	bool GetChunkInfo();
	void SkipChunk();

	bool LoadMesh(k3d::idocument& Document);
	bool Handle_OBJMESH(k3d::idocument& Document, int chunkbytes);
	bool Handle_OBJBLOCK(k3d::idocument& Document, int chunkbytes);
	bool Handle_TRIMESH(int chunkbytes, k3dIMesh* mesh);

	unsigned short m_ChunkId;
	unsigned long m_ChunkLength;

	unsigned char *m_Buf;
	unsigned long m_Bufsize;
	unsigned char *m_Ptr;

	//std::map<std::string, k3d::imaterial*> materials;
	//k3d::imaterial* currentmaterial = 0;
};

bool max_reader_implementation::read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
{
	k3d::log() << info << "Reading " << FilePath.native_string() << " with " << factory().name() << std::endl;
	
	// Allocate a buffer and load file
	k3d::filesystem::ifstream file(FilePath, std::ios::in | std::ios::binary);
	return_val_if_fail(file.good(), false);
	file.seekg(0, std::ios::end);
	m_Bufsize = file.tellg();
	file.seekg(0);
	m_Buf = new (unsigned char)[m_Bufsize];
	file.read(reinterpret_cast<char*>(m_Buf), m_Bufsize);
	file.close();
	m_Ptr = m_Buf;

	bool ret = LoadMesh(Document);

	// Free buffer
	delete [] m_Buf;

	return ret;
}

bool max_reader_implementation::GetChunkInfo()
{
	// Get chunk ID and chunk length
	if(m_Buf+m_Bufsize-m_Ptr < 6)
		return false;

	m_ChunkId = GetShort();
	m_ChunkLength = GetLong() - 6;

	return true;
}

void max_reader_implementation::SkipChunk()
{
	if(m_Ptr + m_ChunkLength <= m_Buf + m_Bufsize)
		m_Ptr += m_ChunkLength;
}

bool max_reader_implementation::LoadMesh(k3d::idocument& Document)
{
	// Check 3DS ID
	return_val_if_fail(GetChunkInfo() == true, false);
	return_val_if_fail(m_ChunkId == MAXID_MAIN, false);

	// Get version and loop through contained objects
	unsigned long chunkbytes = m_ChunkLength;
//	k3d::log() << "main len: " << (chunkbytes+6) << std::endl;
	while(chunkbytes > 6)
		{
			return_val_if_fail(GetChunkInfo() == true, false);
//			k3d::log() << "Chunk 0x" << std::hex << m_ChunkId << " (" << std::dec << (m_ChunkLength+6) << " bytes)" << std::endl;

			chunkbytes -= m_ChunkLength + 6;

			switch(m_ChunkId)
				{
					case MAXID_VERSION:
//						k3d::log() << "  3DS Version: " << GetLong() << std::endl;
						GetLong();
						break;
					case MAXID_OBJMESH:
						if(Handle_OBJMESH(Document, m_ChunkLength) != true)
							return false;
						break;
					default:
						SkipChunk();
				}
		}

	return true;
}

std::string max_reader_implementation::GetString()
{
	std::string s;
	char c;
	while((c = (char)(*m_Ptr++)))
	{
		s += c;
	}

	return s;
}

unsigned short max_reader_implementation::GetShort()
{
	assert_warning(k3d::little_endian());
	if(m_Ptr+2 <= m_Buf+m_Bufsize)
	{
		unsigned short ret = m_Ptr[0] + (m_Ptr[1]*0x100);
		m_Ptr += 2;
		return ret;
	}
	else
		return 0;
}

unsigned long max_reader_implementation::GetLong()
{
	assert_warning(k3d::little_endian());
	if(m_Ptr+4 <= m_Buf+m_Bufsize)
	{
		unsigned long ret = m_Ptr[0] +
			(m_Ptr[1]*0x100) +
			(m_Ptr[2]*0x10000) +
			(m_Ptr[3]*0x1000000);
		m_Ptr += 4;
		return ret;
	}
	else
		return 0;
}

float max_reader_implementation::GetFloat()
{
	unsigned long raw = GetLong();
	return *((float*)(&raw));
}

bool max_reader_implementation::Handle_OBJMESH(k3d::idocument& Document, int chunkbytes)
{
//	k3d::log() << "Handle_OBJMESH()" << std::endl;
	while(chunkbytes>=6)
		{
			return_val_if_fail(GetChunkInfo() == true, false);
			chunkbytes -= m_ChunkLength + 6;

			switch(m_ChunkId)
				{
					case MAXID_MATERIAL:
//						k3d::log() << " MAXID_MATERIAL (Skipping)" << std::endl;
						SkipChunk();
	/*
						std::string materialname;
						while(1)
							{
								char c = stream.get();

								if(stream.eof())
									break;

								materialname += c;
							}

						if(materials.find(materialname) == materials.end())
							{
								k3d::inode* materialobject = Document.CreateObject(sdpMaterial);
								return_val_if_fail(materialobject, false);
								k3d::imaterial* material = dynamic_cast<k3d::imaterial*>(materialobject);
								return_val_if_fail(material, false);

								materialobject->SetName(materialname);
								materials[materialname] = material;
							}

						currentmaterial = materials[materialname];

	*/
						break;
					case MAXID_ONEUNIT:
						GetFloat();
//						k3d::log() << " MAXID_ONEUNIT:  (" << GetFloat() << ")" << std::endl;
						break;
					case MAXID_OBJBLOCK:
						if(!Handle_OBJBLOCK(Document, m_ChunkLength))
							return false;
						break;
					default:
//						k3d::log() << " skipping unknown Chunk 0x" << std::hex << m_ChunkId << "(" << std::dec << (m_ChunkLength+6) << " bytes)" << std::endl;
						SkipChunk();
				}
		}

	return true;
}

bool max_reader_implementation::Handle_OBJBLOCK(k3d::idocument& Document, int chunkbytes)
{
	std::string s = GetString();
	chunkbytes -= (s.size()+1);

	static k3d::iapplication_plugin_factory* const mesh_factory = dynamic_cast<k3d::iapplication_plugin_factory*>(k3d::plugin(k3d::classes::FrozenMesh()));
	return_val_if_fail(mesh_factory, false);

	k3d::inode* const object = mesh_factory->create_plugin(Document);
	return_val_if_fail(object, false);

	k3dIMesh* const mesh = dynamic_cast<k3dIMesh*>(object);
	return_val_if_fail(mesh, false);

	object->set_name(s);

	Document.objects().add_objects(k3d::make_collection<k3d::inode_collection::objects_t>(object));

//	k3d::log() << "  Object '" << s.c_str() << "'" << std::endl;

	while(chunkbytes >= 6)
		{
			return_val_if_fail(GetChunkInfo() == true, false);
			chunkbytes -= m_ChunkLength + 6;

			switch(m_ChunkId)
				{
					case MAXID_TRIMESH:
//						k3d::log() << " MAXID_TRIMESH" << std::endl;
						if(!Handle_TRIMESH(m_ChunkLength, mesh))
							return false;

						mesh->CalculateNormals();

						break;
					case MAXID_CAMERA:
//						k3d::log() << " MAXID_CAMERA (skipping)" << std::endl;
						SkipChunk();
						break;
					default:
//						k3d::log() << " skipping unhandled Chunk 0x" << std::hex << m_ChunkId << "(" << std::dec << (m_ChunkLength+6) << " bytes)" << std::endl;
						SkipChunk();
				}
		}

	return true;
}

bool max_reader_implementation::Handle_TRIMESH(int chunkbytes, k3dIMesh* mesh)
{
	std::vector<k3d::point3> verts;

	while(chunkbytes >= 6)
		{
			return_val_if_fail(GetChunkInfo() == true, false);
			chunkbytes -= m_ChunkLength + 6;

			switch(m_ChunkId)
				{
					case MAXID_VERTLIST:
					{
//						k3d::log() << "   MAXID_VERTLIST" << std::endl;
						verts.clear();

						unsigned long numverts = (unsigned long)GetShort();
//						k3d::log() << "    [" << numverts << " verts]" << std::endl;
						for(unsigned long i = 0; i < numverts; i++)
							{
								float x = GetFloat();
								float y = GetFloat();
								float z = GetFloat();

								verts.push_back(k3d::point3(x,y,z));
							}
						break;
					}
					case MAXID_FACELIST:
					{
//						k3d::log() << "   MAXID_FACELIST";
						int bytesleft = m_ChunkLength;

						unsigned long numfaces = (unsigned long)GetShort();
						bytesleft -= 2;
//						k3d::log() << "    [" << numfaces << " faces]" << std::endl;
						for(unsigned long facenum = 0; facenum < numfaces; facenum++)
							{
								k3dIPath* const path = mesh->CreatePath();
								return_val_if_fail(path, false);

								int indices[3];
								indices[0] = (int)GetShort();
								indices[1] = (int)GetShort();
								indices[2] = (int)GetShort();
								GetShort();	// skip flags for now

								int q;
								for(q=0; q<3; ++q)
								{
									if(indices[q] >= (int)verts.size())
									{
										k3d::log() << "vertex out-of-range" << std::endl;
										return false;
									}

									k3dIPoint* const point = mesh->CreatePoint();
									return_val_if_fail(point, false);

									k3d::point3 pos = verts[ indices[q] ];

									point->Location()->SetLocalXYZ(pos);

		//							k3dIPathPoint* pathpoint =
										path->Points()->AddPoint(point);

								}

								// construct a face normal vector

		//						k3d::point3 a = verts[indices[1]] - verts[ indices[0] ];
		//						k3d::point3 b = verts[indices[1]] - verts[ indices[0] ];
		//						k3d::point3 n = a ^ b;	// a cross b
		//						n.Normalize();
		//						path->SetPathNormal(n);

								bytesleft -= 8;
							}

						while(bytesleft >= 6)
							{
								return_val_if_fail(GetChunkInfo() == true, false);
								bytesleft -= m_ChunkLength + 6;

								switch(m_ChunkId)
									{
										case MAXID_FACEMAT:
											{
//												k3d::log() << "    MAXID_FACEMAT" << std::endl;
												std::string matname = GetString();
//												k3d::log() << "     name: " << matname.c_str() << std::endl;

												int numfaces = (int)GetShort();
//												k3d::log() << "     " << numfaces << " faces" << std::endl;
												int i;
												for(i = 0; i < numfaces; i++)
													GetShort();	// facenum
											}
											break;
										case MAXID_SMOOLIST:
											{
												// not sure if this is correct place...
//												k3d::log() << "    MAXID_SMOOLIST, child of FACELIST (skipping)" << std::endl;
												SkipChunk();
											}
											break;
										default:
//											k3d::log() << " skipping unhandled Chunk 0x" << std::hex << m_ChunkId << "(" << std::dec << (m_ChunkLength+6) << " bytes)" << std::endl;
											SkipChunk();
									}
							}
					}
						break;
					case MAXID_TRMATRIX:
//						k3d::log() << "   MAXID_TRMATRIX" << std::endl;

						float x,y,z;

						x=GetFloat();
						y=GetFloat();
						z=GetFloat();
//						printf("    X axis: %f %f %f\n",x,y,z);
						x=GetFloat();
						y=GetFloat();
						z=GetFloat();
//						printf("    Y axis: %f %f %f\n",x,y,z);
						x=GetFloat();
						y=GetFloat();
						z=GetFloat();
//						printf("    Z axis: %f %f %f\n",x,y,z);
						x=GetFloat();
						y=GetFloat();
						z=GetFloat();
//						printf("    Origin: %f %f %f\n",x,y,z);
						break;
					case MAXID_MAPLIST:
					{
//						k3d::log() << "   MAXID_MAPLIST" << std::endl;

						int numuvs = (int)GetShort();
						int i;

//						k3d::log() << "    [" << numuvs << " UV coords]" << std::endl;
						for(i=0; i<numuvs; ++i)
						{
							GetFloat();	// u
							GetFloat();	// v
						/** \todo add to mesh data */
	/*
							pathpoint->Location()->SetTextureXYZ(Vertex(texture_vertices, texture_index));
							pathpoint->Location()->SetImplicitTextureXYZ(Vertex(texture_vertices, texture_index));
	*/
						}
						break;
					}
					case MAXID_SMOOLIST:
						// not sure if this is correct place...
//						k3d::log() << "   MAXID_SMOOLIST, child of TRIMESH (skipping)" << std::endl;
						SkipChunk();
						break;
					default:
//						k3d::log() << " skipping unhandled Chunk 0x" << std::hex << m_ChunkId << "(" << std::dec << (m_ChunkLength+6) << " bytes)" << std::endl;
						SkipChunk();
						break;
				}
		}

	return true;
}

} // namespace

namespace libk3dgeometry
{

k3d::iplugin_factory& max_reader_factory()
{
	return max_reader_implementation::get_factory();
}

} // namespace libk3dgeometry


