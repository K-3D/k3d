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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "scalar_source.h"

#include <k3d-i18n-config.h>

namespace module
{

namespace scalar
{

scalar_source::scalar_source(k3d::iplugin_factory& Factory, k3d::idocument& Document, const char* const OutputDescription) :
	base(Factory, Document),
	m_output(init_owner(*this) + init_name("output") + init_label(_("Output")) + init_description(OutputDescription) + init_value(0.0))
{
	m_output.set_update_slot(sigc::mem_fun(*this, &scalar_source::execute));
}

sigc::slot<void, k3d::ihint*> scalar_source::make_update_value_slot()
{
	return m_output.make_slot();
}

void scalar_source::execute(const std::vector<k3d::ihint*>& Hints, k3d::double_t& Output)
{
	on_update_value(Output);
}

void scalar_source::on_update_value(k3d::double_t& Output)
{
	Output = 0.0;
}

} // namespace scalar

} // namespace module


