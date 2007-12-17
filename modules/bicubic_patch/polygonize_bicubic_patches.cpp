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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace module
{

namespace bicubic_patch
{

/////////////////////////////////////////////////////////////////////////////
// polygonize_bicubic_patches

class polygonize_bicubic_patches :
	public k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	polygonize_bicubic_patches(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_subdivisions(init_owner(*this) + init_name("subdivisions") + init_label(_("Subdivisions")) + init_description(_("Patch subdivision number")) + init_value(3) + init_step_increment(1) + init_constraint(constraint::minimum(1L)) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_subdivisions.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		const unsigned long subdivisions = m_subdivisions.pipeline_value();

		// Polygonize each bicubic patch ...
		for(k3d::legacy::mesh::bicubic_patches_t::const_iterator patch = InputMesh.bicubic_patches.begin(); patch != InputMesh.bicubic_patches.end(); patch++)
		{
			Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
			k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();

			// Polygonize patch
			std::vector<k3d::legacy::point*> points;
			for(unsigned long j = 0; j <= subdivisions; j++)
			{
				const double u = static_cast<double>(j) / static_cast<double>(subdivisions);
				for(unsigned long k = 0; k <= subdivisions; k++)
				{
					const double v = static_cast<double>(k) / static_cast<double>(subdivisions);
					double U[4], V[4];
					U[0] = (1-u)*(1-u)*(1-u);
					U[1] = 3*u*(1-u)*(1-u);
					U[2] = 3*u*u*(1-u);
					U[3] = u*u*u;
					V[0] = (1-v)*(1-v)*(1-v);
					V[1] = 3*v*(1-v)*(1-v);
					V[2] = 3*v*v*(1-v);
					V[3] = v*v*v;

					k3d::point3 p(0, 0, 0);
					for(unsigned long l = 0; l < 4; l++)
					{
						for(unsigned long m = 0; m < 4; m++)
						{
							const k3d::point3 position = (*patch)->control_points[l*4+m]->position;
							p += k3d::to_vector(position * U[l] * V[m]);
						}
					}

					k3d::legacy::point* const newpoint = new k3d::legacy::point(p);
					return_if_fail(newpoint);

					points.push_back(newpoint);
					Mesh.points.push_back(newpoint);
				}
			}

			// Save quads
			for(unsigned long j = 0; j < subdivisions; j++)
			{
				for(unsigned long k = 0; k < subdivisions; k++)
				{
					std::vector<k3d::legacy::split_edge*> edges;
					edges.push_back(new k3d::legacy::split_edge(points[j*(subdivisions+1)+k]));
					edges.push_back(new k3d::legacy::split_edge(points[j*(subdivisions+1)+k+1]));
					edges.push_back(new k3d::legacy::split_edge(points[(j+1)*(subdivisions+1)+k+1]));
					edges.push_back(new k3d::legacy::split_edge(points[(j+1)*(subdivisions+1)+k]));

					k3d::legacy::loop_edges(edges.begin(), edges.end());
					//for(unsigned long j = 0; j < 4; ++j)
					//	edges[j]->face_clockwise = edges[(j+1) % blobby_polygons[i].size()];

					polyhedron.faces.push_back(new k3d::legacy::face(edges.front(), (*patch)->material));
				}
			}

			assert_warning(is_valid(polyhedron));

			// Set companions
			k3d::legacy::set_companions(polyhedron);
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<polygonize_bicubic_patches,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xbd6bdff8, 0xb4374eb1, 0xad39f4ad, 0x87209e59),
				"PolygonizeBicubicPatches",
				_("Polygonizes bicubic patches"),
				"Patches",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_subdivisions;
};

/////////////////////////////////////////////////////////////////////////////
// polygonize_bicubic_patches_factory

k3d::iplugin_factory& polygonize_bicubic_patches_factory()
{
	return polygonize_bicubic_patches::get_factory();
}

} // namespace bicubic_patch

} // namespace module



