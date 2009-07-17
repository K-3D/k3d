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

#include "light.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/iomanip.h>

namespace module
{

namespace luxrender
{

/////////////////////////////////////////////////////////////////////////////
// light

light::light(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
	k3d::node(Factory, Document),
	m_group(init_owner(*this) + init_name("group") + init_label(_("Light Group")) + init_description(_("Light group.")) + init_value(k3d::string_t("default")))
{
}

void light::setup(std::ostream& Stream)
{
	Stream << k3d::standard_indent << "AttributeBegin\n" << k3d::push_indent;
	Stream << k3d::standard_indent << "LightGroup \"" << m_group.pipeline_value() << "\"\n";

	on_setup(Stream);

	Stream << k3d::pop_indent << k3d::standard_indent << "AttributeEnd\n";
}

} // namespace luxrender

} // namespace module

