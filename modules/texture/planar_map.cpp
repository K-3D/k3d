// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i3d_2d_mapping.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/texture3.h>
#include <k3dsdk/transformable.h>
#include <k3dsdk/utility_gl.h>

#include <iterator>

namespace libk3dtexture
{

namespace detail
{

class non_empty_string : public k3d::data::iconstraint< k3d::string_t >
{
public:

  non_empty_string(const k3d::string_t& Default, iconstraint<k3d::string_t>* NextConstraint = 0) :
      iconstraint<k3d::string_t>(NextConstraint),
        m_default(Default)
      {
      }

  virtual void on_constrain(k3d::string_t& Value)
  {
    if( Value.size() == 0 )
      Value = m_default;
  }

private:
  k3d::string_t m_default;
};

}

/////////////////////////////////////////////////////////////////////////////
// planar_map

class planar_map :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::transformable< k3d::node > > > 
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::transformable< k3d::node > > >  base_t;
	typedef k3d::typed_array<k3d::texture3> texcoord_array_t;

public:
	planar_map(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base_t(Factory, Document),
		m_texcoord_set_name(init_owner(*this) + init_name("texcoord_set") + init_label(_("Texture Coordinate Set")) + init_description(_("Texture Mapping Set")) + init_constraint(new detail::non_empty_string("texcoord")) + init_value<k3d::string_t>("texcoord"))
	{
		m_texcoord_set_name.changed_signal().connect(make_reset_mesh_slot());
		m_input_matrix.changed_signal().connect(make_update_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;

		if(!k3d::validate_polyhedra(Output))
			return;

		k3d::mesh::polyhedra_t& polyhedra = Output.polyhedra.writable();
		polyhedra.face_varying_data.create(m_texcoord_set_name.pipeline_value(), new texcoord_array_t(polyhedra.edge_points->size()));
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(!k3d::validate_polyhedra(Output))
			return;

		const k3d::matrix4 inv_matrix = inverse(m_input_matrix.pipeline_value());

		const k3d::mesh::points_t& points = *Output.points;
		const k3d::mesh::indices_t& edge_points = *Output.polyhedra->edge_points;
		texcoord_array_t& texcoords = *Output.polyhedra.writable().face_varying_data.writable<texcoord_array_t>(m_texcoord_set_name.pipeline_value());

		const k3d::uint_t edge_begin = 0;
		const k3d::uint_t edge_end = edge_points.size();
		for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
		{
			const k3d::point3 point = inv_matrix * points[edge_points[edge]];
			texcoords[edge].n[0] = point.n[0];
			texcoords[edge].n[1] = point.n[1];
			texcoords[edge].n[2] = point.n[2];
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<planar_map,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xd9148b60, 0x7f2a49f0, 0xa28e505a, 0xa3f4d4df),
				"PlanarMap",
				_("Planar projection map"),
				"Texture",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_texcoord_set_name;
};

/////////////////////////////////////////////////////////////////////////////
// planar_map_factory

k3d::iplugin_factory& planar_map_factory()
{
	return planar_map::get_factory();
}

} // namespace libk3dtexture

