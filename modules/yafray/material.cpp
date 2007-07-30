// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\author Romain Behar <romainbehar@yahoo.com>
*/

#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/iyafray.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property_group_collection.h>
#include <k3dsdk/vectors.h>

namespace libk3dyafray
{

/////////////////////////////////////////////////////////////////////////////
// material

class material :
	public k3d::persistent<k3d::node> ,
	public k3d::imaterial,
	public k3d::yafray::imaterial,
	public k3d::property_group_collection
{
	typedef k3d::persistent<k3d::node>  base;

public:
	material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_color(init_owner(*this) + init_name("color") + init_label(_("color")) + init_description(_("Color")) + init_value(k3d::color(1, 1, 1))),
		m_specular_color(init_owner(*this) + init_name("specular_color") + init_label(_("specular_color")) + init_description(_("Specular Color")) + init_value(k3d::color(1, 1, 1))),
		m_reflected_color(init_owner(*this) + init_name("reflected_color") + init_label(_("reflected_color")) + init_description(_("Reflected Color")) + init_value(k3d::color(0, 0, 0))),
		m_transmitted_color(init_owner(*this) + init_name("transmitted_color") + init_label(_("transmitted_color")) + init_description(_("Transmitted Color")) + init_value(k3d::color(0, 0, 0))),
		m_hardness(init_owner(*this) + init_name("hardness") + init_label(_("hardness")) + init_description(_("Hardness")) + init_value(10.0)),
		m_index_of_refraction(init_owner(*this) + init_name("index_of_refraction") + init_label(_("index_of_refraction")) + init_description(_("Index of Refraction")) + init_value(1.0)),
		m_minimum_reflection(init_owner(*this) + init_name("minimum_reflection") + init_label(_("minimum_reflection")) + init_description(_("Minimum Reflection")) + init_value(0.0)),
		m_fast_fresnel(init_owner(*this) + init_name("fast_fresnel") + init_label(_("fast_fresnel")) + init_description(_("Fast fresnel")) + init_value(false)),

		m_shadow(init_owner(*this) + init_name("shadow") + init_label(_("shadow")) + init_description(_("Shadow")) + init_value(true)),
		m_emit_rad(init_owner(*this) + init_name("emit_rad") + init_label(_("emit_rad")) + init_description(_("emit_rad")) + init_value(true)),
		m_recv_rad(init_owner(*this) + init_name("recv_rad") + init_label(_("recv_rad")) + init_description(_("recv_rad")) + init_value(true)),
		m_caustics(init_owner(*this) + init_name("caustics") + init_label(_("caustics")) + init_description(_("Caustics")) + init_value(false)),
		m_caus_IOR(init_owner(*this) + init_name("caus_IOR") + init_label(_("caus_IOR")) + init_description(_("caus_IOR")) + init_value(1.0)),
		m_caus_rcolor(init_owner(*this) + init_name("caus_rcolor") + init_label(_("caus_rcolor")) + init_description(_("caus_rcolor")) + init_value(k3d::color(0, 0, 0))),
		m_caus_tcolor(init_owner(*this) + init_name("caus_tcolor") + init_label(_("caus_tcolor")) + init_description(_("caus_tcolor")) + init_value(k3d::color(0, 0, 0))),

		m_mesh_autosmooth(init_owner(*this) + init_name("mesh_autosmooth") + init_label(_("mesh_autosmooth")) + init_description(_("Mesh autosmooth")) + init_value(false)),
		m_mesh_autosmooth_value(init_owner(*this) + init_name("mesh_autosmooth_value") + init_label(_("mesh_autosmooth_value")) + init_description(_("Mesh autosmooth value")) + init_value(60.0)),
		m_has_orco(init_owner(*this) + init_name("has_orco") + init_label(_("has_orco")) + init_description(_("Has orco")) + init_value(false))
	{
		k3d::iproperty_group_collection::group object_attributes("Object Attributes");
		object_attributes.properties.push_back(&static_cast<k3d::iproperty&>(m_shadow));
		object_attributes.properties.push_back(&static_cast<k3d::iproperty&>(m_emit_rad));
		object_attributes.properties.push_back(&static_cast<k3d::iproperty&>(m_recv_rad));
		object_attributes.properties.push_back(&static_cast<k3d::iproperty&>(m_caustics));
		object_attributes.properties.push_back(&static_cast<k3d::iproperty&>(m_caus_IOR));
		object_attributes.properties.push_back(&static_cast<k3d::iproperty&>(m_caus_rcolor));
		object_attributes.properties.push_back(&static_cast<k3d::iproperty&>(m_caus_tcolor));

		k3d::iproperty_group_collection::group mesh_attributes("Mesh Attributes");
		mesh_attributes.properties.push_back(&static_cast<k3d::iproperty&>(m_mesh_autosmooth));
		mesh_attributes.properties.push_back(&static_cast<k3d::iproperty&>(m_mesh_autosmooth_value));
		mesh_attributes.properties.push_back(&static_cast<k3d::iproperty&>(m_has_orco));

		register_property_group(object_attributes);
		register_property_group(mesh_attributes);
	}

	k3d::gl::imaterial* gl_material()
	{
		return 0;
	}

	k3d::ri::imaterial* ri_material()
	{
		return 0;
	}

	k3d::yafray::imaterial* yafray_material()
	{
		return this;
	}

	void setup_material(std::ostream& Stream)
	{
		const k3d::color color = m_color.pipeline_value();
		const k3d::color specular_color = m_specular_color.pipeline_value();
		const k3d::color reflected_color = m_reflected_color.pipeline_value();
		const k3d::color transmitted_color = m_transmitted_color.pipeline_value();

		const double hardness = m_hardness.pipeline_value();
		const double index_of_refraction = m_index_of_refraction.pipeline_value();
		const double minimum_reflection = m_minimum_reflection.pipeline_value();

		Stream << "<shader type=\"generic\" name=\"" << name() << "\">" << std::endl;
		Stream << "	<attributes>" << std::endl;
		Stream << "		<color r=\"" << color.red << "\" g=\"" << color.green << "\" b=\"" << color.blue << "\"/>" << std::endl;
		Stream << "		<specular r=\"" << specular_color.red << "\" g=\"" << specular_color.green << "\" b=\"" << specular_color.blue << "\"/>" << std::endl;
		Stream << "		<reflected r=\"" << reflected_color.red << "\" g=\"" << reflected_color.green << "\" b=\"" << reflected_color.blue << "\"/>" << std::endl;
		Stream << "		<transmitted r=\"" << transmitted_color.red << "\" g=\"" << transmitted_color.green << "\" b=\"" << transmitted_color.blue << "\"/>" << std::endl;
		Stream << "		<hard value=\"" << hardness << "\"/>" << std::endl;
		Stream << "		<IOR value=\"" << index_of_refraction << "\"/>" << std::endl;
		Stream << "		<min_refle value=\"" << minimum_reflection << "\"/>" << std::endl;
		Stream << "		<fast_fresnel value=\"" << (m_fast_fresnel.pipeline_value() ? "on" : "off") << "\"/>" << std::endl;
		Stream << "	</attributes>" << std::endl;
		Stream << "</shader>" << std::endl;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<material,
				k3d::interface_list<k3d::imaterial,
				k3d::interface_list<k3d::yafray::imaterial> > > factory(
			k3d::uuid(0x4b767ac5, 0x19ec4182, 0x9883cc81, 0x3f091dea),
			"YafrayMaterial",
			_("Yafray Material"),
			"Yafray Materials",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_specular_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_reflected_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_transmitted_color;

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_hardness;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_index_of_refraction;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_minimum_reflection;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_fast_fresnel;

	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_shadow;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_emit_rad;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_recv_rad;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_caustics;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_caus_IOR;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_caus_rcolor;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_caus_tcolor;

	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_mesh_autosmooth;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_mesh_autosmooth_value;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_has_orco;
};

k3d::iplugin_factory& material_factory()
{
	return material::get_factory();
}

} // namespace libk3dyafray


