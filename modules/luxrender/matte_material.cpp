// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Tim Shead <tshead@k-3d.com>
*/

#include "material.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// material

class matte_material :
	public k3d::node ,
  public luxrender::material
{
	typedef k3d::node base;

public:
	matte_material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_color(init_owner(*this) + init_name("Kd") + init_label(_("Color")) + init_description(_("Color")) + init_value(k3d::color(0.8, 0.8, 0.8))),
		m_sigma(init_owner(*this) + init_name("sigma") + init_label(_("Sigma")) + init_description(_("The sigma parameter in the Oren-Nayer shader in degrees.  Zero for pure Lambertian reflection.")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle)))
	{
	}

  void setup(std::ostream& Stream)
  {
    Stream << k3d::standard_indent << "Texture \"a\" \"color\" \"constant\" \"color value\" [" << m_color.pipeline_value() << "]\n";
    Stream << k3d::standard_indent << "Texture \"b\" \"float\" \"constant\" \"float value\" [" << k3d::degrees(m_sigma.pipeline_value()) << "]\n";
    Stream << k3d::standard_indent << "Material \"matte\" \"texture Kd\" \"a\" \"texture sigma\" \"b\" \n";
  }

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<matte_material,
				k3d::interface_list<k3d::imaterial> > factory(
			k3d::uuid(0x77bbeecb, 0x59483223, 0x142c92b9, 0x235096bb),
			"LuxRenderMatteMaterial",
			_("LuxRender Matte Material"),
			"LuxRender Material",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_sigma;
};

k3d::iplugin_factory& matte_material_factory()
{
	return matte_material::get_factory();
}

} // namespace luxrender

} // namespace module

