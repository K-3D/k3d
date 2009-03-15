#ifndef K3DSDK_MESH_WRITER_H
#define K3DSDK_MESH_WRITER_H

// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k3d.com)
*/

#include "data.h"
#include "hints.h"
#include "k3d-i18n-config.h"
#include "imesh_sink.h"
#include "ipipeline_profiler.h"

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// mesh_writer

template<typename base_t>
class mesh_writer :
	public base_t,
	public imesh_sink
{
public:
	iproperty& mesh_sink_input()
	{
		return m_input_mesh;
	}
	
protected:
	mesh_writer(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<mesh*>(0)),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Output file path.")) + init_value(filesystem::path()) + init_path_mode(ipath_property::WRITE) + init_path_type(""))
	{
		m_input_mesh.changed_signal().connect(hint::converter<
			hint::convert<hint::any, hint::none> >(make_write_file_slot()));
		m_file.changed_signal().connect(hint::converter<
			hint::convert<hint::any, hint::none> >(make_write_file_slot()));
	}
	
	/// Slot to call when the file needs to be written
	sigc::slot<void, ihint*> make_write_file_slot()
	{
		return sigc::mem_fun(*this, &mesh_writer::write_file);
	}

	/// Stores the input mesh.
	k3d_data(mesh*, data::immutable_name, data::change_signal, data::no_undo, data::local_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_input_mesh;
	/// Stores the output file path.
	k3d_data(filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;

private:
	/// Called whenever our inputs have changed and it's time to write to disk.
	/// Note that execution is unaffected by the types of hints we've received.
	void write_file(ihint*)
	{
		const k3d::filesystem::path path = m_file.pipeline_value();
		const k3d::mesh* const mesh = m_input_mesh.pipeline_value();

		if(!mesh || path.empty())
			return;
	
		log() << info << "Writing " << path.native_console_string() << " using " << base_t::factory().name() << std::endl;

		k3d::filesystem::ofstream stream(path);
		if(!stream)
		{
			k3d::log() << error << k3d_file_reference << ": error opening [" << path.native_console_string() << "] for writing." << std::endl;
			return;
		}

		base_t::document().pipeline_profiler().start_execution(*this, "Write Mesh");
		on_write_mesh(*mesh, path, stream);
		base_t::document().pipeline_profiler().finish_execution(*this, "Write Mesh");
	}


	/// Implement this in derived classes to write the given mesh to an output stream.  Note that the output
	/// path is provided for reference only, all data must be written to the provided stream.
	virtual void on_write_mesh(const mesh& Input, const filesystem::path& OutputPath, std::ostream& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_MESH_WRITER_H

