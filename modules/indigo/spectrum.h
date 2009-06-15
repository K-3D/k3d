#ifndef MODULES_INDIGO_SPECTRUM_H
#define MODULES_INDIGO_SPECTRUM_H

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

#include <k3d-i18n-config.h>
#include <k3dsdk/data.h>

namespace module
{

namespace indigo
{

/////////////////////////////////////////////////////////////////////////////
// spectrum

/// Provides property storage and rendering for an Indigo "spectrum" tag
class spectrum
{
public:
  template<typename OwnerT>
  spectrum(OwnerT* Owner) : 
    m_blackbody_temperature(init_owner(*Owner) + init_name("blackbody_temperature") + init_label(_("Blackbody Temperature")) + init_description(_("Blackbody Temperature of the light source in degrees kelvin.")) + init_value(6500)), 
    m_blackbody_gain(init_owner(*Owner) + init_name("blackbody_gain") + init_label(_("Blackbody Gain")) + init_description(_("Blackbody Temperature Gain, multiplier.")) + init_value(1.0))
  {
  }


  void setup(const k3d::string_t& ElementName, std::ostream& Stream);

private:
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_blackbody_temperature;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_blackbody_gain;
};

} // namespace indigo

} // namespace module

#endif // !MODULES_INDIGO_SPECTRUM_H

