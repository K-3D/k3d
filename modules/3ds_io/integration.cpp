#include "integration.h"
#include "gprim_factory.h"
#include <k3dsdk/algebra.h>
#include <string.h>

namespace module
{
namespace f3ds
{
namespace io
{

	void add_nodes(k3d::gprim_factory &factory, Lib3dsFile *file, Lib3dsNode *node, int &offset)
	{
		k3d::matrix4 current = k3d::identity3D();

		if (node->type==LIB3DS_OBJECT_NODE)
		{
			for (Lib3dsNode *p=node->childs; p!=0; p=p->next)
				add_nodes(factory, file, p, offset);

			Lib3dsMesh *mesh;
			
			if (strcmp(node->name,"$$$DUMMY")==0) {
				return;
			}
			
			mesh = lib3ds_file_mesh_by_name(file, node->data.object.morph);
			if( mesh == NULL )
				mesh = lib3ds_file_mesh_by_name(file, node->name);

			ASSERT(mesh);
			if (!mesh)
				return;

			for (int p=0; p<mesh->faces; p++)
			{
				Lib3dsFace *f=&mesh->faceL[p];
				k3d::mesh::indices_t vertex_indices;
				for(int j=0; j<3; j++)
					vertex_indices.push_back(f->points[j]+offset);
				factory.add_polygon(vertex_indices);
			}
			for(int i=0; i< mesh->points; i++)
				factory.add_point(k3d::point4(mesh->pointL[i].pos[0], mesh->pointL[i].pos[1], mesh->pointL[i].pos[2], 1));
			offset += mesh->points;
		}
	}

	f3dsParser::f3dsParser(const char *filename, k3d::mesh &Mesh)
	{
		Lib3dsFile *file = lib3ds_file_load(filename);
		if (!file) 
		{
			k3d::log() << debug << "3DS file could not be loaded correctly" << std::endl;
			return;
		}

		/* No nodes?  Fabricate nodes to display all the meshes. */
		if( !file->nodes )
		{
			k3d::log() << error << "No nodes on 3ds file, creating virtual ones!" << std::endl;
			Lib3dsMesh *mesh;
			Lib3dsNode *node;
		
			for(mesh = file->meshes; mesh != NULL; mesh = mesh->next)
			{
			node = lib3ds_node_new_object();
			strcpy(node->name, mesh->name);
			node->parent_id = LIB3DS_NO_PARENT;
			lib3ds_file_insert_node(file, node);
			}
		}

		k3d::gprim_factory factory(Mesh);

		int offset = 0;
		for (Lib3dsNode *p=file->nodes; p!=0; p=p->next)
			add_nodes(factory, file, p, offset);
	}

	f3dsParser::f3dsParser(const char *filename, k3d::idocument &Document)
	{
	}

}
}
}