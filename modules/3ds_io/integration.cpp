#include "integration.h"

#include <k3dsdk/algebra.h>

#include <lib3ds/file.h>
#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

namespace module
{

namespace f3ds
{

namespace io
{

static void add_nodes(k3d::mesh& Mesh, Lib3dsFile* file, Lib3dsNode* node, k3d::uint_t &offset)
{
	if(node->type == LIB3DS_OBJECT_NODE)
	{
		for(Lib3dsNode* p = node->childs; p != 0; p = p->next)
			add_nodes(Mesh, file, p, offset);

		if(strcmp(node->name,"$$$DUMMY") == 0)
			return;
		
		Lib3dsMesh* mesh = lib3ds_file_mesh_by_name(file, node->data.object.morph);
		if(!mesh)
			mesh = lib3ds_file_mesh_by_name(file, node->name);

		if(!mesh)
			return;

		k3d::mesh::polyhedra_t& polyhedra = Mesh.polyhedra.writable();
		k3d::mesh::indices_t& first_faces = polyhedra.first_faces.writable();
		k3d::mesh::counts_t& face_counts = polyhedra.face_counts.writable();
		k3d::mesh::polyhedra_t::types_t& types = polyhedra.types.writable();
		k3d::mesh::indices_t& face_first_loops = polyhedra.face_first_loops.writable();
		k3d::mesh::counts_t& face_loop_counts = polyhedra.face_loop_counts.writable();
		k3d::mesh::selection_t& face_selection = polyhedra.face_selection.writable();
		k3d::mesh::materials_t& face_materials = polyhedra.face_materials.writable();
		k3d::mesh::indices_t& loop_first_edges = polyhedra.loop_first_edges.writable();
		k3d::mesh::indices_t& edge_points = polyhedra.edge_points.writable();
		k3d::mesh::indices_t& clockwise_edges = polyhedra.clockwise_edges.writable();
		k3d::mesh::selection_t& edge_selection = polyhedra.edge_selection.writable();
		for(int p = 0; p < mesh->faces; ++p)
		{
			Lib3dsFace* const f = &mesh->faceL[p];

			face_first_loops.push_back(loop_first_edges.size());
			face_loop_counts.push_back(1);
			face_selection.push_back(0);
			face_materials.push_back(0);
			loop_first_edges.push_back(edge_points.size());

			for(int j = 0; j != 3; ++j)
			{
				edge_points.push_back(f->points[j] + offset);
				clockwise_edges.push_back(edge_points.size());
				edge_selection.push_back(0);
			}
			clockwise_edges.back() = loop_first_edges.back();
		}

		k3d::mesh::points_t& points = Mesh.points.writable();
		k3d::mesh::selection_t& point_selection = Mesh.point_selection.writable();
		for(int i = 0; i < mesh->points; ++i)
		{
			points.push_back(k3d::point3(mesh->pointL[i].pos[0], mesh->pointL[i].pos[1], mesh->pointL[i].pos[2]));
			point_selection.push_back(0);
		}

		offset += mesh->points;
	}
}

f3dsParser::f3dsParser(const char *filename, k3d::mesh &Mesh)
{
	Lib3dsFile* const file = lib3ds_file_load(filename);
	if(!file) 
	{
		k3d::log() << debug << "3DS file could not be loaded correctly" << std::endl;
		return;
	}

	Mesh.points.create();
	Mesh.point_selection.create();

	k3d::mesh::polyhedra_t& polyhedra = Mesh.polyhedra.create();
	k3d::mesh::indices_t& first_faces = polyhedra.first_faces.create();
	k3d::mesh::counts_t& face_counts = polyhedra.face_counts.create();
	k3d::mesh::polyhedra_t::types_t& types = polyhedra.types.create();
	k3d::mesh::indices_t& face_first_loops = polyhedra.face_first_loops.create();
	k3d::mesh::counts_t& face_loop_counts = polyhedra.face_loop_counts.create();
	k3d::mesh::selection_t& face_selection = polyhedra.face_selection.create();
	k3d::mesh::materials_t& face_materials = polyhedra.face_materials.create();
	k3d::mesh::indices_t& loop_first_edges = polyhedra.loop_first_edges.create();
	k3d::mesh::indices_t& edge_points = polyhedra.edge_points.create();
	k3d::mesh::indices_t& clockwise_edges = polyhedra.clockwise_edges.create();
	k3d::mesh::selection_t& edge_selection = polyhedra.edge_selection.create();

	/* No nodes?  Fabricate nodes to display all the meshes. */
	if(!file->nodes)
	{
		k3d::log() << error << "3ds file doesn't contain any nodes, creating virtual ones" << std::endl;
	
		for(Lib3dsMesh* mesh = file->meshes; mesh; mesh = mesh->next)
		{
			Lib3dsNode* const node = lib3ds_node_new_object();
			strcpy(node->name, mesh->name);
			node->parent_id = LIB3DS_NO_PARENT;
			lib3ds_file_insert_node(file, node);
		}
	}

	k3d::uint_t offset = 0;
	for(Lib3dsNode* p = file->nodes; p; p = p->next)
		add_nodes(Mesh, file, p, offset);

	if(face_first_loops.size())
	{
		first_faces.push_back(0);
		face_counts.push_back(face_first_loops.size());
		types.push_back(k3d::mesh::polyhedra_t::POLYGONS);
	}
}

} // namespace io

} // namespace f3ds

} // namespace module

