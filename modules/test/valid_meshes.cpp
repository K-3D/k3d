// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
#include <k3dsdk/bezier_triangle_patch.h>
#include <k3dsdk/bicubic_patch.h>
#include <k3dsdk/bilinear_patch.h>
#include <k3dsdk/blobby.h>
#include <k3dsdk/cone.h>
#include <k3dsdk/cubic_curve.h>
#include <k3dsdk/cylinder.h>
#include <k3dsdk/disk.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/hyperboloid.h>
#include <k3dsdk/linear_curve.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/paraboloid.h>
#include <k3dsdk/particle.h>
#include <k3dsdk/primitive_validation.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/sphere.h>
#include <k3dsdk/teapot.h>
#include <k3dsdk/torus.h>
#include <k3dsdk/user_property_changed_signal.h>
#include <k3dsdk/value_demand_storage.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace test
{

/////////////////////////////////////////////////////////////////////////////
// valid_meshes

class valid_meshes :
	public k3d::node
{
	typedef k3d::node base;

public:
	valid_meshes(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_valid(init_owner(*this) + init_name("valid") + init_label(_("Valid")) + init_description(_("True iff every every input mesh can be validated.")) + init_value(true)),
		m_user_property_changed_signal(*this)
	{
		m_user_property_changed_signal.connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_valid.make_slot()));

		m_valid.set_update_slot(sigc::mem_fun(*this, &valid_meshes::execute));
	}

	void execute(const std::vector<k3d::ihint*>& Hints, k3d::bool_t& Output)
	{
		Output = true;

		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::mesh*))
			{
				if(const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(property)))
				{
					try
					{
						k3d::require_valid_points(*mesh);
					}
					catch(std::exception& e)
					{
						k3d::log() << error << e.what() << std::endl;
						Output = false;
						return;
					}

					for(k3d::mesh::primitives_t::const_iterator primitive = mesh->primitives.begin(); primitive != mesh->primitives.end(); ++primitive)
					{
						boost::scoped_ptr<k3d::bezier_triangle_patch::const_primitive> bezier_triangle_patch(k3d::bezier_triangle_patch::validate(*mesh, **primitive));
						if(bezier_triangle_patch)
							continue;

						boost::scoped_ptr<k3d::bicubic_patch::const_primitive> bicubic_patch(k3d::bicubic_patch::validate(*mesh, **primitive));
						if(bicubic_patch)
							continue;

						boost::scoped_ptr<k3d::bilinear_patch::const_primitive> bilinear_patch(k3d::bilinear_patch::validate(*mesh, **primitive));
						if(bilinear_patch)
							continue;

						boost::scoped_ptr<k3d::blobby::const_primitive> blobby(k3d::blobby::validate(*mesh, **primitive));
						if(blobby)
							continue;

						boost::scoped_ptr<k3d::cone::const_primitive> cone(k3d::cone::validate(*mesh, **primitive));
						if(cone)
							continue;

						boost::scoped_ptr<k3d::cubic_curve::const_primitive> cubic_curve(k3d::cubic_curve::validate(*mesh, **primitive));
						if(cubic_curve)
							continue;

						boost::scoped_ptr<k3d::cylinder::const_primitive> cylinder(k3d::cylinder::validate(*mesh, **primitive));
						if(cylinder)
							continue;

						boost::scoped_ptr<k3d::disk::const_primitive> disk(k3d::disk::validate(*mesh, **primitive));
						if(disk)
							continue;

						boost::scoped_ptr<k3d::hyperboloid::const_primitive> hyperboloid(k3d::hyperboloid::validate(*mesh, **primitive));
						if(hyperboloid)
							continue;

						boost::scoped_ptr<k3d::linear_curve::const_primitive> linear_curve(k3d::linear_curve::validate(*mesh, **primitive));
						if(linear_curve)
							continue;

						boost::scoped_ptr<k3d::nurbs_curve::const_primitive> nurbs_curve(k3d::nurbs_curve::validate(*mesh, **primitive));
						if(nurbs_curve)
							continue;

						boost::scoped_ptr<k3d::nurbs_patch::const_primitive> nurbs_patch(k3d::nurbs_patch::validate(*mesh, **primitive));
						if(nurbs_patch)
							continue;

						boost::scoped_ptr<k3d::paraboloid::const_primitive> paraboloid(k3d::paraboloid::validate(*mesh, **primitive));
						if(paraboloid)
							continue;

						boost::scoped_ptr<k3d::particle::const_primitive> particle(k3d::particle::validate(*mesh, **primitive));
						if(particle)
							continue;

						boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(*mesh, **primitive));
						if(polyhedron)
							continue;

						boost::scoped_ptr<k3d::sphere::const_primitive> sphere(k3d::sphere::validate(*mesh, **primitive));
						if(sphere)
							continue;

						boost::scoped_ptr<k3d::teapot::const_primitive> teapot(k3d::teapot::validate(*mesh, **primitive));
						if(teapot)
							continue;

						boost::scoped_ptr<k3d::torus::const_primitive> torus(k3d::torus::validate(*mesh, **primitive));
						if(torus)
							continue;

						Output = false;
						return;
					}
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<valid_meshes >
			factory(
				k3d::uuid(0x55617239, 0x914e0fcf, 0x0d0f9784, 0x899546a4),
				"ValidMeshes",
				_("Tests to see that every every primitive and mesh in a collection of meshes can be validated."),
				"Test",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::bool_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_valid;
	k3d::user_property_changed_signal m_user_property_changed_signal;
};

/////////////////////////////////////////////////////////////////////////////
// valid_meshes_factory

k3d::iplugin_factory& valid_meshes_factory()
{
	return valid_meshes::get_factory();
}

} // namespace test

} // namespace module

