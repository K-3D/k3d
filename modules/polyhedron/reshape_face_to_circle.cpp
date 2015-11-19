// K-3D
// Copyright (c) 2004-2010, Timothy M. Shead
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
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/table_copier.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// reshape_face_to_circle

class reshape_face_to_circle :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	reshape_face_to_circle(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("radius")) + init_description(_("Radius")) + init_value(0.5) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_reset_mesh_slot()));
		m_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_geometry_changed> >(make_reset_mesh_slot()));
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		if(!Output.points)
			return;
		if(!Output.point_selection)
			return;

		k3d::mesh::points_t& points = Output.points.writable();

		const k3d::double_t radius = m_radius.pipeline_value();

		// For each polyhedron ...
		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			// For each selected face ...
			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + polyhedron->face_shells.size();
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(!polyhedron->face_selections[face])
					continue;

				// first compute the centroid
				k3d::point3 centroid(0, 0, 0);
				k3d::uint_t npoints = 0;
				const k3d::uint_t first_loop = polyhedron->face_first_loops[face];
				const k3d::uint_t first_edge = polyhedron->loop_first_edges[first_loop];
				for(k3d::uint_t edge = first_edge; ; )
				{
					centroid += k3d::to_vector(points[polyhedron->vertex_points[edge]]);
					++npoints;

					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
					{
						break;
					}
				}

				centroid /= k3d::double_t(npoints);
				const k3d::double_t angle_step = 2.*k3d::pi() / k3d::double_t(npoints);
				k3d::point3& first_point = points[polyhedron->vertex_points[first_edge]];
				const k3d::normal3 normal = k3d::polyhedron::normal(polyhedron->vertex_points, polyhedron->clockwise_edges, points, first_edge);
				first_point = centroid + radius * (first_point - centroid) / (first_point - centroid).length();
				k3d::double_t angle = 0;

				// Move the points
				for(k3d::uint_t edge = first_edge; ; )
				{
					points[polyhedron->vertex_points[edge]] = k3d::rotate3(k3d::angle_axis(angle, k3d::to_vector(normal))) * first_point;
					angle += angle_step;

					edge = polyhedron->clockwise_edges[edge];
					if(edge == first_edge)
					{
						break;
					}
				}
			}
		}
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<reshape_face_to_circle,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x7adb5e69, 0xec6e447f, 0x93070dd8, 0xf78f2f65),
				"ReshapeFaceToCircle",
				"Reshapes the first edge loop of the face to a circle.",
				"Polyhedron",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
};

/////////////////////////////////////////////////////////////////////////////
// reshape_face_to_circle_factory

k3d::iplugin_factory& reshape_face_to_circle_factory()
{
	return reshape_face_to_circle::get_factory();
}

} // namespace polyhedron

} // namespace module
