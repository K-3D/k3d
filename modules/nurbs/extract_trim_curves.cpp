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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
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
class extract_trim_curves :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
public:
	extract_trim_curves(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
			base(Factory, Document),
			m_samples(init_owner(*this) + init_name(_("samples")) + init_label(_("Samples")) + init_description(_("Samples per knot interval of the trim curves. More is better")) + init_value(5) + init_constraint(constraint::minimum(1)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_samples.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		if(!Output.points.get())
			return;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		const k3d::uint_t prim_count = Input.primitives.size();
		for(k3d::uint_t prim_idx = 0; prim_idx != prim_count; ++prim_idx)
		{
			boost::scoped_ptr<k3d::nurbs_patch::const_primitive> patches(k3d::nurbs_patch::validate(Output, *Output.primitives[prim_idx]));
			if(patches)
			{
				boost::scoped_ptr<k3d::nurbs_curve::primitive> output_curves;
				for(k3d::uint_t patch = 0; patch != patches->patch_first_points.size(); ++patch)
				{
					if(patches->patch_selections[patch] && patches->patch_trim_loop_counts[patch])
					{
						if(!output_curves)
							output_curves.reset(k3d::nurbs_curve::create(Output));
						trim_to_nurbs(Output, *output_curves, Output, *patches, patch, m_samples.pipeline_value());
					}
				}
			}
		}

		delete_empty_primitives(Output);

		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<extract_trim_curves, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
		  k3d::uuid(0xb8facce3, 0x424bf565, 0xc4d5d5b0, 0x727fe955),
		  "NurbsExtractTrimCurves",
		  _("Extracts patch trim curves as 3D NURBS curve approximations"),
		  "NURBS",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_samples;
};

k3d::iplugin_factory& extract_trim_curves_factory()
{
	return extract_trim_curves::get_factory();
}

} //namespace nurbs

} //namespace module

