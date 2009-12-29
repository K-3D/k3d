// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/bicubic_patch.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace bicubic_patch
{

/////////////////////////////////////////////////////////////////////////////
// polygonize_bicubic_patches

class polygonize_bicubic_patches :
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;

public:
	polygonize_bicubic_patches(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_subdivisions(init_owner(*this) + init_name("subdivisions") + init_label(_("Subdivisions")) + init_description(_("Patch subdivision number")) + init_value(3) + init_step_increment(1) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_subdivisions.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = k3d::mesh();
		Output.points = Input.points;
		Output.point_selection = Input.point_selection;
		Output.point_attributes = Input.point_attributes;

		const k3d::int32_t subdivisions = m_subdivisions.pipeline_value();

		for(k3d::mesh::primitives_t::const_iterator primitive = Input.primitives.begin(); primitive != Input.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::bicubic_patch::const_primitive> bicubic_patch(k3d::bicubic_patch::validate(Output, **primitive));
			if(!bicubic_patch)
			{
				Output.primitives.push_back(*primitive);
				continue;
			}

			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));

			k3d::mesh::points_t& points = Output.points.writable();
			k3d::mesh::selection_t& point_selection = Output.point_selection.writable();

			const k3d::uint_t patch_begin = 0;
			const k3d::uint_t patch_end = patch_begin + bicubic_patch->patch_selections.size();
			for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
			{
				const k3d::uint_t point_offset = points.size();

				for(k3d::int32_t j = 0; j <= subdivisions; ++j)
				{
					const k3d::double_t u = static_cast<k3d::double_t>(j) / static_cast<k3d::double_t>(subdivisions);
					for(k3d::int32_t k = 0; k <= subdivisions; ++k)
					{
						const k3d::double_t v = static_cast<k3d::double_t>(k) / static_cast<k3d::double_t>(subdivisions);

						k3d::double_t U[4], V[4];
						U[0] = (1-u)*(1-u)*(1-u);
						U[1] = 3*u*(1-u)*(1-u);
						U[2] = 3*u*u*(1-u);
						U[3] = u*u*u;
						V[0] = (1-v)*(1-v)*(1-v);
						V[1] = 3*v*(1-v)*(1-v);
						V[2] = 3*v*v*(1-v);
						V[3] = v*v*v;

						k3d::point3 new_point(0, 0, 0);
						for(unsigned long l = 0; l < 4; ++l)
						{
							for(unsigned long m = 0; m < 4; ++m)
							{
								const k3d::point3 position = points[bicubic_patch->patch_points[(patch * 16) + (l * 4) + m]];
								new_point += k3d::to_vector(position * U[l] * V[m]);
							}
						}

						points.push_back(new_point);
						point_selection.push_back(1);
					}
				}

				for(k3d::int32_t j = 0; j != subdivisions; ++j)
				{
					for(k3d::int32_t k = 0; k != subdivisions; ++k)
					{
						polyhedron->face_shells.push_back(0);
						polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
						polyhedron->face_loop_counts.push_back(1);
						polyhedron->face_selections.push_back(1);
						polyhedron->face_materials.push_back(bicubic_patch->patch_materials[patch]);
						
						polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

						polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
						polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
						polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
						polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() - 3);

						polyhedron->edge_selections.push_back(1);
						polyhedron->edge_selections.push_back(1);
						polyhedron->edge_selections.push_back(1);
						polyhedron->edge_selections.push_back(1);

						polyhedron->vertex_points.push_back(point_offset + (j+0) * (subdivisions+1) + (k+0));
						polyhedron->vertex_points.push_back(point_offset + (j+0) * (subdivisions+1) + (k+1));
						polyhedron->vertex_points.push_back(point_offset + (j+1) * (subdivisions+1) + (k+1));
						polyhedron->vertex_points.push_back(point_offset + (j+1) * (subdivisions+1) + (k+0));

						polyhedron->vertex_selections.push_back(1);
						polyhedron->vertex_selections.push_back(1);
						polyhedron->vertex_selections.push_back(1);
						polyhedron->vertex_selections.push_back(1);
					}
				}
			}

			polyhedron->shell_types.push_back(k3d::polyhedron::POLYGONS);
		}

		k3d::mesh::delete_unused_points(Output);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
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
				"Patch",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_subdivisions;
};

/////////////////////////////////////////////////////////////////////////////
// polygonize_bicubic_patches_factory

k3d::iplugin_factory& polygonize_bicubic_patches_factory()
{
	return polygonize_bicubic_patches::get_factory();
}

} // namespace bicubic_patch

} // namespace module



