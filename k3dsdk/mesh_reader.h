#ifndef K3DSDK_MESH_READER_H
#define K3DSDK_MESH_READER_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Tim Shead (tshead@k3d.com)
*/

#include "data.h"
#include "hints.h"
#include "k3d-i18n-config.h"
#include "ipipeline_profiler.h"
#include "imesh_source.h"
#include "imesh_storage.h"
#include "measurement.h"
#include "mesh.h"
#include "node.h"
#include "pointer_demand_storage.h"
#include "user_interface.h"
#include "watched_path_property.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// mesh_reader

template<typename base_t>
class mesh_reader :
	public base_t,
	public imesh_source,
	public imesh_storage
{
public:
	iproperty& mesh_source_output()
	{
		return m_output_mesh;
	}

	void reset_mesh(mesh* const Mesh)
	{
		m_output_mesh.reset(Mesh);
	}

	/// Returns a slot that should be connected to input properties to signal that the output mesh has changed
	sigc::slot<void, ihint*> make_reload_mesh_slot()
	{
		return m_output_mesh.make_slot();
	}

protected:
	mesh_reader(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file path.")) + init_value(filesystem::path()) + init_path_mode(ipath_property::READ) + init_path_type("")),
		m_center(init_owner(*this) + init_name("center") + init_label(_("Center on Origin")) + init_description(_("Center the output mesh around the origin.")) + init_value(true)),
		m_scale_to_size(init_owner(*this) + init_name("scale_to_size") + init_label(_("Scale to Size")) + init_description(_("Scale the output mesh to fit within a fixed-size bounding-box.")) + init_value(true)),
		m_size(init_owner(*this) + init_name("size") + init_label(_("Size")) + init_description(_("Output mesh size when \"Scale to Size\" is enabled.")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(measurement::distance))),
		m_output_mesh(init_owner(*this) + init_name("output_mesh") + init_label(_("Output Mesh")) + init_description("Output mesh"))
	{
		m_file.changed_signal().connect(hint::converter<
			hint::convert<hint::any, hint::none> >(make_reload_mesh_slot()));
		m_center.changed_signal().connect(hint::converter<
			hint::convert<hint::any, hint::none> >(make_reload_mesh_slot()));
		m_scale_to_size.changed_signal().connect(hint::converter<
			hint::convert<hint::any, hint::none> >(make_reload_mesh_slot()));
		m_size.changed_signal().connect(hint::converter<
			hint::convert<hint::any, hint::none> >(make_reload_mesh_slot()));

		m_output_mesh.set_update_slot(sigc::mem_fun(*this, &mesh_reader<base_t>::execute));
	}

	/// Stores the input file path.
	k3d_data(filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, watched_path_property, watched_path_serialization) m_file;
	/// Center the output mesh on the origin.
	k3d_data(bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_center;
	/// Scale the output mesh to fit within a fixed-size bounding-box.
	k3d_data(bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_scale_to_size;
	/// Stores the size of the bounding-box used with m_scale_to_size.
	k3d_data(double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_size;
	/// Stores the output mesh, which is created / updated on-demand.
	k3d_data(mesh*, immutable_name, change_signal, no_undo, pointer_demand_storage, no_constraint, read_only_property, no_serialization) m_output_mesh;

private:
	/// Called whenever the output mesh has been modified and needs to be updated.
	/// Note that execution is unaffected by the types of hints we've received.
	void execute(const std::vector<ihint*>& Hints, mesh& Mesh)
	{
		// Start with an empty mesh ...
		Mesh = mesh();

		const filesystem::path path = m_file.pipeline_value();
		if(path.empty())
			return;
	
		log() << info << "Loading " << path.native_console_string() << " using " << base_t::factory().name() << std::endl;

		if(!filesystem::exists(path))
		{
			log() << error << "filesystem path " << path.native_console_string() << " doesn't exist" << std::endl;
			return;
		}

		base_t::document().pipeline_profiler().start_execution(*this, "Load Mesh");
		on_load_mesh(path, Mesh);
		base_t::document().pipeline_profiler().finish_execution(*this, "Load Mesh");

		base_t::document().pipeline_profiler().start_execution(*this, "Transform Mesh");

		if(Mesh.points)
		{
			const bool_t center = m_center.pipeline_value();
			const bool_t scale_to_size = m_scale_to_size.pipeline_value();

			bounding_box3 bounding_box;
			matrix4 transformation = identity3();

			mesh::points_t& output_points = Mesh.points.writable();
			if(center || scale_to_size)
				bounding_box = mesh::bounds(output_points);

			if(center)
			{
				transformation = transformation * translate3(vector3(
					-0.5 * (bounding_box.px + bounding_box.nx),
					-0.5 * (bounding_box.py + bounding_box.ny),
					-0.5 * (bounding_box.pz + bounding_box.nz)));
			}

			if(scale_to_size)
			{
				const double_t current_size = std::max(bounding_box.width(), std::max(bounding_box.height(), bounding_box.depth()));
				if(current_size)
					transformation = scale3(m_size.pipeline_value() / current_size) * transformation;
			}

			const uint_t point_begin = 0;
			const uint_t point_end = output_points.size();
			for(uint_t point = point_begin; point != point_end; ++point)
				output_points[point] = transformation * output_points[point];
		}

		base_t::document().pipeline_profiler().finish_execution(*this, "Transform Mesh");
	}


	/// Implement this in derived classes to read the input file and produce an output mesh.  Note that the
	/// output mesh will be empty every time this method is called.
	virtual void on_load_mesh(const filesystem::path& Path, mesh& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_MESH_READER_H

