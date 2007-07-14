// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
		\brief Nodes representing the actual interpolation plugins
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/tokens.h>
#include <k3dsdk/vectors.h>

#include "interpolator.h"
#include <k3dsdk/types.h>

namespace libk3danimation
{

class interpolator_double_matrix4_linear :
	public linear_interpolator<double, k3d::matrix4>
{
	typedef linear_interpolator<double, k3d::matrix4> base;
public:
	interpolator_double_matrix4_linear(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document) {}
	
	static k3d::iplugin_factory& get_factory()
	{
		k3d::register_type(typeid(interpolator<double, k3d::matrix4>), "libk3danimation::interpolator<double, k3d::matrix4>");
		static k3d::document_plugin_factory<interpolator_double_matrix4_linear, k3d::interface_list<interpolator<double, k3d::matrix4> > >factory(
				k3d::uuid(0x2df4d81e, 0xc5499561, 0x68c25fa8, 0x34e77dc4),
				"InterpolatorDoubleMatrix4Linear",
				("Linearly interpolates matrix4 values to a double time source"),
				"Animation",
				k3d::iplugin_factory::EXPERIMENTAL);
		return factory;
	}
};

k3d::iplugin_factory& interpolator_double_matrix4_linear_factory()
{
	return interpolator_double_matrix4_linear::get_factory();
}

class interpolator_double_double_linear :
	public linear_interpolator<double, double>
{
	typedef linear_interpolator<double, double> base;
public:
	interpolator_double_double_linear(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document) {}
	
	static k3d::iplugin_factory& get_factory()
	{
		k3d::register_type(typeid(interpolator<double, double>), "libk3danimation::interpolator<double, double>");
		static k3d::document_plugin_factory<interpolator_double_double_linear, k3d::interface_list<interpolator<double, double> > >factory(
				k3d::uuid(0x1aca4dea, 0x8c44f5c1, 0x5f21b9b4, 0x6ec0bbce),
				"InterpolatorDoubleDoubleLinear",
				("Linearly interpolates double values to a double time source"),
				"Animation",
				k3d::iplugin_factory::EXPERIMENTAL);
		return factory;
	}
};

k3d::iplugin_factory& interpolator_double_double_linear_factory()
{
	return interpolator_double_double_linear::get_factory();
}

}
