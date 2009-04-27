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
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_reader.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace md2
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_reader_implementation

class mesh_reader_implementation :
	public k3d::mesh_reader<k3d::node >
{
	typedef k3d::mesh_reader<k3d::node > base;

public:
	mesh_reader_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_frame(init_owner(*this) + init_name("frame") + init_label(_("Frame")) + init_description(_("Frame of model")) + init_value(0) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_frame.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_reload_mesh_slot()));
	}

	void on_load_mesh(const k3d::filesystem::path& Path, k3d::mesh& Output)
	{
		Output = k3d::mesh();

		boost::scoped_ptr<md2Model> model(new md2Model(Path.native_filesystem_string().c_str()));

		const k3d::int32_t frame = m_frame.pipeline_value();
		if(frame < 0 || frame >= model->get_num_frames())
		{
			k3d::log() << error << "frame [" << frame << "] out-of-range for [" << Path.native_console_string() << "]" << std::endl;
			return;
		}

		const k3d::int32_t vertex_count = model->get_num_vertices();
		const k3d::int32_t triangle_count = model->get_num_triangles();

		k3d::mesh::points_t& points = Output.points.create(new k3d::mesh::points_t(vertex_count));
		k3d::mesh::selection_t& point_selection = Output.point_selection.create(new k3d::mesh::selection_t(vertex_count, 0.0));
		for(k3d::int32_t i = 0; i != vertex_count; ++i)
			points[i] = model->get_point(frame, i);

		boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));

		polyhedron->shell_first_faces.push_back(polyhedron->face_first_loops.size());
		polyhedron->shell_face_counts.push_back(triangle_count);
		polyhedron->shell_types.push_back(k3d::mesh::polyhedra_t::POLYGONS);

		polyhedron->face_first_loops.reserve(triangle_count);
		polyhedron->face_loop_counts.reserve(triangle_count);
		polyhedron->face_selections.reserve(triangle_count);
		polyhedron->face_materials.reserve(triangle_count);
		polyhedron->loop_first_edges.reserve(triangle_count);
		polyhedron->edge_points.reserve(3 * triangle_count);
		polyhedron->clockwise_edges.reserve(3 * triangle_count);
		polyhedron->edge_selections.reserve(3 * triangle_count);

		for(k3d::int32_t i = 0; i != triangle_count; ++i)
		{
			polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
			polyhedron->face_loop_counts.push_back(1);
			polyhedron->face_selections.push_back(0);
			polyhedron->face_materials.push_back(0);
			polyhedron->loop_first_edges.push_back(polyhedron->edge_points.size());

			for(k3d::uint_t j = 0; j != 3; ++j)
			{
				polyhedron->edge_points.push_back(model->get_index(i, j));
				polyhedron->clockwise_edges.push_back(polyhedron->edge_points.size());
				polyhedron->edge_selections.push_back(0);
			}
			polyhedron->clockwise_edges.back() = polyhedron->loop_first_edges.back();
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
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
	k3d_data(k3d::int32_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, with_serialization) m_frame;
};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader_implementation::get_factory();
}

} // namespace io

} // namespace md2

} // namespace module

