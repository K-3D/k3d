// K-3D
// Copyright (c) 2004-2008, Timothy M. Shead
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

#include <k3d-i18n-config.h>

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <k3dsdk/subdivision_surface/catmull_clark.h>

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/scoped_ptr.hpp>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// catmull_clark_subdivider

class catmull_clark_subdivider :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	catmull_clark_subdivider(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_level(init_owner(*this) + init_name("level") + init_label(_("Level")) + init_description(_("Subdivision level")) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_value(1) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_level.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		m_subdividers.clear();
		Output = Input;
		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);
		const k3d::uint_t level = m_level.pipeline_value();
		for(k3d::mesh::primitives_t::const_iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
			if(!polyhedron.get())
				continue;
			m_subdividers[primitive->get()].set_levels(level);
			m_subdividers[primitive->get()].create_mesh(*Output.points, *polyhedron, polyhedron->face_selections, this);
		}
		k3d::mesh::selection_t& output_point_selection = Output.point_selection.create();
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		k3d::mesh::primitives_t::const_iterator input_primitive = Input.primitives.begin();
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(*primitive));
			if(!polyhedron.get())
				continue;
			boost::scoped_ptr<k3d::polyhedron::const_primitive> input_polyhedron(k3d::polyhedron::validate(**input_primitive));
			polyhedron->face_selections = input_polyhedron->face_selections;
			++input_primitive;
		}
		k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), Output);
		Output.points = Input.points;
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(*primitive));
			if(!polyhedron.get())
				continue;
			m_subdividers[primitive->get()].update_mesh(*Output.points, *polyhedron, Output.vertex_data, polyhedron->face_selections, this);
			m_subdividers[primitive->get()].copy_output(Output.points.writable(), *polyhedron, Output.vertex_data);
			Output.point_selection.writable().resize(Output.points->size());
		}
	}


	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<catmull_clark_subdivider,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xdc73f6e3, 0x744b4b51, 0xac535307, 0xc82d261c),
				"CatmullClark",
				_("Generates a Catmull-Clark subdivision mesh"),
				"Subdivision",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_level;
	boost::ptr_map<const k3d::mesh::primitive*, k3d::sds::catmull_clark_subdivider> m_subdividers;
};

/////////////////////////////////////////////////////////////////////////////
// catmull_clark_subdivision_factory

k3d::iplugin_factory& catmull_clark_subdivision_factory()
{
	return module::mesh::catmull_clark_subdivider::get_factory();
}

} // namespace mesh

} // namespace module

