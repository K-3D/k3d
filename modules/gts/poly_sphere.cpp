// K-3D
// Copyright (c) 2004-2007, Romain Behar
//
// Contact: romainbehar@yahoo.com
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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include "helpers.h"

namespace module
{

namespace gts
{

/////////////////////////////////////////////////////////////////////////////
// poly_sphere_tessellation

class poly_sphere_tessellation :
	public k3d::material_client<k3d::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	poly_sphere_tessellation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_tessellation_level(init_owner(*this) + init_name("tessellation_level") + init_label(_("Tessellation level")) + init_description(_("Tessellation Level")) + init_value(3) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Sphere radius")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_tessellation_level.changed_signal().connect(make_topology_changed_slot());
		m_material.changed_signal().connect(make_topology_changed_slot());
		m_radius.changed_signal().connect(make_topology_changed_slot());
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		const gts_ptr<GtsSurface> gts_surface = gts_surface_new(gts_surface_class(), gts_face_class(), gts_edge_class(), gts_vertex_class());

		gts_surface_generate_sphere(gts_surface, m_tessellation_level.pipeline_value());

		Mesh = convert(gts_surface);

/*
		// Apply radius
		double radius = m_radius.pipeline_value();
		k3d::legacy::mesh::points_t::iterator mesh_end(Mesh.points.end());
		for(k3d::legacy::mesh::points_t::iterator p = Mesh.points.begin(); p != mesh_end; ++p)
			(*p)->position *= radius;

		// Set material
		k3d::imaterial* const material = m_material.pipeline_value();
		k3d::legacy::polyhedron& polyhedron = *Mesh.polyhedra.back();
		for(k3d::legacy::polyhedron::faces_t::iterator face = polyhedron.faces.begin(); face != polyhedron.faces.end(); ++face)
			(*face)->material = material;
*/
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_sphere_tessellation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x5d228c83, 0xff624faa, 0xa95cb0ee, 0xc2b1613b),
			"GTSPolySphere",
			_("Generates a polygonal sphere using recursive subdivision of an isocahedron"),
			"Polygons",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_tessellation_level;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
};

/////////////////////////////////////////////////////////////////////////////
// poly_sphere_tessellation_factory

k3d::iplugin_factory& poly_sphere_tessellation_factory()
{
	return poly_sphere_tessellation::get_factory();
}

} // namespace gts

} // namespace module

