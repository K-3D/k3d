// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>

namespace module
{

namespace test
{

/////////////////////////////////////////////////////////////////////////////
// selection_to_stdout

class selection_to_stdout :
	public k3d::mesh_selection_sink<k3d::node>
{
	typedef k3d::mesh_selection_sink<k3d::node> base;

public:
	selection_to_stdout(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
		m_mesh_selection.changed_signal().connect(sigc::mem_fun(*this, &selection_to_stdout::selection_changed));
	}

	void selection_changed(iunknown* const Hint)
	{
		std::cout << m_mesh_selection.pipeline_value() << std::endl;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<selection_to_stdout,
			k3d::interface_list<k3d::inode> > factory(
				k3d::uuid(0x51739d44, 0xc0422e83, 0x6b50ccba, 0x97cad438),
				"SelectionToStdout",
				_("Prints selection information to stdout"),
				"Test",
				k3d::iplugin_factory::STABLE);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// selection_to_stdout_factory

k3d::iplugin_factory& selection_to_stdout_factory()
{
	return selection_to_stdout::get_factory();
}

} // namespace test

} // namespace module

