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

typedef struct
{

	int id;
	int version;
	int skinWidth;
	int skinHeight;
	int frameSize;
	int numSkins;
	int numVertex;
	int numTexCoords;
	int numTriangles;
	int numGLCommands;
	int numFrames;
	int offsetSkins;
	int offsetTexCoords;
	int offsetTriangles;
	int offsetFrames;
	int offsetGLCommands;
	int fileSize;

}md2Header;

typedef struct
{
	float val[3];
}md2Vertex;

typedef struct
{
	byte val[3];
	byte lightnormalIndex;
}md2TriangleVertex;

typedef struct 
{
	float scale[3];
	float translate[3];
	char name[16];
	md2TriangleVertex vertex[1];
}md2SingleFrame;

typedef struct
{
	char name[16];
	md2Vertex *vertex;
}md2Frames;

typedef struct
{
	unsigned short vertexIndices[3];
	unsigned short textureIndices[3];
}md2Triangles;

typedef struct
{
	short x, y;
}md2TmpTexCoords;

typedef struct
{
	float val[2];
}md2TexCoords;

class md2Model
{
public:
	md2Model(const char *f);
	~md2Model();

	void draw(int fr);
	void loadSkin(char *f);

	int get_num_triangles(){return header.numTriangles;}
	int get_num_frames(){return header.numFrames;}
	int get_num_vertices(){return header.numVertex;}
	int get_num_texcoords(){return header.numTexCoords;}
	int get_index(int triangle, int p){return triangles[triangle].vertexIndices[p];}
	int get_texindex(int triangle, int p){return triangles[triangle].textureIndices[p];}
	k3d::point4 get_point(int frame, int index)
	{
		return k3d::point4(frames[frame].vertex[index].val[0],
							frames[frame].vertex[index].val[1],
							frames[frame].vertex[index].val[2],1);
	}
	k3d::point4 get_point(int frame, int triangle, int p)
	{
		return k3d::point4(frames[frame].vertex[triangles[triangle].vertexIndices[p]].val[0],
							frames[frame].vertex[triangles[triangle].vertexIndices[p]].val[1],
							frames[frame].vertex[triangles[triangle].vertexIndices[p]].val[2], 1);
	}
	k3d::texture3 get_texcoord(int index)
	{
		return k3d::texture3(texcoords[index].val[0],
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

