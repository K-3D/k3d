// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
#include <k3dsdk/algebra.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/bounding_box.h>
#include <k3dsdk/color.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>

#include <iterator>

namespace libk3dtexture
{

/////////////////////////////////////////////////////////////////////////////
// planar_map2

class planar_map2 :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > > base;

public:
	planar_map2(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_axis(init_owner(*this) + init_name("axis") + init_label(_("axis")) + init_description(_("Projection Axis")) + init_value(k3d::PZ) + init_enumeration(k3d::signed_axis_values())),
		m_s0(init_owner(*this) + init_name("s0") + init_label(_("s0")) + init_description(_("S0")) + init_value(0)),
		m_s1(init_owner(*this) + init_name("s1") + init_label(_("s1")) + init_description(_("S1")) + init_value(1)),
		m_t0(init_owner(*this) + init_name("t0") + init_label(_("t0")) + init_description(_("T0")) + init_value(0)),
		m_t1(init_owner(*this) + init_name("t1") + init_label(_("t1")) + init_description(_("T1")) + init_value(1)),
		m_default_s(init_owner(*this) + init_name("default_s") + init_label(_("default_s")) + init_description(_("Default S")) + init_value(0)),
		m_default_t(init_owner(*this) + init_name("default_t") + init_label(_("default_t")) + init_description(_("Default T")) + init_value(0)),
		m_tag_points(init_owner(*this) + init_name("tag_points") + init_label(_("tag_points")) + init_description(_("Tag Points")) + init_value(false)),
		m_tag_edges(init_owner(*this) + init_name("tag_edges") + init_label(_("tag_edges")) + init_description(_("Tag Edges")) + init_value(false))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_axis.changed_signal().connect(make_reset_mesh_slot());
		m_s0.changed_signal().connect(make_reset_mesh_slot());
		m_s1.changed_signal().connect(make_reset_mesh_slot());
		m_t0.changed_signal().connect(make_reset_mesh_slot());
		m_t1.changed_signal().connect(make_reset_mesh_slot());
		m_default_s.changed_signal().connect(make_reset_mesh_slot());
		m_default_t.changed_signal().connect(make_reset_mesh_slot());
		m_tag_points.changed_signal().connect(make_reset_mesh_slot());
		m_tag_edges.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::replace_selection(m_mesh_selection.pipeline_value(), Mesh);

		const double s0 = m_s0.pipeline_value();
		const double s1 = m_s1.pipeline_value();
		const double t0 = m_t0.pipeline_value();
		const double t1 = m_t1.pipeline_value();
		const double default_s = m_default_s.pipeline_value();
		const double default_t = m_default_t.pipeline_value();

		k3d::matrix4 transformation = k3d::identity3D();
		switch(m_axis.pipeline_value())
		{
			case k3d::NX:
				transformation = k3d::rotation3D(k3d::radians(90.0), k3d::vector3(0, 1, 0));
				break;

			case k3d::PX:
				transformation = k3d::rotation3D(k3d::radians(-90.0), k3d::vector3(0, 1, 0));
				break;

			case k3d::NY:
				transformation = k3d::rotation3D(k3d::radians(-90.0), k3d::vector3(1, 0, 0));
				break;

			case k3d::PY:
				transformation = k3d::rotation3D(k3d::radians(90.0), k3d::vector3(1, 0, 0));
				break;

			case k3d::NZ:
				transformation = k3d::rotation3D(k3d::radians(180.0), k3d::vector3(0, 1, 0));
				break;

			case k3d::PZ:
				break;

			default:
				k3d::log() << warning << k3d_file_reference << ": unknown axis enumeration" << std::endl;
				break;
		}

		// Compute a bounding-box that contains all selected points ...
		k3d::bounding_box3 bounds;
		if(m_tag_points.pipeline_value())
		{
			for(k3d::legacy::mesh::points_t::iterator point = Mesh.points.begin(); point != Mesh.points.end(); ++point)
				update_bounds(**point, transformation, (*point)->position, bounds);
		}

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			if(m_tag_edges.pipeline_value())
			{
				for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
				{
					k3d::legacy::split_edge* edge = (*face)->first_edge;
					do
					{
						if(edge->vertex)
							update_bounds(*edge, transformation, edge->vertex->position, bounds);

						edge = edge->face_clockwise;
					}
					while(edge != (*face)->first_edge);
				}
			}
		}

		// If we didn't have any input to process, we're done ...
		if(bounds.empty())
			return;

		// Calculate s,t coordinates using linear interpolation
		if(m_tag_points.pipeline_value())
		{
			for(k3d::legacy::mesh::points_t::iterator point = Mesh.points.begin(); point != Mesh.points.end(); ++point)
				set_coordinates(**point, transformation, (*point)->position, bounds, s0, s1, t0, t1, default_s, default_t, (*point)->vertex_data);
		}

		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			if(m_tag_edges.pipeline_value())
			{
				for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
				{
					k3d::legacy::split_edge* edge = (*face)->first_edge;
					do
					{
						if(edge->vertex)
							set_coordinates(*edge, transformation, edge->vertex->position, bounds, s0, s1, t0, t1, default_s, default_t, edge->facevarying_data);

						edge = edge->face_clockwise;
					}
					while(edge != (*face)->first_edge);
				}
			}
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	template<typename T>
	void update_bounds(T& Object, const k3d::matrix4& Transformation, const k3d::point3& Position, k3d::bounding_box3& Bounds)
	{
		if(Object.selection_weight)
			Bounds.insert(Transformation * Position);
	}

	template<typename T>
	void set_coordinates(T& Object, const k3d::matrix4& Transformation, const k3d::point3& Position, const k3d::bounding_box3& Bounds, const double S0, const double S1, const double T0, const double T1, const double DefaultS, const double DefaultT, k3d::legacy::parameters_t& Parameters)
	{
		if(Object.selection_weight)
		{
			const k3d::point3 position = Transformation * Position;

			Parameters["s"] = Bounds.width() ? k3d::mix(S0, S1, (position[0] - Bounds.nx) / (Bounds.width())) : DefaultS;
			Parameters["t"] = Bounds.height() ? k3d::mix(T0, T1, (Bounds.py - position[1]) / (Bounds.height())) : DefaultT;
		}
		else
		{
			if(0 == Parameters.count("s"))
				Parameters["s"] = DefaultS;
			if(0 == Parameters.count("t"))
				Parameters["t"] = DefaultT;
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<planar_map2,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9e25bcdd, 0x621d4536, 0xa622a63f, 0xa59e874b),
				"PlanarMap2",
				_("Maps values to geometry using a planar projection map"),
				"Textures",
				k3d::iplugin_factory::DEPRECATED);

		return factory;
	}

private:
	k3d_data(k3d::signed_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_axis;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_s0;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_s1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_t0;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_t1;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_default_s;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_default_t;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_points;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_tag_edges;
};

/////////////////////////////////////////////////////////////////////////////
// planar_map2_factory

k3d::iplugin_factory& planar_map2_factory()
{
	return planar_map2::get_factory();
}

} // namespace libk3dtexture

