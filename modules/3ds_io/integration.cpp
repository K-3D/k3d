#include "integration.h"
#include <k3dsdk/gprim_factory.h>
#include <k3dsdk/algebra.h>
#include <string.h>

namespace module
{
namespace f3ds
{
namespace io
{

	void add_nodes(k3d::gprim_factory &factory, Lib3dsFile *file, Lib3dsNode *node, k3d::matrix4 mat)
	{
		k3d::matrix4 current = k3d::identity3D();

		for (Lib3dsNode *p=node->childs; p!=0; p=p->next)
			add_nodes(factory, file, p, mat*current);
		
		if (node->type==LIB3DS_OBJECT_NODE)
		{
		
			Lib3dsMesh *mesh = lib3ds_file_mesh_by_name(file, node->data.object.morph);
			if( mesh == NULL )
				mesh = lib3ds_file_mesh_by_name(file, node->name);


			Lib3dsMatrix M;
			lib3ds_matrix_copy(M, mesh->matrix);
			lib3ds_matrix_inv(M);

			for(int i=0; i<4; i++)
				for(int j=0; j<4; j++)
					current[i][j] = M[i][j];

			current = mat * current;

			for (int p=0; p<mesh->faces; ++p)
			{
				Lib3dsFace *f=&mesh->faceL[p];
				k3d::mesh::indices_t vertex_indices;
				for(int j=0; j<3; j++)
					vertex_indices.push_back(f->points[j]);
				factory.add_polygon(vertex_indices);
			}

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

		for (Lib3dsNode *p=file->nodes; p!=0; p=p->next)
		{
			//add_nodes(factory, file, p, k3d::identity3D());
		}

		for(Lib3dsMesh *mesh = file->meshes; mesh != NULL; mesh = mesh->next)
		{
			k3d::log() << debug << "un mesh" << std::endl;

			k3d::matrix4 current = k3d::identity3D();

			Lib3dsMatrix M;
			lib3ds_matrix_copy(M, mesh->matrix);
			lib3ds_matrix_inv(M);

			for(int i=0; i<4; i++)
				for(int j=0; j<4; j++)
					current[i][j] = M[i][j];


			for(int i=0; i< mesh->points; i++)
				factory.add_point(k3d::point4(mesh->pointL[i].pos[0], mesh->pointL[i].pos[1], mesh->pointL[i].pos[2], 1));

			for(int i=0; i< mesh->faces; ++i)
			{
				k3d::mesh::indices_t vertex_indices;
				for(int j=0; j<3; j++)
					vertex_indices.push_back(mesh->faceL[i].points[j]);
				factory.add_polygon(vertex_indices);
			}
		//node = lib3ds_node_new_object();
		//strcpy(node->name, mesh->name);
		//node->parent_id = LIB3DS_NO_PARENT;
		//lib3ds_file_insert_node(file, node);
		}

	}

	f3dsParser::f3dsParser(const char *filename, k3d::idocument &Document)
	{
	}

}
}
}