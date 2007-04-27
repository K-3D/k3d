#ifndef K3DSDK_NEW_MESH_MODIFIER_H
#define K3DSDK_NEW_MESH_MODIFIER_H

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
		\author Timothy M. Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "data.h"
#include "i18n.h"
#include "idocument.h"
#include "imesh_sink.h"
#include "imesh_source.h"
#include "ipipeline_profiler.h"
#include "new_mesh.h"

namespace k3d
{

namespace dev
{

template<typename base_t>
class mesh_modifier :
	public base_t,
	public imesh_sink,
	public imesh_source
{
public:
	mesh_modifier(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<mesh*>(0)),
		m_output_mesh(init_owner(*this) + init_name("output_mesh") + init_label(_("Output Mesh")) + init_description(_("Output mesh")) + init_slot(sigc::mem_fun(*this, &mesh_modifier<base_t>::create_mesh)))
	{
		m_input_mesh.changed_signal().connect(make_reset_mesh_slot());
	}

	iproperty& mesh_source_output()
	{
		return m_output_mesh;
	}

	iproperty& mesh_sink_input()
	{
		return m_input_mesh;
	}

	sigc::slot<void, iunknown*> make_reset_mesh_slot()
	{
		return sigc::mem_fun(*this, &mesh_modifier<base_t>::reset_mesh);
	}

	sigc::slot<void, iunknown*> make_update_mesh_slot()
	{
		return sigc::mem_fun(*this, &mesh_modifier<base_t>::update_mesh);
	}

protected:
	/// Allow implementations to override emitted hint
	virtual void emit_hint()
	{
		m_output_mesh.changed_signal().emit(0);
	}

	void emit_hint(iunknown* Hint)
	{
		m_output_mesh.changed_signal().emit(Hint);
	}

	k3d_data(mesh*, data::immutable_name, data::change_signal, data::no_undo, data::local_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_input_mesh;
	k3d_data(mesh*, data::immutable_name, data::change_signal, data::no_undo, data::demand_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_mesh;

private:
	void reset_mesh(iunknown* const Hint)
	{
		m_output_mesh.reset(0, Hint);
	}

	void create_mesh(mesh& Output)
	{
		if(const mesh* const input = m_input_mesh.value())
		{
			base_t::document().pipeline_profiler().start_execution(*this, "Create Mesh");
			on_create_mesh(*input, Output);
			base_t::document().pipeline_profiler().finish_execution(*this, "Create Mesh");

			base_t::document().pipeline_profiler().start_execution(*this, "Update Mesh");
			on_update_mesh(*input, Output);
			base_t::document().pipeline_profiler().finish_execution(*this, "Update Mesh");
		}
	}

	void update_mesh(iunknown* const Hint)
	{
		if(const mesh* const input = m_input_mesh.value())
		{
			if(mesh* const output = m_output_mesh.internal_value())
			{
				base_t::document().pipeline_profiler().start_execution(*this, "Update Mesh");
				on_update_mesh(*input, *output);
				base_t::document().pipeline_profiler().finish_execution(*this, "Update Mesh");

				emit_hint();
			}
		}
	}

	virtual void on_create_mesh(const mesh& Input, mesh& Output) = 0;
	virtual void on_update_mesh(const mesh& Input, mesh& Output) = 0;
};

} // namespace dev
	
} // namespace k3d

#endif // !K3DSDK_NEW_MESH_MODIFIER

