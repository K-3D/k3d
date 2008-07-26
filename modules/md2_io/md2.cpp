#include "md2.h"

md2Model::md2Model(const char *f)
{
//////////////////////////////////////////////////////////////
///////////////////////Model Loading//////////////////////////
//////////////////////////////////////////////////////////////
	FILE *md2file = fopen(f, "rb");

//Check if the file was loaded correctly
	if(md2file == NULL)
	{
		printf("Could not load %s model!\n", f);
		exit(0);
	}
//Extract Header and check if it is a valid md2 file
	fread(&header, 1, sizeof(md2Header), md2file);	
	
	if((header.id != 844121161) || (header.version != 8))
	{
		printf("%s is not a valid md2 model!\n", f);
	}
//Extract Skin names
	skins = new md2Skins[header.numSkins];
	for(int i=0; i<header.numSkins;i++)
		fread(skins[i],1,sizeof(md2Skins), md2file);
//Extract Texture coordinates
	texcoords = new md2TexCoords[header.numTexCoords];
	md2TmpTexCoords tmpcoords;
	fseek(md2file, header.offsetTexCoords, SEEK_SET);
	for(int i=0; i<header.numTexCoords;i++)
	{
		//Converting values to [0,1]
		fread(&tmpcoords, sizeof(md2TmpTexCoords), 1, md2file);
		texcoords[i].val[0] = (float)tmpcoords.x/(float)header.skinWidth;
		texcoords[i].val[1] = 1-(float)tmpcoords.y/(float)header.skinHeight;
		//printf("%f \t %f \n", texcoords[i].val[0], texcoords[i].val[1]);
	}
//Extract triangle indices
	triangles = new md2Triangles[header.numTriangles];
	fseek(md2file, header.offsetTriangles, SEEK_SET);
	for(int i=0; i<header.numTriangles; i++)
		fread(&triangles[i], 1, sizeof(md2Triangles), md2file);
//Extract frames and vertices
	byte buffer[2048*4+128];
	float a;
	frames = new md2Frames[header.numFrames];

	for(int i=0; i<header.numFrames; i++)
	{
		md2SingleFrame *frame = (md2SingleFrame *) buffer;
		frames[i].vertex = new md2Vertex[header.numVertex];
		fseek(md2file, header.offsetFrames+header.frameSize*i, SEEK_SET);
		fread(&a, 1, sizeof(float), md2file);
		frame->scale[0] = a;
		fread(&a, 1, sizeof(float), md2file);
		frame->scale[1] = a;
		fread(&a, 1, sizeof(float), md2file);
		frame->scale[2] = a;
		fread(&a, 1, sizeof(float), md2file);
		frame->translate[0] = a;
		fread(&a, 1, sizeof(float), md2file);
		frame->translate[1] = a;
		fread(&a, 1, sizeof(float), md2file);
		frame->translate[2] = a;
		fread(frame->name, sizeof(char), 16, md2file);
		strcpy(frames[i].name, frame->name);
		//printf("%s\n", frame->name);
		fread(frame->vertex, sizeof(md2TriangleVertex), header.numVertex, md2file);
		for(int j=0; j<header.numVertex; j++)
		{
			frames[i].vertex[j].val[0] = frame->vertex[j].val[0]*frame->scale[0] + frame->translate[0];
			frames[i].vertex[j].val[2] = frame->vertex[j].val[1]*frame->scale[1] + frame->translate[1];
			frames[i].vertex[j].val[1] = frame->vertex[j].val[2]*frame->scale[2] + frame->translate[2];
		}
	}

	fclose(md2file);
}

md2Model::~md2Model()
{
	delete [] frames;
	delete [] triangles;
	delete [] texcoords;
	delete [] skins;
}
