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

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>

#include <iterator>

namespace libk3dmesh
{

/////////////////////////////////////////////////////////////////////////////
// tag_color_implementation

class tag_color_implementation :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	tag_color_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_new_color(init_owner(*this) + init_name("new_color") + init_label(_("New color")) + init_description(_("New Color")) + init_value(k3d::color(1, 0, 0))),
		m_default_color(init_owner(*this) + init_name("default_color") + init_label(_("Default color")) + init_description(_("Default Color")) + init_value(k3d::color(1, 1, 1))),
		m_tag_points(init_owner(*this) + init_name("tag_points") + init_label(_("Tag points")) + init_description(_("Tag Points")) + init_value(false)),
		m_tag_point_groups(init_owner(*this) + init_name("tag_point_groups") + init_label(_("Tag point groups")) + init_description(_("Tag Point Groups")) + init_value(false)),
		m_tag_edges(init_owner(*this) + init_name("tag_edges") + init_label(_("Tag edges")) + init_description(_("Tag Edges")) + init_value(false)),
		m_tag_faces(init_owner(*this) + init_name("tag_faces") + init_label(_("Tag faces")) + init_description(_("Tag Faces")) + init_value(false)),
		m_tag_polyhedra(init_owner(*this) + init_name("tag_polyhedra") + init_label(_("Tag polyhedra")) + init_description(_("Tag Polyhedra")) + init_value(false)),
		m_tag_linear_curve_groups(init_owner(*this) + init_name("tag_linear_curve_groups") + init_label(_("Tag linear curve groups")) + init_description(_("Tag Linear Groups")) + init_value(false)),
		m_tag_cubic_curve_groups(init_owner(*this) + init_name("tag_cubic_curve_groups") + init_label(_("Tag cubic curve groups")) + init_description(_("Tag Cubic Curve Groups")) + init_value(false)),
		m_tag_bilinear_patches(init_owner(*this) + init_name("tag_bilinear_patches") + init_label(_("Tag bilinear patches")) + init_description(_("Tag Bilinear Patches")) + init_value(false)),
		m_tag_bicubic_patches(init_owner(*this) + init_name("tag_bicubic_patches") + init_label(_("Tag bicubic patches")) + init_description(_("Tag Bicubic Patches")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());

		m_new_color.changed_signal().connect(make_update_mesh_slot());
		m_default_color.changed_signal().connect(make_update_mesh_slot());
		m_tag_points.changed_signal().connect(make_update_mesh_slot());
		m_tag_point_groups.changed_signal().connect(make_update_mesh_slot());
		m_tag_edges.changed_signal().connect(make_update_mesh_slot());
		m_tag_faces.changed_signal().connect(make_update_mesh_slot());
		m_tag_polyhedra.changed_signal().connect(make_update_mesh_slot());
		m_tag_linear_curve_groups.changed_signal().connect(make_update_mesh_slot());
		m_tag_cubic_curve_groups.changed_signal().connect(make_update_mesh_slot());
		m_tag_bilinear_patches.changed_signal().connect(make_update_mesh_slot());
		m_tag_bicubic_patches.changed_signal().connect(make_update_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::replace_selection(m_mesh_selection.value(), Mesh);
	}

	void on_update_mesh(const k3d::legacy::mesh& Source, k3d::legacy::mesh& Target)
	{
		const k3d::color new_color = m_new_color.value();
		const k3d::color default_color = m_default_color.value();

		if(m_tag_points.value())
		{
			for(k3d::legacy::mesh::points_t::iterator point = Target.points.begin(); point != Target.points.end(); ++point)
				set_color(**point, new_color, default_color, (*point)->vertex_data);
		}

		if(m_tag_point_groups.value())
		{
			for(k3d::legacy::mesh::point_groups_t::iterator group = Target.point_groups.begin(); group != Target.point_groups.end(); ++group)
				set_color(**group, new_color, default_color, (*group)->constant_data);
		}

		if(m_tag_linear_curve_groups.value())
		{
			for(k3d::legacy::mesh::linear_curve_groups_t::iterator group = Target.linear_curve_groups.begin(); group != Target.linear_curve_groups.end(); ++group)
				set_color(**group, new_color, default_color, (*group)->constant_data);
		}

		if(m_tag_cubic_curve_groups.value())
		{
			for(k3d::legacy::mesh::cubic_curve_groups_t::iterator group = Target.cubic_curve_groups.begin(); group != Target.cubic_curve_groups.end(); ++group)
				set_color(**group, new_color, default_color, (*group)->constant_data);
		}

		if(m_tag_bilinear_patches.value())
		{
			for(k3d::legacy::mesh::bilinear_patches_t::iterator patch = Target.bilinear_patches.begin(); patch != Target.bilinear_patches.end(); ++patch)
				set_color(**patch, new_color, default_color, (*patch)->uniform_data);
		}

		if(m_tag_bicubic_patches.value())
		{
			for(k3d::legacy::mesh::bicubic_patches_t::iterator patch = Target.bicubic_patches.begin(); patch != Target.bicubic_patches.end(); ++patch)
				set_color(**patch, new_color, default_color, (*patch)->uniform_data);
		}

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Target.polyhedra.begin(); polyhedron != Target.polyhedra.end(); ++polyhedron)
		{
			if(m_tag_polyhedra.value())
			{
				set_color(**polyhedron, new_color, default_color, (*polyhedron)->constant_data);
			}

			if(m_tag_edges.value())
			{
				for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
				{
					k3d::legacy::split_edge* edge = (*face)->first_edge;
					do
					{
						set_color(*edge, new_color, default_color, edge->facevarying_data);

						edge = edge->face_clockwise;
					}
					while(edge != (*face)->first_edge);
				}
			}

			if(m_tag_faces.value())
			{
				for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
					set_color(**face, new_color, default_color, (*face)->uniform_data);
			}
		}
	}
	
	template<typename T>
	void set_color(T& Object, const k3d::color& NewColor, const k3d::color& DefaultColor, k3d::legacy::parameters_t& Parameters)
	{
		if(Object.selection_weight)
			Parameters["Cs"] = NewColor;
		else if(0 == Parameters.count("Cs"))
			Parameters["Cs"] = DefaultColor;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<tag_color_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x852e9ffb, 0x8e8b42f3, 0x993eb194, 0x5c342ca4),
				"TagColor",
				_("Tags geometry with a color"),
				"Textures",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_new_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_default_color;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_points;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_point_groups;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_edges;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_faces;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_polyhedra;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_linear_curve_groups;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_cubic_curve_groups;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_bilinear_patches;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_bicubic_patches;
};

/////////////////////////////////////////////////////////////////////////////
// tag_color_factory

k3d::iplugin_factory& tag_color_factory()
{
	return tag_color_implementation::get_factory();
}

} // namespace libk3dmesh

