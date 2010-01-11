// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
	\author Carsten Haubold (CarstenHaubold@web.de)
*/

#include "nurbs_patches.h"
#include "utility.h"

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/log.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <vector>

namespace module
{

namespace nurbs
{
class polygonize_patch :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	polygonize_patch(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
			base(Factory, Document),
			m_u_samples(init_owner(*this) + init_name(_("u_samples")) + init_label(_("U Samples")) + init_description(_("Samples per knot interval in the U direction. More is better")) + init_value(5) + init_constraint(constraint::minimum(1))),
			m_v_samples(init_owner(*this) + init_name(_("v_samples")) + init_label(_("V Samples")) + init_description(_("Samples per knot interval in the V direction. More is better")) + init_value(5) + init_constraint(constraint::minimum(1))),
			m_delete_orig(init_owner(*this) + init_name(_("delete_orig")) + init_label(_("Delete original?")) + init_description(_("Delete original NURBS curve?")) + init_value(true))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_u_samples.changed_signal().connect(make_update_mesh_slot());
		m_v_samples.changed_signal().connect(make_update_mesh_slot());
		m_delete_orig.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		k3d::mesh temp = Input;
		if(!temp.points.get())
			return;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), temp);
		Output = temp;
		Output.primitives.clear();

		const k3d::uint_t u_samples = m_u_samples.pipeline_value();
		const k3d::uint_t v_samples = m_v_samples.pipeline_value();

		const k3d::uint_t prim_count = temp.primitives.size();
		for(k3d::uint_t prim_idx = 0; prim_idx != prim_count; ++prim_idx)
		{
			boost::scoped_ptr<k3d::nurbs_patch::const_primitive> patches(k3d::nurbs_patch::validate(temp, *temp.primitives[prim_idx]));
			if(patches)
			{
				boost::scoped_ptr<k3d::nurbs_patch::primitive> output_patches(k3d::nurbs_patch::create(Output));
				k3d::mesh::points_t vertices;
				k3d::mesh::counts_t vertex_counts;
				k3d::mesh::indices_t vertex_indices;
				for(k3d::uint_t patch = 0; patch != patches->patch_first_points.size(); ++patch)
				{
					// Copy existing patches, if required
					if(!patches->patch_selections[patch] || !m_delete_orig.pipeline_value())
						add_patch(Output, *output_patches, temp, *patches, patch);
					if(patches->patch_selections[patch])
					{
						polygonize(vertices, vertex_counts, vertex_indices, temp, *patches, patch, u_samples, v_samples);
					}
				}
				if(!vertices.empty())
				{
					return_if_fail(patches->patch_materials.size());
					boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output, vertices, vertex_counts, vertex_indices, patches->patch_materials.front()));
				}
			}
			else
			{
				Output.primitives.push_back(temp.primitives[prim_idx]);
			}
		}
		delete_empty_primitives(Output);

		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<polygonize_patch, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0x84f89545, 0x2746be1b, 0x045df3b0, 0x539eee95),
		  "NurbsPolygonizePatch",
		  _("Creates a Polyhedron representation of the selected NURBS patch"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_u_samples;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_v_samples;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_delete_orig;
};

k3d::iplugin_factory& polygonize_patch_factory()
{
	return polygonize_patch::get_factory();
}

} //namespace nurbs

} //namespace module

