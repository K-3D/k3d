#ifndef K3DSDK_LEGACY_MESH_SOURCE_H
#define K3DSDK_LEGACY_MESH_SOURCE_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "data.h"
#include "hints.h"
#include "k3d-i18n-config.h"
#include "imesh_source.h"
#include "ipipeline_profiler.h"
#include "legacy_mesh.h"
#include "mesh.h"

namespace k3d
{

namespace legacy
{

/// Boilerplate mesh source code for legacy mesh source plugins that handles conversion between array meshes and legacy meshes
/** \todo Rewrite plugins that use this class to generate array meshes directly */
template<typename base_t>
class mesh_source :
	public base_t,
	public imesh_source
{
public:
	mesh_source(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_output_mesh(init_owner(*this) + init_name("output_mesh") + init_label(_("Output Mesh")) + init_description("Output mesh"))
	{
		m_output_mesh.set_initialize_slot(sigc::mem_fun(*this, &mesh_source<base_t>::initialize_mesh));
		m_output_mesh.set_update_slot(sigc::mem_fun(*this, &mesh_source<base_t>::update_mesh));
	}

	iproperty& mesh_source_output()
	{
		return m_output_mesh;
	}

	sigc::slot<void, ihint*> make_reset_mesh_slot()
	{
		return sigc::mem_fun(*this, &mesh_source<base_t>::reset_mesh);
	}

	sigc::slot<void, ihint*> make_update_mesh_slot()
	{
		return m_output_mesh.make_update_slot();
	}

protected:
	k3d_data(k3d::mesh*, data::immutable_name, data::change_signal, data::no_undo, data::pointer_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_mesh;

private:
	void reset_mesh(ihint* Hint)
	{
		m_legacy_output.reset();
		m_output_mesh.changed_signal().emit(hint::mesh_deleted::instance());
		m_output_mesh.reset(0, hint::mesh_topology_changed::instance());
	}

	void initialize_mesh(k3d::mesh& Output)
	{
		m_legacy_output.reset(new legacy::mesh());

		base_t::document().pipeline_profiler().start_execution(*this, "Create Mesh");
		on_initialize_mesh(*m_legacy_output);
		base_t::document().pipeline_profiler().finish_execution(*this, "Create Mesh");

		base_t::document().pipeline_profiler().start_execution(*this, "Update Mesh");
		on_update_mesh(*m_legacy_output);
		base_t::document().pipeline_profiler().finish_execution(*this, "Update Mesh");

		base_t::document().pipeline_profiler().start_execution(*this, "Convert Output");
		Output = *m_legacy_output;
		base_t::document().pipeline_profiler().finish_execution(*this, "Convert Output");
	}

	void update_mesh(k3d::mesh& Output)
	{
		return_if_fail(m_legacy_output.get());

		base_t::document().pipeline_profiler().start_execution(*this, "Update Mesh");
		on_update_mesh(*m_legacy_output);
		base_t::document().pipeline_profiler().finish_execution(*this, "Update Mesh");

		base_t::document().pipeline_profiler().start_execution(*this, "Convert Output");
		Output = *m_legacy_output;
		base_t::document().pipeline_profiler().finish_execution(*this, "Convert Output");
	}

	virtual void on_initialize_mesh(legacy::mesh& Output) = 0;
	virtual void on_update_mesh(legacy::mesh& Output) = 0;

	std::auto_ptr<legacy::mesh> m_legacy_output;
};

} // namespace legacy

} // namespace k3d

#endif // !K3DSDK_LEGACY_MESH_SOURCE_H

