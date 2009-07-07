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

#include "texture2.h"
#include "utility.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/iomanip.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// mapping_values

static const k3d::ilist_property<std::string>::values_t& mapping_values()
{
	static k3d::ilist_property<std::string>::values_t values;
	if(values.empty())
	{
		values.push_back("uv");
		values.push_back("spherical");
		values.push_back("cylindrical");
		values.push_back("planar");
	}
	return values;
}

/////////////////////////////////////////////////////////////////////////////
// texture2

texture2::texture2(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
	base(Factory, Document),
	m_mapping(init_owner(*this) + init_name("mapping") + init_label(_("Mapping")) + init_description(_("Projection mapping.")) + init_value(k3d::string_t("uv")) + init_values(mapping_values())),
	m_u_scale(init_owner(*this) + init_name("u_scale") + init_label(_("U Scale")) + init_description(_("U Scale.")) + init_value(1.0)),
	m_v_scale(init_owner(*this) + init_name("v_scale") + init_label(_("V Scale")) + init_description(_("V Scale.")) + init_value(1.0)),
	m_u_delta(init_owner(*this) + init_name("u_delta") + init_label(_("U Delta")) + init_description(_("U Delta.")) + init_value(0.0)),
	m_v_delta(init_owner(*this) + init_name("v_delta") + init_label(_("V Delta")) + init_description(_("V Delta.")) + init_value(0.0)),
	m_u_axis(init_owner(*this) + init_name("u_axis") + init_label(_("U Axis")) + init_description(_("Specify the U axis for planar mapping.")) + init_value(k3d::vector3(1, 0, 0))),
	m_v_axis(init_owner(*this) + init_name("v_axis") + init_label(_("V Axis")) + init_description(_("Specify the V axis for planar mapping.")) + init_value(k3d::vector3(0, 1, 0)))
{
}

void texture2::setup_texture2(std::ostream& Stream)
{
	const k3d::string_t mapping = m_mapping.pipeline_value();

	Stream << k3d::standard_indent << "\"string mapping\" \"" << mapping << "\"\n";

	if(mapping == "uv")
	{
		Stream << k3d::standard_indent << "\"float uscale\" [" << m_u_scale.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "\"float vscale\" [" << m_v_scale.pipeline_value() << "]\n";
	}

	if(mapping == "uv" || mapping == "planar")
	{
		Stream << k3d::standard_indent << "\"float udelta\" [" << m_u_delta.pipeline_value() << "]\n";
		Stream << k3d::standard_indent << "\"float vdelta\" [" << m_v_delta.pipeline_value() << "]\n";
	}

	if(mapping == "planar")
	{
		Stream << k3d::standard_indent << "\"vector v1\" [" << convert(m_u_axis.pipeline_value()) << "]\n";
		Stream << k3d::standard_indent << "\"vector v2\" [" << convert(m_v_axis.pipeline_value()) << "]\n";
	}
}

} // namespace luxrender

} // namespace module

