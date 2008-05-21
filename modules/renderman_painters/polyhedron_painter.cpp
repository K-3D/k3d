// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "array_helpers.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/node.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include <set>

namespace libk3drendermanpainters
{

/////////////////////////////////////////////////////////////////////////////
// polyhedron_painter

class polyhedron_painter :
	public k3d::node,
	public k3d::ri::imesh_painter
{
	typedef k3d::node base;

public:
	polyhedron_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		if(!k3d::validate_polyhedra(Mesh))
			return;

		const k3d::mesh::indices_t& first_faces = *Mesh.polyhedra->first_faces;
		const k3d::mesh::counts_t& face_counts = *Mesh.polyhedra->face_counts;
		const k3d::mesh::polyhedra_t::types_t& types = *Mesh.polyhedra->types;
		const k3d::mesh::named_arrays& constant_data = Mesh.polyhedra->constant_data;
		const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = *Mesh.polyhedra->face_loop_counts;
		const k3d::mesh::materials_t& face_materials = *Mesh.polyhedra->face_materials;
		const k3d::mesh::named_arrays& uniform_data = Mesh.polyhedra->uniform_data; 
		const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
		const k3d::mesh::named_arrays& face_varying_data = Mesh.polyhedra->face_varying_data;
		const k3d::mesh::points_t& points = *Mesh.points;
		const k3d::mesh::named_arrays& vertex_data = Mesh.vertex_data;

		const size_t polyhedron_begin = 0;
		const size_t polyhedron_end = polyhedron_begin + types.size();
		for(size_t polyhedron = polyhedron_begin; polyhedron != polyhedron_end; ++polyhedron)
		{
			if(types[polyhedron] != k3d::mesh::polyhedra_t::POLYGONS)
				continue;

			// Get the set of all materials used in this polyhedron ...
			typedef std::set<k3d::imaterial*> materials_t;
			materials_t materials;

			const size_t faces_begin = first_faces[polyhedron];
			const size_t faces_end = faces_begin + face_counts[polyhedron];
			for(size_t face = faces_begin; face != faces_end; ++face)
				materials.insert(face_materials[face]);

			// Iterate over each material, rendering all the faces that use that material in a single pass
			for(materials_t::iterator m = materials.begin(); m != materials.end(); ++m)
			{
				k3d::imaterial* const material = *m;

				k3d::ri::unsigned_integers loop_counts;
				k3d::ri::unsigned_integers vertex_counts;
				k3d::ri::unsigned_integers vertex_ids;

				array_copier ri_constant_data;
				ri_constant_data.add_arrays(constant_data);

				array_copier ri_uniform_data;
				ri_uniform_data.add_arrays(uniform_data);

				array_copier ri_facevarying_data;
				ri_facevarying_data.add_arrays(face_varying_data);

				array_copier ri_vertex_data;
				ri_vertex_data.add_arrays(vertex_data);
				ri_vertex_data.add_array(k3d::ri::RI_P(), points);

				const size_t faces_begin = first_faces[polyhedron];
				const size_t faces_end = faces_begin + face_counts[polyhedron];
				for(size_t face = faces_begin; face != faces_end; ++face)
				{
					if(face_materials[face] != material)
						continue;

					loop_counts.push_back(face_loop_counts[face]);

					ri_uniform_data.push_back(face);

					const size_t loop_begin = face_first_loops[face];
					const size_t loop_end = loop_begin + face_loop_counts[face];
					for(size_t loop = loop_begin; loop != loop_end; ++loop)
					{
						size_t vertex_count = 0;

						const size_t first_edge = loop_first_edges[loop];
						for(size_t edge = first_edge; ; )
						{
							ri_facevarying_data.push_back(edge);

							++vertex_count;
							vertex_ids.push_back(edge_points[edge]);

							edge = clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}

						vertex_counts.push_back(vertex_count);
					}
				}

				ri_constant_data.push_back(polyhedron);

				ri_vertex_data.insert(0, points.size());

				k3d::ri::parameter_list ri_parameters;
				ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);
				ri_uniform_data.copy_to(k3d::ri::UNIFORM, ri_parameters);
				ri_facevarying_data.copy_to(k3d::ri::FACEVARYING, ri_parameters);
				ri_vertex_data.copy_to(k3d::ri::VERTEX, ri_parameters);

				k3d::ri::setup_material(material, RenderState);
				RenderState.stream.RiPointsGeneralPolygonsV(loop_counts, vertex_counts, vertex_ids, ri_parameters);
			}
		}
	}

	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<polyhedron_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0x0771ea79, 0x6c8c4c6d, 0x8bab8c83, 0x42fc73e9),
			"RenderManPolyhedronPainter",
			_("Renders mesh polyhedra"),
			"RenderMan Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// polyhedron_painter_factory

k3d::iplugin_factory& polyhedron_painter_factory()
{
	return polyhedron_painter::get_factory();
}

} // namespace libk3drendermanpainters

