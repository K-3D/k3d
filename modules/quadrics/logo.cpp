// K-3D
// Copyright (c) 2008, Timothy M. Shead 
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
#include <k3dsdk/basic_math.h>
#include <k3dsdk/cylinder.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/sphere.h>
#include <k3dsdk/torus.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace quadrics
{

/////////////////////////////////////////////////////////////////////////////
// logo

class logo :
	public k3d::material_sink<k3d::mesh_source<k3d::node> >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node> > base;

public:
	logo(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Controls the radius of the output logo.")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_radius.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	static void add_capsule(k3d::cylinder::primitive& Cylinders, k3d::sphere::primitive& Spheres, k3d::imaterial* const Material, const k3d::double_t Radius, const k3d::point3& From, const k3d::point3& To, const k3d::matrix4& Global)
	{
		const k3d::vector3 look = k3d::normalize(To - From);
		const k3d::vector3 right = k3d::vector3(0, -1, 0);
		const k3d::vector3 up = look ^ right;

		const k3d::double_t length = k3d::length(To - From);

		const k3d::matrix4 place(
			k3d::vector4(right[0], up[0], look[0], From[0]),
			k3d::vector4(right[1], up[1], look[1], From[1]),
			k3d::vector4(right[2], up[2], look[2], From[2]),
			k3d::vector4(0, 0, 0, 1));

		Spheres.matrices.push_back(Global * place * k3d::translate3(0, 0, length));
		Spheres.materials.push_back(Material);
		Spheres.radii.push_back(Radius);
		Spheres.z_min.push_back(0.0);
		Spheres.z_max.push_back(1.0);
		Spheres.sweep_angles.push_back(k3d::pi_times_2());
		Spheres.selections.push_back(0.0);

		Cylinders.matrices.push_back(Global * place);
		Cylinders.materials.push_back(Material);
		Cylinders.radii.push_back(Radius);
		Cylinders.z_min.push_back(0.0);
		Cylinders.z_max.push_back(length);
		Cylinders.sweep_angles.push_back(k3d::pi_times_2());
		Cylinders.selections.push_back(0.0);

		Spheres.matrices.push_back(Global * place);
		Spheres.materials.push_back(Material);
		Spheres.radii.push_back(Radius);
		Spheres.z_min.push_back(-1.0);
		Spheres.z_max.push_back(0.0);
		Spheres.sweep_angles.push_back(k3d::pi_times_2());
		Spheres.selections.push_back(0.0);
	}

	static void add_sphere(k3d::sphere::primitive& Spheres, k3d::imaterial* const Material, const k3d::double_t Radius, const k3d::point3& Center, const k3d::matrix4& Global)
	{
		const k3d::matrix4 center = k3d::translate3(Center[0], Center[1], Center[2]);

		Spheres.matrices.push_back(Global * center);
		Spheres.materials.push_back(Material);
		Spheres.radii.push_back(Radius);
		Spheres.z_min.push_back(-1.0);
		Spheres.z_max.push_back(1.0);
		Spheres.sweep_angles.push_back(k3d::pi_times_2());
		Spheres.selections.push_back(0.0);
	}

	static void add_torus(k3d::torus::primitive& Tori, k3d::sphere::primitive& Spheres, k3d::imaterial* const Material, const k3d::double_t Radius, const k3d::point3& Center, const k3d::double_t MajorRadius, const k3d::double_t SweepAngle, const k3d::matrix4& Global, const k3d::matrix4& Orientation)
	{
		const k3d::matrix4 center = k3d::translate3(Center[0], Center[1], Center[2]);

		Spheres.matrices.push_back(Global * center * Orientation * k3d::translate3(MajorRadius, 0, 0) * k3d::rotate3(k3d::pi_over_2(), k3d::vector3(1, 0, 0)));
		Spheres.materials.push_back(Material);
		Spheres.radii.push_back(Radius);
		Spheres.z_min.push_back(0.0);
		Spheres.z_max.push_back(1.0);
		Spheres.sweep_angles.push_back(k3d::pi_times_2());
		Spheres.selections.push_back(0.0);

		Tori.matrices.push_back(Global * center * Orientation);
		Tori.materials.push_back(Material);
		Tori.major_radii.push_back(MajorRadius);
		Tori.minor_radii.push_back(Radius);
		Tori.phi_min.push_back(0.0);
		Tori.phi_max.push_back(k3d::pi_times_2());
		Tori.sweep_angles.push_back(SweepAngle);
		Tori.selections.push_back(0.0);

		Spheres.matrices.push_back(Global * center * Orientation * k3d::rotate3(SweepAngle, k3d::vector3(0, 0, 1)) * k3d::translate3(MajorRadius, 0, 0) * k3d::rotate3(k3d::pi_over_2(), k3d::vector3(1, 0, 0)));
		Spheres.materials.push_back(Material);
		Spheres.radii.push_back(Radius);
		Spheres.z_min.push_back(-1.0);
		Spheres.z_max.push_back(0.0);
		Spheres.sweep_angles.push_back(k3d::pi_times_2());
		Spheres.selections.push_back(0.0);
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::imaterial* const material = m_material.pipeline_value();
		const k3d::double_t radius = m_radius.pipeline_value();

		boost::scoped_ptr<k3d::cylinder::primitive> cylinders(k3d::cylinder::create(Output));
		boost::scoped_ptr<k3d::sphere::primitive> spheres(k3d::sphere::create(Output));
		boost::scoped_ptr<k3d::torus::primitive> tori(k3d::torus::create(Output));

		const k3d::matrix4 transformation = k3d::scale3(0.1, 0.1, 0.1);

		// Create the "K"
		add_capsule(*cylinders, *spheres, material, radius, k3d::point3(11.5, 0, -11.5), k3d::point3(11.5, 0, -70), transformation);
		add_capsule(*cylinders, *spheres, material, radius, k3d::point3(37.5, 0, -63), k3d::point3(62.917, 0, -88.417), transformation);
		add_sphere(*spheres, material, radius, k3d::point3(47, 0, -39), transformation);

		// Create the "-"
		add_capsule(*cylinders, *spheres, material, radius, k3d::point3(71, 0, -57.5), k3d::point3(90.917, 0, -57.5), transformation);

		// Create the "3"
		add_capsule(*cylinders, *spheres, material, radius, k3d::point3(145, 0, -30), k3d::point3(100.5, 0, -30), transformation);
		add_capsule(*cylinders, *spheres, material, radius, k3d::point3(122.25, 0, -56.5), k3d::point3(145, 0, -30), transformation);
		add_torus(*tori, *spheres, material, radius, k3d::point3(122.25, 0, -80.25), 23.75, 3 * k3d::pi_over_2(), transformation, k3d::rotate3(k3d::pi(), k3d::vector3(0, 1, 0)) * k3d::rotate3(k3d::pi_over_2(), k3d::vector3(1, 0, 0)));

		// Create the "D"
		add_capsule(*cylinders, *spheres, material, radius, k3d::point3(172, 0, -29), k3d::point3(172, 0, -104.5), transformation);
		add_torus(*tori, *spheres, material, radius, k3d::point3(197.5, 0, -66.75), 37.75, k3d::pi(), transformation, k3d::rotate3(k3d::pi_over_2(), k3d::vector3(0, 1, 0)) * k3d::rotate3(k3d::pi_over_2(), k3d::vector3(1, 0, 0)));
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<logo > factory(
			k3d::uuid(0xefc80ecb, 0x9340c5b0, 0x0061109e, 0xc97591a3),
			"QuadricLogo",
			"Creates the K-3D logo using quadric primitives.",
			"Quadric",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
};

k3d::iplugin_factory& logo_factory()
{
	return logo::get_factory();
}

} // namespace quadrics

} // namespace module

