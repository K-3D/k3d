// K-3D
// Copyright (c) 2002-2005, Romain Behar
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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/material.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>

#include <iterator>

namespace libk3dprimitives
{

/////////////////////////////////////////////////////////////////////////////
// poly_cushion_implementation

class poly_cushion_implementation :
	public k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	poly_cushion_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_length_segments(init_owner(*this) + init_name("length_segments") + init_label(_("Length segments")) + init_description(_("Length segments")) + init_value(5) + init_constraint(constraint::minimum(1L)) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar))),
		m_radial_segments(init_owner(*this) + init_name("radial_segments") + init_label(_("Radial segments")) + init_description(_("Radial segments")) + init_value(5) + init_constraint(constraint::minimum(1L)) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar))),
		m_diameter(init_owner(*this) + init_name("diameter") + init_label(_("Diameter")) + init_description(_("Rounded parts diameter")) + init_value(4.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::distance))),
		m_width(init_owner(*this) + init_name("width") + init_label(_("Width")) + init_description(_("Original cube width")) + init_value(8.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Original cube height")) + init_value(8.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_depth(init_owner(*this) + init_name("depth") + init_label(_("Depth")) + init_description(_("Original cube depth")) + init_value(8.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_length_segments.changed_signal().connect(make_reset_mesh_slot());
		m_radial_segments.changed_signal().connect(make_reset_mesh_slot());
		m_diameter.changed_signal().connect(make_reset_mesh_slot());
		m_width.changed_signal().connect(make_reset_mesh_slot());
		m_height.changed_signal().connect(make_reset_mesh_slot());
		m_depth.changed_signal().connect(make_reset_mesh_slot());
	}

	void add_point(k3d::point3 Position)
	{
		m_mesh->points.push_back(new k3d::legacy::point(Position));
	}

	void add_face(const unsigned long v1, const unsigned long v2, const unsigned long v3)
	{
		k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(m_mesh->points[v1]);
		k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(m_mesh->points[v2]);
		k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(m_mesh->points[v3]);

		edge1->face_clockwise = edge2;
		edge2->face_clockwise = edge3;
		edge3->face_clockwise = edge1;

		m_polyhedron->faces.push_back(new k3d::legacy::face(edge1, m_material.value()));
	}

	void add_face(const unsigned long v1, const unsigned long v2, const unsigned long v3, const unsigned long v4)
	{
		k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(m_mesh->points[v1]);
		k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(m_mesh->points[v2]);
		k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(m_mesh->points[v3]);
		k3d::legacy::split_edge* edge4 = new k3d::legacy::split_edge(m_mesh->points[v4]);

		edge1->face_clockwise = edge2;
		edge2->face_clockwise = edge3;
		edge3->face_clockwise = edge4;
		edge4->face_clockwise = edge1;

		m_polyhedron->faces.push_back(new k3d::legacy::face(edge1, m_material.value()));
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		/** \todo Get rid of this - pass the mesh as a method argument instead */
		m_mesh = &Mesh;

		Mesh.polyhedra.push_back(new k3d::legacy::polyhedron());
		m_polyhedron = Mesh.polyhedra.back();

		const unsigned long length_segments = m_length_segments.value();
		const unsigned long radial_segments = m_radial_segments.value();
		const double diameter = m_diameter.value();
		const double width = m_width.value();
		const double height = m_height.value();
		const double depth = m_depth.value();

		// Build a sphere
		typedef std::vector<k3d::point3> points_t;
		points_t sphere_points;

		const unsigned long N = length_segments*4;
		const unsigned long R = radial_segments*2-1;

		unsigned long i, j;

		for(j = 0; j < N; j++)
			for(i = 0; i < R; i++)
				{
					double l = sin(k3d::pi()/2 -(double)(i+1)*k3d::pi() / (double)(R+1))*diameter/2;
					double r = sqrt(diameter*diameter/4 - l*l);

					sphere_points.push_back(k3d::point3( r*sin((double)j*2*k3d::pi() / (double)N), l, r*cos((double)j*2*k3d::pi() / (double)N)));
				}
		sphere_points.push_back(k3d::point3(0, diameter/2, 0));
		sphere_points.push_back(k3d::point3(0, -diameter/2, 0));

		// Cut sphere into 8 parts (cushion corners)
		const unsigned long c = (R+1)/2;

		k3d::point3 tP = k3d::point3(width / 2.0, height / 2.0, depth / 2.0);
		for(j = 0; j < N / 4+1; j++)
			for(i = 0; i < c; i++)
				add_point(sphere_points[R == 1 ? j : j*R+i] + tP);

		add_point(sphere_points[N*R] + tP);

		tP = k3d::point3(width / 2.0, -height / 2.0, depth / 2.0);
		for(j = 0; j < N / 4+1; j++)
			for(i = 0; i < c; i++)
				add_point(sphere_points[R / 2 +(R == 1 ? j : j*R+i)] + tP);

		add_point(sphere_points[N*R+1] + tP);

		tP = k3d::point3(width / 2.0, height / 2.0, -depth / 2.0);
		for(j = N / 4; j < N / 2+1; j++)
			for(i = 0; i < c; i++)
				add_point(sphere_points[R == 1 ? j : j*R+i] + tP);

		add_point(sphere_points[N*R] + tP);

		tP = k3d::point3(width / 2.0, -height / 2.0, -depth / 2.0);
		for(j = N / 4; j < N / 2+1; j++)
			for(i = 0; i < c; i++)
				add_point(sphere_points[R / 2 +(R == 1 ? j : j*R+i)] + tP);

		add_point(sphere_points[N*R+1] + tP);

		tP = k3d::point3(-width / 2.0, height / 2.0, -depth / 2.0);
		for(j = N / 2; j < 3*N / 4+1; j++)
			for(i = 0; i < c; i++)
				add_point(sphere_points[R == 1 ? j : j*R+i] + tP);

		add_point(sphere_points[N*R] + tP);

		tP = k3d::point3(-width / 2.0, -height / 2.0, -depth / 2.0);
		for(j = N / 2; j < 3*N / 4+1; j++)
			for(i = 0; i < c; i++)
				add_point(sphere_points[R / 2 +(R == 1 ? j : j*R+i)] + tP);

		add_point(sphere_points[N*R+1] + tP);

		tP = k3d::point3(-width / 2.0, height / 2.0, depth / 2.0);
		for(j = 3*N / 4; j <= N; j++)
			for(i = 0; i < c; i++)
				add_point(sphere_points[R == 1 ? j % N :(j % N)*R+i] + tP);

		add_point(sphere_points[N*R] + tP);

		tP = k3d::point3(-width / 2.0, -height / 2.0, depth / 2.0);
		for(j = 3*N / 4; j <= N; j++)
			for(i = 0; i < c; i++)
				add_point(sphere_points[R / 2 +(R == 1 ? j % N :(j % N)*R+i)] + tP);

		add_point(sphere_points[N*R+1] + tP);

		// Make faces for sphere parts
		const unsigned long d = c*(N/4+1)+1;

		for(unsigned long n = 0; n < 8; n++)
			{
				for(j = 0; j < N / 4; j++)
					for(i = 0; i < c-1; i++)
						add_face(n*d + c*j + i, n*d + c*j + i+1, n*d + c*(j+1) + i+1, n*d + c*(j+1) + i);

				for(j = 0; j < N / 4; j ++)
					if(n % 2 == 0)
						add_face(n*d + c*j, n*d + c*(j+1), (n+1) * d - 1);
					else
						add_face(n*d + c*(j+1) - 1, (n+1) * d - 1, n*d + c*(j+2) - 1);
			}

		// Link parts
		for(unsigned long n = 0; n < 8; n++)
			{
				const unsigned long e = (n+1)*d - c-1;
				const unsigned long f = ((n+2) % 8)*d;

				for(j = 0; j < c-1; j++)
					add_face(e+j, e+j+1, f+j+1, f+j);
			}

		add_face(0, d - 1, 7*d - 1, 7*d - c-1);
		add_face(d - c-1, 2*d, 3*d - 1, d - 1);
		add_face(3*d - c-1, 4*d, 5*d - 1, 3*d - 1);
		add_face(5*d - c-1, 6*d, 7*d - 1, 5*d - 1);

		add_face(d + c-1, 8*d - 2, 8*d - 1, 2*d - 1);
		add_face(2*d - 2, 2*d - 1, 4*d - 1, 3*d + c-1);
		add_face(4*d - 2, 4*d - 1, 6*d - 1, 5*d + c-1);
		add_face(6*d - 2, 6*d - 1, 8*d - 1, 7*d + c-1);


		for(unsigned long n = 0; n < 4; n++)
			{
				const unsigned long e = d*(n*2) + c-1;
				const unsigned long f = d*(n*2+1);

				for(j = 0; j < N / 4; j++)
					add_face(e+j*c, f+j*c, f+(j+1)*c, e+(j+1)*c);
			}

		add_face(c-1, d*7 - 2, d*8 - c-1, d);
		add_face(d-2, d*2 - c-1, d*3, d*2 + c-1);
		add_face(d*3 - 2, d*4 - c-1, d*5, d*4 + c-1);
		add_face(d*5 - 2, d*6 - c-1, d*7, d*6 + c-1);

		add_face(d-1, d*3 - 1, d*5 - 1, d*7 - 1);
		add_face(d*2 - 1, d*8 - 1, d*6 - 1, d*4 - 1);

		// Make it a solid
		k3d::legacy::set_companions(*m_polyhedron);
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_cushion_implementation, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0xc11b963d, 0x108d471c, 0xa3826195, 0x821116b0),
			"PolyCushion",
			_("Generates a polygonal cushion (a cube with rounded edges)"),
			"Polygons",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_length_segments;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_radial_segments;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_diameter;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_width;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_depth;

	/** \todo Get rid of this and pass the mesh in method arguments */
	k3d::legacy::mesh* m_mesh;
	/** \todo Get rid of this and pass the polyhedron in method arguments */
	k3d::legacy::polyhedron* m_polyhedron;
};

/////////////////////////////////////////////////////////////////////////////
// poly_cushion_factory

k3d::iplugin_factory& poly_cushion_factory()
{
	return poly_cushion_implementation::get_factory();
}

} // namespace libk3dprimitives

