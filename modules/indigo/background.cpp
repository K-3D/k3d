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
#include "spectrum.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/node.h>
#include <k3dsdk/vector3.h>

namespace module
{

namespace indigo
{

/////////////////////////////////////////////////////////////////////////////
// background

class background :
	public k3d::node,
	public indigo::light
{
	typedef k3d::node base;

public:
	background(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
    m_spectrum(this)
	{
	}

  void setup(std::ostream& Stream)
  {
    Stream << k3d::standard_indent << "<background>\n" << k3d::push_indent;
    m_spectrum.setup("spectrum", Stream);
    Stream << k3d::pop_indent << k3d::standard_indent << "</background>\n";
  }

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<background> factory(
			k3d::uuid(0x80dbb6b4, 0xbd46a1db, 0x67fc4995, 0x01d98679),
			"IndigoBackground",
			_("Indigo Background"),
			"Indigo Light",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
  spectrum m_spectrum;
};

k3d::iplugin_factory& background_factory()
{
	return background::get_factory();
}

} // namespace indigo

} // namespace module

