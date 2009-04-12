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

#include <boost/format.hpp>

namespace module
{

namespace core
{

class print_stdout :
	public k3d::node
{
	typedef k3d::node base;
public:
	print_stdout(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input(init_owner(*this) + init_name("input") + init_label(_("Input")) + init_description(_("Prints input string to console's stdout")) + init_value(std::string("")))
	{
		m_input.changed_signal().connect(sigc::mem_fun(*this, &print_stdout::on_input_changed));
	}

	void on_input_changed(k3d::iunknown*)
	{
		std::cout << m_input.pipeline_value() << std::endl;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<print_stdout > factory(
			k3d::uuid(0x9fdbe940, 0x7f394ad7, 0x8654d53c, 0xe33b7a9a),
			"PrintStdout",
			_("Prints an input string value to stdout"),
			"Scalar",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input;
};

k3d::iplugin_factory& print_stdout_factory()
{
	return print_stdout::get_factory();
}

} //namespace core

} // namespace module


