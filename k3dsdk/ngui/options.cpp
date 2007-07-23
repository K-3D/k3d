// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Declares k3d::ioptions, an abstract interface for global user options
		\author Tim Shead (tshead@k-3d.com)
*/

#include "options.h"

#include <k3dsdk/options.h>
#include <k3dsdk/xml.h>

namespace libk3dngui
{

namespace options
{

namespace detail
{

k3d::xml::element& ngui_element()
{
	return k3d::options::tree().safe_element("ngui");
}

k3d::xml::element& tutorials_element()
{
	return ngui_element().safe_element("tutorials");
}

k3d::xml::element& tutorial_speed_element()
{
	const k3d::xml::element match("speed");
	const k3d::xml::element prototype("speed", "1.0");
	return tutorials_element().safe_element(match, prototype);
}

k3d::xml::element& nags_element()
{
	return ngui_element().safe_element("nags");
}

k3d::xml::element& nag_element(const std::string& Type)
{
	const k3d::xml::element match("nag", k3d::xml::attribute("message", Type));
	const k3d::xml::element prototype("nag", k3d::string_cast(true), k3d::xml::attribute("message", Type));
	return nags_element().safe_element(match, prototype);
}

} // namespace detail

const double tutorial_speed()
{
	return k3d::from_string<double>(detail::tutorial_speed_element().text, 1.0);
}

void set_tutorial_speed(const double Speed)
{
	detail::tutorial_speed_element().text = k3d::string_cast(Speed);
}

const bool nag(const std::string& Type)
{
	return k3d::from_string<bool>(detail::nag_element(Type).text, true);
}

void enable_nag(const std::string& Type, const bool Enabled)
{
	detail::nag_element(Type).text = k3d::string_cast(Enabled);
}

} // namespace options

} // namespace libk3dngui

