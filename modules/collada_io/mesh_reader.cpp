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
	\author Carlos Andres Dominguez Caballero (carlosadc@gmail.com)
	\author Barbiero Mattia
*/

#include "int_elements.h"
#include "integration.h"
#include "collada.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_reader.h>
#include <k3dsdk/node.h>
#include <k3dsdk/path.h>

#include <dae.h>
#include <dom/domCOLLADA.h>
#include <dom/domConstants.h>

namespace module
{

namespace collada
{

namespace io
{



/////////////////////////////////////////////////////////////////////////////
// mesh_reader

class mesh_reader :
	public k3d::mesh_reader<k3d::node >
{
	typedef k3d::mesh_reader<k3d::node > base;

public:
	mesh_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) : base(Factory, Document),
		m_geom(init_owner(*this) + init_name("geom") + init_label(_("Geometry")) + init_description(_("Geometry index from file to extract mesh")) + init_value(0) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_geom.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_reload_mesh_slot()));
	}

	void on_load_mesh(const k3d::filesystem::path& Path, k3d::mesh& Output)
	{
		Output = k3d::mesh();

		// Instantiate the reference implementation
		DAE dae;

		domCOLLADA* root = dae.open(Path.native_filesystem_string());
		if(!root) {
            		k3d::log() << error << k3d_file_reference << ": error opening [" << Path.native_console_string() << "]" << std::endl;
			return;
		}
		// Do the conversion. The conversion process throws an exception on error, so
		// we'll include a try/catch handler.
		//daeParser dae_file(*root, Output);

		//Add all possible Geometries to pipeline
		domLibrary_geometries_Array library_geometries = root->getLibrary_geometries_array();

		if(library_geometries.getCount()==0)
			return;

		domGeometry_Array geometries = library_geometries[0]->getGeometry_array();

		int cur_geom = m_geom.pipeline_value();
		if(cur_geom >= geometries.getCount())
		{
			k3d::log() << error << "There aren't that many geometries in file!" << std::endl;
			return;
		}

		intGeometry geom = intGeometry(*geometries[cur_geom], k3d::identity3());
		Output = geom.getMesh();

		// destroy the objects we created during the conversion process
		freeConversionObjects<Node, domNode>(dae);
		freeConversionObjects<intGeometry, domGeometry>(dae);
		//freeConversionObjects<Material, domMaterial>(dae);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_reader,k3d::interface_list<k3d::imesh_source,k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0x4d603851, 0x0441b8b5, 0x294d5a8c, 0x95478016),
			"COLLADAMeshReader",
			_("Mesh reader that loads external COLLADA (.dae) files into the document by reference"),
			"MeshReader",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(int, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, with_serialization) m_geom;
};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader::get_factory();
}

} // namespace io

} // namespace collada

} // namespace module

