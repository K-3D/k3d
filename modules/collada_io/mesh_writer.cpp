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
	\author Tim Shead (tshead@k-3d.com)
	\author Wladyslaw Strugala (fizws@julia.univ.gda.pl)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_sink.h>
#include <k3dsdk/node.h>
#include "collada.h"

namespace module
{

namespace collada
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_writerio

class mesh_writer :
	public k3d::mesh_sink<k3d::node >
{
	typedef k3d::mesh_sink<k3d::node > base;

public:
	mesh_writer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Output file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::WRITE) + init_path_type("obj_files"))
	{
		m_file.changed_signal().connect(sigc::mem_fun(*this, &mesh_writer::on_write_file));
		m_input_mesh.changed_signal().connect(sigc::mem_fun(*this, &mesh_writer::on_write_file));
	}

	void on_write_file(k3d::iunknown*)
	{
		const k3d::filesystem::path path = m_file.pipeline_value();
		k3d::mesh* const mesh = m_input_mesh.pipeline_value();

		if(!mesh || path.empty())
			return;

		k3d::log() << info << "Writing .dae file: " << path.native_console_string() << std::endl;

		// The main file where de mesh will be exported
		DAE dae;
		//Create root branch of the dae xml tree and create the only library we are 
		// going to need, library_geometries where we are going to store the mesh
		daeElement *root = dae.add(path.native_console_string());
		daeElement *library_geometries = root->add("library_geometries");

		// Every dae file needs an asset node for reference
		daeElement *asset = root->add("asset");
		daeElement* contributor = asset->add("contributor");
		daeElement* created = asset->add("created");
		daeElement* modified = asset->add("modified");

		// Fill asset values. Note that the date is hardcoded, this is going to be changed soon
		std::stringstream author;
		author << "K-3D " << K3D_PACKAGE << " , " << K3D_VERSION << " , " << K3D_HOST;
		contributor->setCharData(author.str().c_str());
		const char* date = "2008-04-08T13:07:52-08:00";
		created->setCharData(date);
		modified->setCharData(date);

		// Store the mesh source into library_geometries using the name "K3DGeometry" for reference
		addGeometry(library_geometries, "K3DGeometry", mesh);

		// Setup visual scenes library and the main scene
		SafeAdd(root, "library_visual_scenes", visualSceneLib);
		SafeAdd(visualSceneLib, "visual_scene", visualScene);
		visualScene->setAttribute("id", "mainScene");

		SafeAdd(visualScene, "node", node);
		node->setAttribute("id", "K3DWriter_mesh");
		node->setAttribute("name", "K3DWriter_mesh");

		// Instantiate the <geometry> so it is displayed on the scene
		SafeAdd(node, "instance_geometry", instanceGeom);
		instanceGeom->setAttribute("url", makeUriRef("K3DGeometry").c_str());

		// Add the main scene to root file and instanciate the created visual_scene
		root->add("scene instance_visual_scene")->setAttribute("url", makeUriRef("mainScene").c_str());

		dae.writeAll();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_writer, k3d::interface_list<k3d::imesh_sink > > factory(
			k3d::uuid(0xcf5833ea, 0x264c781c, 0x844512a5, 0xe2270efc),
			"COLLADAMeshWriter",
			_("Mesh writer that saves external COLLADA (.dae) files"),
			"MeshWriter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

k3d::iplugin_factory& mesh_writer_factory()
{
	return mesh_writer::get_factory();
}

}
}
}