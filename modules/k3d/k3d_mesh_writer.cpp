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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/dependencies.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/new_mesh_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/persistent_lookup.h>
#include <k3dsdk/serialization.h>
#include <k3dsdk/version.h>

#include <k3dsdk/fstream.h>

namespace libk3dk3d
{

/////////////////////////////////////////////////////////////////////////////
// k3d_mesh_writer

class k3d_mesh_writer :
	public k3d::dev::mesh_sink<k3d::persistent<k3d::node> >
{
	typedef k3d::dev::mesh_sink<k3d::persistent<k3d::node> > base;

public:
	k3d_mesh_writer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Output file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::WRITE) + init_path_type("k3d_files"))
	{
		m_file.changed_signal().connect(sigc::mem_fun(*this, &k3d_mesh_writer::on_write_file));
		m_input_mesh.changed_signal().connect(sigc::mem_fun(*this, &k3d_mesh_writer::on_write_file));
	}

	void on_write_file(k3d::iunknown*)
	{
		const k3d::filesystem::path path = m_file.value();
		k3d::dev::mesh* const mesh = m_input_mesh.value();

		if(!mesh || path.empty())
			return;

		k3d::log() << info << "Writing .k3d file: " << path.native_console_string() << std::endl;
		k3d::filesystem::ofstream file(path);
		if(!file)
		{
			k3d::log() << error << k3d_file_reference << ": error opening [" << path.native_console_string() << "]" << std::endl;
			return;
		}

		const k3d::filesystem::path root_path(path.branch_path());
		k3d::dependencies dependencies;
		k3d::persistent_lookup lookup;
		k3d::ipersistent::save_context context(root_path, dependencies, lookup);

		k3d::xml::element xml("k3dml");
		k3d::xml::element& xml_mesh = xml.append(k3d::xml::element("mesh_arrays"));
		k3d::save_mesh(*mesh, xml_mesh, context);

		file << k3d::xml::declaration() <<  xml;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<k3d_mesh_writer, k3d::interface_list<k3d::imesh_sink > > factory(
			k3d::uuid(0xefdcb347, 0x0e984075, 0x87973a80, 0xca8b196d),
			"K3DMeshWriter",
			_("Mesh writer that saves K-3D XML (.k3d) files"),
			"MeshWriter",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

k3d::iplugin_factory& k3d_mesh_writer_factory()
{
	return k3d_mesh_writer::get_factory();
}

} // namespace libk3dk3d

