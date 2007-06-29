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
		\brief An object that converts a scalar to a string using printf() style formatting
		\author Timothy M. Shead (tshead@k-3d.com)
*/
#include <k3dsdk/document_plugin_factory.h>

#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <boost/format.hpp>

namespace libk3dcore
{

class format_scalar :
	public k3d::persistent<k3d::node>
{
	typedef k3d::persistent<k3d::node> base;
public:
	format_scalar(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input(init_owner(*this) + init_name("input") + init_label(_("Input")) + init_description(_("Float value")) + init_value(0.0)),
		m_format(init_owner(*this) + init_name("format") + init_label(_("Format")) + init_description(_("Format string, printf() like")) + init_value(std::string("%f"))),
		m_output(init_owner(*this) + init_name("output") + init_label(_("Output text")) + init_description(_("Output string (read only)")) + init_slot(sigc::mem_fun(*this, &format_scalar::get_value)))
	{
		m_input.changed_signal().connect(m_output.make_reset_slot());
		m_format.changed_signal().connect(m_output.make_reset_slot());
	}

	std::string get_value()
	{
		try
			{
				return (boost::format(m_format.value()) % m_input.value()).str();
			}
		catch(std::exception& e)
			{
				k3d::log() << error << e.what() << std::endl;
				return std::string();
			}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<format_scalar > factory(
			k3d::uuid(0x3af7d777, 0x691d41b0, 0xaa801d59, 0x7ee4180e),
			"FormatScalar",
			_("Converts a scalar to a string using printf() style formatting"),
			"Scalar",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_format;
	k3d_data(std::string, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_output;
};

k3d::iplugin_factory& format_scalar_factory()
{
	return format_scalar::get_factory();
}

} //namespace libk3dcore


