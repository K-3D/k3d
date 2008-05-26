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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/surface_polygonizer/blobby_polygonizer.h>
#include <k3dsdk/types_ri.h>

namespace module
{

namespace blobby
{

/////////////////////////////////////////////////////////////////////////////
// polygonize_blobbies_implementation

class polygonize_blobbies_implementation :
	public k3d::material_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	polygonize_blobbies_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_voxels(init_owner(*this) + init_name("voxels") + init_label(_("Voxels")) + init_description(_("Voxel subdivisions along blobby's bounding-box width, height and depth ")) + init_value(30) + init_step_increment(1) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_voxels.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		const unsigned long voxels = m_voxels.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		// Polygonize each blobby ...
		for(k3d::legacy::mesh::blobbies_t::const_iterator blobby = InputMesh.blobbies.begin(); blobby != InputMesh.blobbies.end(); blobby++)
		{
			Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
			k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

			// Polygonize blobby
			typedef std::vector<k3d::point3> vertices_t;
			vertices_t blobby_vertices;
			vertices_t blobby_normals;
			typedef std::vector< std::vector<unsigned long> > polygons_t;
			polygons_t blobby_polygons;
			polygonize_blobby(*blobby, voxels, blobby_vertices, blobby_normals, blobby_polygons);

			// Add geometry to the mesh ...
			vertices_t::const_iterator vertex = blobby_vertices.begin();
			vertices_t::const_iterator normal = blobby_normals.begin();
			vertices_t::const_iterator vertices_end = blobby_vertices.end();
			for(; vertex != vertices_end; vertex++, normal++)
			{
				k3d::legacy::point* point = new k3d::legacy::point(*vertex);
				point->vertex_data["N"] = k3d::ri::normal(k3d::to_normal(-(*normal)));
				Mesh.points.push_back(point);
			}

			for(unsigned long i = 0; i != blobby_polygons.size(); ++i)
			{
				if(!blobby_polygons[i].size())
					continue;

				std::vector<k3d::legacy::split_edge*> edges;
				for(unsigned long j = 0; j != blobby_polygons[i].size(); ++j)
					edges.push_back(new k3d::legacy::split_edge(Mesh.points[blobby_polygons[i][j]]));
				for(unsigned long j = 0; j != blobby_polygons[i].size(); ++j)
					edges[j]->face_clockwise = edges[(j+1) % blobby_polygons[i].size()];

				polyhedron.faces.push_back(new k3d::legacy::face(edges.front(), material));
			}

			// Set companions and check solidness
			k3d::legacy::set_companions(polyhedron);
			assert_warning(k3d::legacy::is_solid(polyhedron));
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<polygonize_blobbies_implementation,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x446d652b, 0x938042d6, 0x9f006d66, 0x3cbff023),
				"PolygonizeBlobbies",
				"Polygonizes blobby implicit surfaces",
				"Blobby",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_voxels;
};

/////////////////////////////////////////////////////////////////////////////
// polygonize_blobbies_factory

k3d::iplugin_factory& polygonize_blobbies_factory()
{
	return polygonize_blobbies_implementation::get_factory();
}

} // namespace blobby

} // namespace module

