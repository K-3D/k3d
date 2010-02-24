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
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace bicubic_patch
{

/////////////////////////////////////////////////////////////////////////////
// bicubic_to_poly

class bicubic_to_poly :
	public k3d::mesh_modifier<k3d::node >
{
	typedef k3d::mesh_modifier<k3d::node > base;

public:
	bicubic_to_poly(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_subdivisions(init_owner(*this) + init_name("subdivisions") + init_label(_("Subdivisions")) + init_description(_("Patch subdivision number")) + init_value(3) + init_step_increment(1) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_subdivisions.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		// Copy point data only ...
		Output = k3d::mesh();
		Output.points = Input.points;
		Output.point_selection = Input.point_selection;
		Output.point_attributes = Input.point_attributes;

		const k3d::int32_t subdivisions = m_subdivisions.pipeline_value();

		// For each primitive ...
		for(k3d::mesh::primitives_t::const_iterator primitive = Input.primitives.begin(); primitive != Input.primitives.end(); ++primitive)
		{
			// Convert bicubic patch primitives to polyhedron primitives, passing-through all other primitive types ...
			boost::scoped_ptr<k3d::bicubic_patch::const_primitive> bicubic_patch(k3d::bicubic_patch::validate(Output, **primitive));
			if(!bicubic_patch)
			{
				Output.primitives.push_back(*primitive);
				continue;
			}

			// Create a new polyhedron and prepare to copy attributes ...
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));
			polyhedron->face_attributes = bicubic_patch->patch_attributes.clone_types();
			k3d::table_copier face_attributes(bicubic_patch->patch_attributes, polyhedron->face_attributes);

			// Prepare to create new points ...
			k3d::mesh::points_t& points = Output.points.writable();
			k3d::mesh::selection_t& point_selection = Output.point_selection.writable();
			k3d::table_copier point_attributes(Output.point_attributes);

			const k3d::uint_t patch_begin = 0;
			const k3d::uint_t patch_end = patch_begin + bicubic_patch->patch_selections.size();
			for(k3d::uint_t patch = patch_begin; patch != patch_end; ++patch)
			{
				const k3d::uint_t point_offset = points.size();

				for(k3d::int32_t j = 0; j <= subdivisions; ++j)
				{
					const k3d::double_t u = k3d::ratio(j, subdivisions);
					for(k3d::int32_t k = 0; k <= subdivisions; ++k)
					{
						const k3d::double_t v = k3d::ratio(k, subdivisions);

						k3d::double_t U[4], V[4];
						U[0] = (1-u)*(1-u)*(1-u);
						U[1] = 3*u*(1-u)*(1-u);
						U[2] = 3*u*u*(1-u);
						U[3] = u*u*u;
						V[0] = (1-v)*(1-v)*(1-v);
						V[1] = 3*v*(1-v)*(1-v);
						V[2] = 3*v*v*(1-v);
						V[3] = v*v*v;

						k3d::mesh::indices_t point_indices;
						k3d::mesh::weights_t point_weights;
						for(k3d::int32_t l = 0; l < 4; ++l)
						{
							for(k3d::int32_t m = 0; m < 4; ++m)
							{
								point_indices.push_back(bicubic_patch->patch_points[(patch * 16) + (l * 4) + m]);
								point_weights.push_back(U[l] * V[m]);
							}
						}
						k3d::point3 new_point(0, 0, 0);
						for(k3d::uint_t i = 0; i != point_indices.size(); ++i)
							new_point += k3d::to_vector(point_weights[i] * points[point_indices[i]]);

						points.push_back(new_point);
						point_selection.push_back(1);
						point_attributes.push_back(point_indices.size(), &point_indices[0], &point_weights[0]);
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
						face_attributes.push_back(patch);
						
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

		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bicubic_to_poly,
				k3d::interface_list<k3d::imesh_source,
				k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xbd6bdff8, 0xb4374eb1, 0xad39f4ad, 0x87209e59),
				"BicubicToPoly",
				_("Polygonizes bicubic patches"),
				"BicubicPatch",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_subdivisions;
};

/////////////////////////////////////////////////////////////////////////////
// bicubic_to_poly_factory

k3d::iplugin_factory& bicubic_to_poly_factory()
{
	return bicubic_to_poly::get_factory();
}

} // namespace bicubic_patch

} // namespace module



