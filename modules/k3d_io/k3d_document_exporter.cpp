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
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/dependencies.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_exporter.h>
#include <k3dsdk/ifile_format.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/ipersistent.h>
#include <k3dsdk/istate_recorder.h>
#include <k3dsdk/persistent_lookup.h>
#include <k3dsdk/property.h>
#include <k3dsdk/result.h>
#include <k3dsdk/serialization_xml.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/xml.h>

namespace libk3dk3dio
{

/////////////////////////////////////////////////////////////////////////////
// k3d_document_exporter

/// Serializes a K-3D document using the native K-3D XML format
class k3d_document_exporter :
	public k3d::ifile_format,
	public k3d::idocument_exporter
{
public:
	bool write_file(k3d::idocument& Document, const k3d::filesystem::path& Path)
	{
		k3d::log() << info << "Writing " << Path.native_console_string() << " using " << get_factory().name() << std::endl;

		// Try to open the file ...
		k3d::filesystem::ofstream filestream(Path);
		return_val_if_fail(filestream.good(), false);

		// Record our new path & title ...
		return_val_if_fail(k3d::property::set_internal_value(Document.path(), Path), false);
		return_val_if_fail(k3d::property::set_internal_value(Document.title(), Path.leaf()), false);

		// Create our output document and dependencies objects ...
		k3d::xml::element xml(
			"k3dml",
			k3d::xml::attribute("package", K3D_PACKAGE),
			k3d::xml::attribute("version", K3D_VERSION),
			k3d::xml::attribute("host", K3D_HOST));

		const k3d::filesystem::path root_path = Path.branch_path();
		k3d::dependencies dependencies;
		k3d::persistent_lookup lookup;
		k3d::ipersistent::save_context context(root_path, dependencies, lookup);

		// Save per-document data ...
		k3d::xml::element& xml_document = xml.append(k3d::xml::element("document"));
		k3d::xml::element& xml_nodes = xml_document.append(k3d::xml::element("nodes"));

		// Sort objects by ID before saving them ...
		const k3d::inode_collection::nodes_t nodes = Document.nodes().collection();

		for(k3d::inode_collection::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
			k3d::xml::save(**node, xml_nodes, context);

		// Save the DAG ...
		k3d::xml::save_pipeline(Document, xml_document, context);

		// Save the XML ...
		filestream << k3d::xml::declaration() << xml << std::endl;
		return_val_if_fail(filestream.good(), false);

		// Record the saved position in the undo/redo hierarchy
		Document.state_recorder().mark_saved();

		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<k3d_document_exporter, k3d::interface_list<k3d::idocument_exporter> > factory(
			k3d::classes::DocumentExporter(),
			"K3DDocumentExporter",
			_("K-3D Native ( .k3d )"),
			"");

		return factory;
	}
};

k3d::iplugin_factory& k3d_document_exporter_factory()
{
	return k3d_document_exporter::get_factory();
}

} // namespace libk3dk3dio

