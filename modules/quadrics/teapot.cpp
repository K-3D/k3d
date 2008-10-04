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
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/teapot.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace quadrics
{

/////////////////////////////////////////////////////////////////////////////
// teapot

class teapot :
	public k3d::material_sink<k3d::mesh_source<k3d::node> >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node> > base;

public:
	teapot(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Controls the color of the output teapot.")) + init_value(k3d::color(1, 1, 1))),
		m_transformation(init_owner(*this) + init_name("transformation") + init_label(_("Transformation")) + init_description(_("Transformation matrix used to position / orient / scale the output teapot.")) + init_value(k3d::identity3D()))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_color.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_transformation.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		boost::scoped_ptr<k3d::teapot::primitive> primitive(k3d::teapot::create(Output));
		k3d::typed_array<k3d::color>& colors = primitive->uniform_data.create<k3d::typed_array<k3d::color> >("Cs");

		primitive->matrices.push_back(m_transformation.pipeline_value());
		primitive->materials.push_back(m_material.pipeline_value());
		colors.push_back(m_color.pipeline_value());
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<teapot > factory(
			k3d::uuid(0xf0df2bd5, 0xd94e10ca, 0x1883899e, 0xd39d0c29),
			"Teapot",
			"Creates a classic teapot primitive",
			"Quadric",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_transformation;
};

k3d::iplugin_factory& teapot_factory()
{
	return teapot::get_factory();
}

} // namespace quadrics

} // namespace module

