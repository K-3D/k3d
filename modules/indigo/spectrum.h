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
		m_type(init_owner(*Owner) + init_name("type") + init_label(_("Spectrum Type")) + init_description(_("Spectrum type.")) + init_value(RGB) + init_enumeration(type_values())),
    m_blackbody_temperature(init_owner(*Owner) + init_name("blackbody_temperature") + init_label(_("Blackbody Temperature")) + init_description(_("Blackbody Temperature of the light source in degrees kelvin.")) + init_value(6500)), 
    m_blackbody_gain(init_owner(*Owner) + init_name("blackbody_gain") + init_label(_("Blackbody Gain")) + init_description(_("Blackbody Temperature Gain, multiplier.")) + init_value(1.0)),
    m_red(init_owner(*Owner) + init_name("red") + init_label(_("Red")) + init_description(_("Red")) + init_value(1.0) + init_constraint(k3d::data::constraint::minimum<k3d::double_t>(0.0))),
    m_green(init_owner(*Owner) + init_name("green") + init_label(_("Green")) + init_description(_("Green")) + init_value(1.0) + init_constraint(k3d::data::constraint::minimum<k3d::double_t>(0.0))),
    m_blue(init_owner(*Owner) + init_name("blue") + init_label(_("Blue")) + init_description(_("Blue")) + init_value(1.0) + init_constraint(k3d::data::constraint::minimum<k3d::double_t>(0.0))),
    m_gamma(init_owner(*Owner) + init_name("gamma") + init_label(_("Gamma")) + init_description(_("Gamma")) + init_value(2.2))
  {
  }

  void setup(const k3d::string_t& ElementName, std::ostream& Stream);

private:
	enum type
	{
		BLACKBODY,
		RGB,
	};

	friend std::ostream& operator << (std::ostream& Stream, const type& Value);
	friend std::istream& operator >> (std::istream& Stream, type& Value);
	static const k3d::ienumeration_property::enumeration_values_t& type_values();

	k3d_data(type, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_blackbody_temperature;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_blackbody_gain;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_red;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_green;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_blue;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_gamma;
};

} // namespace indigo

} // namespace module

#endif // !MODULES_INDIGO_SPECTRUM_H

