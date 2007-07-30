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

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/mesh_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3d-version-config.h>

#include <k3dsdk/fstream.h>

namespace libk3dobjio
{

/////////////////////////////////////////////////////////////////////////////
// obj_mesh_writerio

class obj_mesh_writer :
	public k3d::mesh_sink<k3d::persistent<k3d::node> >
{
	typedef k3d::mesh_sink<k3d::persistent<k3d::node> > base;

public:
	obj_mesh_writer(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Output file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::WRITE) + init_path_type("obj_files"))
	{
		m_file.changed_signal().connect(sigc::mem_fun(*this, &obj_mesh_writer::on_write_file));
		m_input_mesh.changed_signal().connect(sigc::mem_fun(*this, &obj_mesh_writer::on_write_file));
	}

	void on_write_file(k3d::iunknown*)
	{
		const k3d::filesystem::path path = m_file.pipeline_value();
		k3d::mesh* const mesh = m_input_mesh.pipeline_value();

		if(!mesh || path.empty())
			return;

		k3d::log() << info << "Writing .obj file: " << path.native_console_string() << std::endl;

		k3d::filesystem::ofstream file(path);
		if(!file)
		{
			k3d::log() << error << k3d_file_reference << ": error opening [" << path.native_console_string() << "]" << std::endl;
			return;
		}

		file << "# Written by K-3D " << K3D_VERSION << "\n\n";

		// Store points ...
		if(mesh->points)
		{
			const k3d::mesh::points_t& points = *mesh->points;
			
			const size_t point_begin = 0;
			const size_t point_end = point_begin + points.size();
			for(size_t point = point_begin; point != point_end; ++point)
				file << "v " << points[point][0] << " " << points[point][1] << " " << points[point][2] << "\n";	
		}

		// Store polyhedra ...
		if(validate_polyhedra(*mesh))
		{
			const k3d::mesh::indices_t& first_faces = *mesh->polyhedra->first_faces;
			const k3d::mesh::counts_t& face_counts = *mesh->polyhedra->face_counts;
			const k3d::mesh::polyhedra_t::types_t& types = *mesh->polyhedra->types;
			const k3d::mesh::indices_t& face_first_loops = *mesh->polyhedra->face_first_loops;
			const k3d::mesh::counts_t& face_loop_counts = *mesh->polyhedra->face_loop_counts;
//			const k3d::mesh::selection_t& face_selection = *mesh->polyhedra->face_selection.get();
//			const k3d::mesh::materials_t& face_materials = *mesh->polyhedra->face_materials.get();
			const k3d::mesh::indices_t& loop_first_edges = *mesh->polyhedra->loop_first_edges;
			const k3d::mesh::indices_t& edge_points = *mesh->polyhedra->edge_points;
			const k3d::mesh::indices_t& clockwise_edges = *mesh->polyhedra->clockwise_edges;
//			const k3d::mesh::selection_t& edge_selection = *mesh->polyhedra->edge_selection.get();

			const size_t polyhedron_begin = 0;
			const size_t polyhedron_end = polyhedron_begin + first_faces.size();
			for(size_t polyhedron = polyhedron_begin; polyhedron != polyhedron_end; ++polyhedron)
			{
				const size_t face_begin = first_faces[polyhedron];
				const size_t face_end = face_begin + face_counts[polyhedron];
				for(size_t face = face_begin; face != face_end; ++face)
				{
					file << "f";

					const size_t loop_begin = face_first_loops[face];
					const size_t loop_end = loop_begin + face_loop_counts[face];
					for(size_t loop = loop_begin; loop != loop_end; ++loop)
					{
						const size_t first_edge = loop_first_edges[loop];
						for(size_t edge = first_edge; ; )
						{
							file << " " << edge_points[edge] + 1; //specs want indices starting with 1

							edge = clockwise_edges[edge];
							if(edge == first_edge)
								break;
						}

						/** \todo Support faces with holes */
						break;
					}

					file << "\n";
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<obj_mesh_writer, k3d::interface_list<k3d::imesh_sink > > factory(
			k3d::uuid(0x32120889, 0x85964fd3, 0x8dac7deb, 0xe3fc9676),
			"OBJMeshWriter",
			_("Mesh writer that saves external Wavefront (.obj) files"),
			"MeshWriter",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
};

k3d::iplugin_factory& obj_mesh_writer_factory()
{
	return obj_mesh_writer::get_factory();
}

} // namespace libk3dobjioio

