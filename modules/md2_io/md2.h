#ifndef _MD2_H_
#define _MD2_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <k3dsdk/algebra.h>
#include <k3dsdk/texture3.h>

namespace module
{

namespace md2
{

namespace io
{

typedef unsigned char byte;

typedef char md2Skins[64];

struct md2Header
{

	k3d::int32_t id;
	k3d::int32_t version;
	k3d::int32_t skinWidth;
	k3d::int32_t skinHeight;
	k3d::int32_t frameSize;
	k3d::int32_t numSkins;
	k3d::int32_t numVertex;
	k3d::int32_t numTexCoords;
	k3d::int32_t numTriangles;
	k3d::int32_t numGLCommands;
	k3d::int32_t numFrames;
	k3d::int32_t offsetSkins;
	k3d::int32_t offsetTexCoords;
	k3d::int32_t offsetTriangles;
	k3d::int32_t offsetFrames;
	k3d::int32_t offsetGLCommands;
	k3d::int32_t fileSize;

};

struct md2Vertex
{
	float val[3];
};

struct md2TriangleVertex
{
	byte val[3];
	byte lightnormalIndex;
};

struct md2SingleFrame
{
	float scale[3];
	float translate[3];
	char name[16];
	md2TriangleVertex vertex[1];
};

struct md2Frames
{
	char name[16];
	md2Vertex *vertex;
};

struct md2Triangles
{
	k3d::uint16_t vertexIndices[3];
	k3d::uint16_t textureIndices[3];
};

struct md2TmpTexCoords
{
	k3d::int16_t x, y;
};

struct md2TexCoords
{
	float val[2];
};

class md2Model
{
public:
	md2Model(const char *f);
	~md2Model();

	void draw(k3d::int32_t fr);
	void loadSkin(char *f);

	k3d::int32_t get_num_triangles(){return header.numTriangles;}
	k3d::int32_t get_num_frames(){return header.numFrames;}
	k3d::int32_t get_num_vertices(){return header.numVertex;}
	k3d::int32_t get_num_texcoords(){return header.numTexCoords;}
	k3d::int32_t get_index(k3d::int32_t triangle, k3d::int32_t p){return triangles[triangle].vertexIndices[p];}
	k3d::int32_t get_texindex(k3d::int32_t triangle, k3d::int32_t p){return triangles[triangle].textureIndices[p];}

	k3d::point3 get_point(k3d::int32_t frame, k3d::int32_t index)
	{
		return k3d::point3(
			frames[frame].vertex[index].val[0],
			frames[frame].vertex[index].val[1],
			frames[frame].vertex[index].val[2]);
	}

	k3d::point3 get_point(k3d::int32_t frame, k3d::int32_t triangle, k3d::int32_t p)
	{
		return k3d::point3(
			frames[frame].vertex[triangles[triangle].vertexIndices[p]].val[0],
			frames[frame].vertex[triangles[triangle].vertexIndices[p]].val[1],
			frames[frame].vertex[triangles[triangle].vertexIndices[p]].val[2]);
	}
	
	k3d::texture3 get_texcoord(k3d::int32_t index)
	{
		return k3d::texture3(
			texcoords[index].val[0],
			1-texcoords[index].val[1],
			1);
	}
	
private:
	md2Header header;
	md2Triangles *triangles;
	md2Frames *frames;
	md2TexCoords *texcoords;
	md2Skins *skins;

};

}
}
}

#endif

