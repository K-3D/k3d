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
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>

#include <iterator>

namespace libk3dmesh
{

/////////////////////////////////////////////////////////////////////////////
// face_normals_implementation

class face_normals_implementation :
	public k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	face_normals_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_normalize(init_owner(*this) + init_name("normalize") + init_label(_("Normalize")) + init_description(_("Normalize normals")) + init_value(false)),
		m_scale(init_owner(*this) + init_name("normals_scale") + init_label(_("Normals scale")) + init_description(_("Scale normals")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_normalize.changed_signal().connect(make_reset_mesh_slot());
		m_scale.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		const bool normalize = m_normalize.value();
		const double scale = m_scale.value();

		// Create output geometry
		k3d::legacy::linear_curve_group* const group = new k3d::legacy::linear_curve_group();
		Mesh.linear_curve_groups.push_back(group);

		// Build face normals
		for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = InputMesh.polyhedra.begin(); polyhedron != InputMesh.polyhedra.end(); ++polyhedron)
		{
			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				k3d::legacy::split_edge* first = (*face)->first_edge;
				if(!first)
					// Skip empty faces
					continue;

				k3d::point3 v_sum = first->vertex->position;
				unsigned long n_vertices = 1;

				k3d::legacy::split_edge* current_edge = first->face_clockwise;
				while(current_edge)
				{
					v_sum += current_edge->vertex->position;
					++n_vertices;

					current_edge = current_edge->face_clockwise;
					if(current_edge == first)
						break;
				}

				// Get barycentric point and face normal
				v_sum /= static_cast<double>(n_vertices);
				k3d::normal3 normal = k3d::legacy::normal((*face)->first_edge);
				if(normalize)
					normal = k3d::normalize(normal);
				normal *= scale;

				// Create normal in output geometry
				k3d::legacy::linear_curve* const curve = new k3d::legacy::linear_curve();

				const k3d::point3 base = v_sum;
				const k3d::point3 tip = v_sum + k3d::to_vector(normal);

				k3d::legacy::point* const base_p = new k3d::legacy::point(base);
				k3d::legacy::point* const tip_p = new k3d::legacy::point(tip);

				Mesh.points.push_back(base_p);
				Mesh.points.push_back(tip_p);

				curve->control_points.push_back(base_p);
				curve->control_points.push_back(tip_p);

				group->curves.push_back(curve);
			}
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<face_normals_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xb5cd532b, 0x6ebd4848, 0x86a72136, 0xe25151eb),
				"FaceNormals",
				"Converts a mesh into a collection of linear curves constructed from its normal vectors",
				"Mesh",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_normalize;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_scale;
};

/////////////////////////////////////////////////////////////////////////////
// face_normals_factory

k3d::iplugin_factory& face_normals_factory()
{
	return face_normals_implementation::get_factory();
}

} // namespace libk3dmesh


