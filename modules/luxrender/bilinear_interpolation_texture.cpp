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

#include "color_texture.h"
#include "texture2.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/measurement.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// bilinear_interpolation_texture

class bilinear_interpolation_texture :
	public texture2,
	public color_texture
{
	typedef texture2 base;

public:
	bilinear_interpolation_texture(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_v00(init_owner(*this) + init_name("v00") + init_label(_("V00")) + init_description(_("V00")) + init_value(k3d::color(1, 0, 0))),
		m_v01(init_owner(*this) + init_name("v01") + init_label(_("V01")) + init_description(_("V01")) + init_value(k3d::color(0, 1, 0))),
		m_v10(init_owner(*this) + init_name("v10") + init_label(_("V10")) + init_description(_("V10")) + init_value(k3d::color(0, 0, 1))),
		m_v11(init_owner(*this) + init_name("v11") + init_label(_("V11")) + init_description(_("V11")) + init_value(k3d::color(1, 1, 1)))
	{
	}

	void on_setup(name_map& TextureNames, const k3d::string_t& Name, std::ostream& Stream)
	{
		Stream << k3d::standard_indent << "Texture \"" << Name << "\" \"color\" \"bilerp\"\n" << k3d::push_indent;
		setup_texture2(Stream);
		Stream << k3d::standard_indent << "\"color v00\" [" << m_v00.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "\"color v01\" [" << m_v01.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "\"color v10\" [" << m_v10.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "\"color v11\" [" << m_v11.pipeline_value() << "]\n";
		Stream << k3d::pop_indent; 
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bilinear_interpolation_texture,
			k3d::interface_list<k3d::itexture> > factory(
			k3d::uuid(0x9df9057c, 0x204e6f80, 0x5768e69b, 0xb5ba08ba),
			"LuxRenderBilinearInterpolationTexture",
			_("LuxRender Bilinear Interpolation Texture"),
			"LuxRender Texture",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_v00;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_v01;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_v10;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_v11;
};

k3d::iplugin_factory& bilinear_interpolation_texture_factory()
{
	return bilinear_interpolation_texture::get_factory();
}

} // namespace luxrender

} // namespace module

