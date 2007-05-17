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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

namespace libk3dannotation
{

/////////////////////////////////////////////////////////////////////////////
// mesh_stats

class mesh_stats :
	public k3d::dev::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::dev::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	mesh_stats(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_point_count(init_owner(*this) + init_name("point_count") + init_label(_("Point count")) + init_description(_("Point count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_point_count))),
		m_polyhedron_count(init_owner(*this) + init_name("polyhedron_count") + init_label(_("Polyhedron Count")) + init_description(_("Polyhedron count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_polyhedron_count))),
		m_edge_count(init_owner(*this) + init_name("edge_count") + init_label(_("Edge count")) + init_description(_("Edge count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_edge_count))),
		m_face_count(init_owner(*this) + init_name("face_count") + init_label(_("Face count")) + init_description(_("Face count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_face_count))),
		m_linear_curve_group_count(init_owner(*this) + init_name("linear_curve_group_count") + init_label(_("Linear curve group count")) + init_description(_("Linear curve group count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_linear_curve_group_count))),
		m_linear_curve_count(init_owner(*this) + init_name("linear_curve_count") + init_label(_("Linear curve count")) + init_description(_("Linear curve count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_linear_curve_count))),
		m_cubic_curve_group_count(init_owner(*this) + init_name("cubic_curve_group_count") + init_label(_("Cubic curve group count")) + init_description(_("Cubic curve group count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_cubic_curve_group_count))),
		m_cubic_curve_count(init_owner(*this) + init_name("cubic_curve_count") + init_label(_("Cubic curve count")) + init_description(_("Cubic curve count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_cubic_curve_count))),
		m_nucurve_group_count(init_owner(*this) + init_name("nucurve_group_count") + init_label(_("Nucurve group count")) + init_description(_("Nucurve group count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_nucurve_group_count))),
		m_nucurve_count(init_owner(*this) + init_name("nucurve_count") + init_label(_("Nucurve count")) + init_description(_("Nucurve count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_nucurve_count))),
		m_bilinear_patch_count(init_owner(*this) + init_name("bilinear_patch_count") + init_label(_("Bilinear patch count")) + init_description(_("Bilinear patch count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_bilinear_patch_count))),
		m_bicubic_patch_count(init_owner(*this) + init_name("bicubic_patch_count") + init_label(_("Bicubic Patch Count")) + init_description(_("Bicubic Patch Count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_bicubic_patch_count))),
		m_nupatch_count(init_owner(*this) + init_name("nupatch_count") + init_label(_("Nupatch Count")) + init_description(_("Nupatch Count")) + init_slot(sigc::mem_fun(*this, &mesh_stats::get_nupatch_count)))
	{
		m_input_mesh.changed_signal().connect(m_point_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_polyhedron_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_edge_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_face_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_linear_curve_group_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_linear_curve_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_cubic_curve_group_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_cubic_curve_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_nucurve_group_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_nucurve_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_bilinear_patch_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_bicubic_patch_count.make_reset_slot());
		m_input_mesh.changed_signal().connect(m_nupatch_count.make_reset_slot());
	}

	void on_create_mesh(const k3d::dev::mesh& Input, k3d::dev::mesh& Output)
	{
		Output = Input;
	}

	void on_update_mesh(const k3d::dev::mesh& Input, k3d::dev::mesh& Output)
	{
	}

	unsigned long get_point_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->points)
				return mesh->points->size();
		}
	
		return 0;
	}

	unsigned long get_polyhedron_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->polyhedra && mesh->polyhedra->first_faces)
				return mesh->polyhedra->first_faces->size();
		}
	
		return 0;
	}

	unsigned long get_edge_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->polyhedra && mesh->polyhedra->edge_points)
				return mesh->polyhedra->edge_points->size();
		}

		return 0;
	}

	unsigned long get_face_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->polyhedra && mesh->polyhedra->face_first_loops)
				return mesh->polyhedra->face_first_loops->size();
		}

		return 0;
	}

	unsigned long get_linear_curve_group_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->linear_curve_groups && mesh->linear_curve_groups->first_curves)
				return mesh->linear_curve_groups->first_curves->size();
		}

		return 0;
	}

	unsigned long get_linear_curve_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->linear_curve_groups && mesh->linear_curve_groups->curve_first_points)
				return mesh->linear_curve_groups->curve_first_points->size();
		}
		
		return 0;
	}

	unsigned long get_cubic_curve_group_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->cubic_curve_groups && mesh->cubic_curve_groups->first_curves)
				return mesh->cubic_curve_groups->first_curves->size();
		}

		return 0;
	}

	unsigned long get_cubic_curve_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->cubic_curve_groups && mesh->cubic_curve_groups->curve_first_points)
				return mesh->cubic_curve_groups->curve_first_points->size();
		}
		
		return 0;
	}

	unsigned long get_nucurve_group_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->nurbs_curve_groups && mesh->nurbs_curve_groups->first_curves)
				return mesh->nurbs_curve_groups->first_curves->size();
		}

		return 0;
	}

	unsigned long get_nucurve_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->nurbs_curve_groups && mesh->nurbs_curve_groups->curve_first_points)
				return mesh->nurbs_curve_groups->curve_first_points->size();
		}

		return 0;
	}

	unsigned long get_bilinear_patch_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->bilinear_patches && mesh->bilinear_patches->patch_selection)
				return mesh->bilinear_patches->patch_selection->size();
		}

		return 0;
	}

	unsigned long get_bicubic_patch_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->bicubic_patches && mesh->bicubic_patches->patch_selection)
				return mesh->bicubic_patches->patch_selection->size();
		}

		return 0;
	}

	unsigned long get_nupatch_count()
	{
		if(k3d::dev::mesh* const mesh = m_input_mesh.value())
		{
			if(mesh->nurbs_patches && mesh->nurbs_patches->patch_first_points)
				return mesh->nurbs_patches->patch_first_points->size();
		}

		return 0;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_stats,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x5e9aae8e, 0xc73345c1, 0x932bfe88, 0xb912bbf1),
				"MeshStats",
				_("Displays statistics about the input mesh"),
				"Utility",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_point_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_polyhedron_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_edge_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_face_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_linear_curve_group_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_linear_curve_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_cubic_curve_group_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_cubic_curve_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_nucurve_group_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_nucurve_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_bilinear_patch_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_bicubic_patch_count;
	k3d_data(unsigned long, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_nupatch_count;
};

/////////////////////////////////////////////////////////////////////////////
// mesh_stats_factory

k3d::iplugin_factory& mesh_stats_factory()
{
	return mesh_stats::get_factory();
}

} // namespace libk3dannotation

