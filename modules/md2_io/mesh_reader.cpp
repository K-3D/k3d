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
*   \author Carlos Andres Dominguez Caballero
*   (carlosadc@gmail.com)
*/

#include "md2.h"
#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gprim_factory.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/xml.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/measurement.h>
#include <stack>

namespace module
{

namespace md2
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_reader_implementation

class mesh_reader_implementation :
	public k3d::mesh_source<k3d::node >,
	public k3d::imesh_storage
{
	typedef k3d::mesh_source<k3d::node > base;

public:
	mesh_reader_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("svg_files")),
		m_frame(init_owner(*this) + init_name("frame") + init_label(_("Frame")) + init_description(_("Frame of model")) + init_value(0) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_file.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_frame.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void reset_mesh(k3d::mesh* const Mesh)
	{
		m_output_mesh.reset(Mesh);
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		k3d::filesystem::path md2_path = m_file.pipeline_value();
		if(md2_path.empty())
			return;

		md2Model *model = new md2Model(md2_path.native_console_string().c_str());

		int frame = m_frame.pipeline_value();
		if(frame>=model->get_num_frames())
		{
			k3d::log() << error << "Frame does not exist on md2 model!" << std::endl;
			return;
		}

		k3d::gprim_factory factory(Output);

		for(int i=0; i<model->get_num_vertices(); i++)
			factory.add_point(model->get_point(frame,i));

		for(int i=0; i<model->get_num_triangles(); i++)
		{
			k3d::mesh::indices_t vertex_indices;
			for(int j=0; j<3; j++)
				vertex_indices.push_back(model->get_index(i,j));
			factory.add_polygon(vertex_indices);
		}
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
		// add scale factor and scale code for changing its size
	}

	static k3d::iplugin_factory& get_factory()
	{
		//static k3d::document_plugin_factory<mesh_reader_implementation,
        ////        k3d::interface_list<k3d::imesh_source,
        //        k3d::interface_list<k3d::imesh_storage> > > factory(
		//	k3d::uuid(0xcd0962b6, 0x3e4a132b, 0x575537a5, 0xc4af7d0a),
		//	"MD2MeshReader",
		//	_("Reader that loads external MD2 (.md2) files into the document by reference"),
		//	"MeshReader");
		static k3d::document_plugin_factory<mesh_reader_implementation,
               k3d::interface_list<k3d::imesh_source,
                k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0xcd0962b6, 0x3e4a132b, 0x575537a5, 0xc4af7d0a),
			"MD2MeshReader",
			_("Reader that loads external MD2 (.md2) files into the document by reference"),
			"MeshReader",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
	k3d_data(int, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, with_serialization) m_frame;
};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader_implementation::get_factory();
}

} // namespace io

} // namespace md2

} // namespace module

