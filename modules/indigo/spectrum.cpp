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

#include "spectrum.h"

#include <k3dsdk/iomanip.h>

namespace module
{

namespace indigo
{

/////////////////////////////////////////////////////////////////////////////
// spectrum

void spectrum::setup(const k3d::string_t& ElementName, std::ostream& Stream)
{
  Stream << k3d::standard_indent << "<" << ElementName << ">\n" << k3d::push_indent;
  Stream << k3d::standard_indent << "<blackbody>\n" << k3d::push_indent;
  Stream << k3d::standard_indent << "<temperature>" << m_blackbody_temperature.pipeline_value() << "</temperature>\n";
  Stream << k3d::standard_indent << "<gain>" << m_blackbody_gain.pipeline_value() << "</gain>\n";
  Stream << k3d::pop_indent << k3d::standard_indent << "</blackbody>\n";
  Stream << k3d::pop_indent << k3d::standard_indent << "</" << ElementName << ">\n";
}

} // namespace indigo

} // namespace module

