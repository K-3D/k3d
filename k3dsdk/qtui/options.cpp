// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3dsdk/qtui/options.h>

#include <k3dsdk/options.h>
#include <k3dsdk/xml.h>

namespace k3d
{

namespace qtui
{

namespace options
{

static xml::element& qtui_element()
{
	return k3d::options::tree().safe_element("qtui");
}

static xml::element& nags_element()
{
	return qtui_element().safe_element("nags");
}

static xml::element& nag_element(const std::string& Type)
{
	const xml::element match("nag", xml::attribute("message", Type));
	const xml::element prototype("nag", string_cast(true), xml::attribute("message", Type));
	return nags_element().safe_element(match, prototype);
}

const bool_t nag(const string_t& Type)
{
	return from_string<bool_t>(nag_element(Type).text, true);
}

void enable_nag(const string_t& Type, const bool_t Enabled)
{
	nag_element(Type).text = string_cast(Enabled);
}

} // namespace options

} // namespace qtui

} // namespace k3d

