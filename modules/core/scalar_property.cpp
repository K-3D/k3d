// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief An object that acts as a property source for scalar properties
		\author Bart Janssens (bart.janssens@polytechnic.be)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/measurement.h>

namespace libk3dcore
{

class scalar_property :
	public k3d::persistent<k3d::node>
{
	typedef k3d::persistent<k3d::node> base;

public:
	scalar_property(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_value(init_owner(*this) + init_name("value") + init_label(_("Output value")) + init_description(_("Outputs the value exposed by this property")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)))
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<scalar_property > factory(
			k3d::uuid(0x35b28760,0x0547458f,0xb4e3d324,0x84ae3545),
			"ScalarProperty",
			_("Provides a source object to link scalar properties to"),
			"Scalar",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	// The scalar property itself:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_value;
};

k3d::iplugin_factory& scalar_property_factory()
{
	return scalar_property::get_factory();
}

} //namespace libk3dcore


