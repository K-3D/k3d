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
*/

#include <k3dsdk/bounded.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/drawable_gl.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/parentable.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/snappable.h>
#include <k3dsdk/transformable.h>

namespace libk3ddevelopment
{

/////////////////////////////////////////////////////////////////////////////
// new_mesh_instance

class new_mesh_instance :
	public k3d::snappable<k3d::bounded<k3d::gl::drawable<k3d::ri::renderable<k3d::mesh_selection_sink<k3d::parentable<k3d::transformable<k3d::persistent<k3d::node> > > > > > > >,
	public k3d::imesh_sink,
	public k3d::imesh_source
{
	typedef k3d::snappable<k3d::bounded<k3d::gl::drawable<k3d::ri::renderable<k3d::mesh_selection_sink<k3d::parentable<k3d::transformable<k3d::persistent<k3d::node> > > > > > > > base;

public:
	new_mesh_instance(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<k3d::mesh*>(0)),
		m_output_mesh(init_owner(*this) + init_name("output_mesh") + init_label(_("Output Mesh")) + init_description(_("Output mesh")) + init_slot(sigc::mem_fun(*this, &new_mesh_instance::create_mesh))),
		m_gl_painter(init_owner(*this) + init_name("gl_painter") + init_label(_("OpenGL Mesh Painter")) + init_description(_("OpenGL Mesh Painter")) + init_value(static_cast<k3d::gl::imesh_painter*>(0))),
		m_ri_painter(init_owner(*this) + init_name("ri_painter") + init_label(_("RenderMan Mesh Painter")) + init_description(_("RenderMan Mesh Painter")) + init_value(static_cast<k3d::ri::imesh_painter*>(0))),
		m_show_component_selection(init_owner(*this) + init_name("show_component_selection") + init_label(_("Show Component Selection")) + init_description(_("Show component selection")) + init_value(false))
	{
		m_input_mesh.changed_signal().connect(make_reset_mesh_slot());
		m_mesh_selection.changed_signal().connect(make_selection_changed_slot());
		m_selection_weight.changed_signal().connect(make_selection_changed_slot());

		m_input_matrix.changed_signal().connect(make_async_redraw_slot());
		m_gl_painter.changed_signal().connect(make_async_redraw_slot());
		m_show_component_selection.changed_signal().connect(make_async_redraw_slot());
	}

	~new_mesh_instance()
	{
	}

	sigc::slot<void, iunknown*> make_reset_mesh_slot()
	{
		return sigc::mem_fun(*this, &new_mesh_instance::reset_mesh);
	}

	sigc::slot<void, iunknown*> make_selection_changed_slot()
	{
		return sigc::mem_fun(*this, &new_mesh_instance::selection_changed);
	}

	void reset_mesh(iunknown* const Hint)
	{
		if (dynamic_cast<k3d::hint::mesh_topology_changed_t*>(Hint) || !Hint)
		{
			k3d::log() << debug << "new_mesh_instance: doing real mesh reset" << std::endl;
			m_output_mesh.reset(0, Hint);
		}
		else
		{
			m_output_mesh.changed_signal().emit(Hint);
		}
		
		if(const k3d::mesh* const output_mesh = m_output_mesh.value())
		{
			if(k3d::gl::imesh_painter* const painter = m_gl_painter.value())
				painter->mesh_changed(*output_mesh, Hint);
		}

		async_redraw(Hint);
	}

	void selection_changed(iunknown* const Hint)
	{
		if (Hint) // pass-through in case a hint was given upstream
		{
			reset_mesh(Hint);
			return;
		}
		if(k3d::mesh* output_mesh = m_output_mesh.value())
		{
			k3d::replace_selection(m_mesh_selection.value(), *output_mesh);
		}
		k3d::hint::selection_changed_t hint;
		hint.set_selection(m_mesh_selection.value()); // provide current selection as hint
		reset_mesh(&hint);
	}

	void create_mesh(k3d::mesh& OutputMesh)
	{
		k3d::ipipeline_profiler::profile profile(document().pipeline_profiler(), *this, "Copy Input");
		if(const k3d::mesh* const input_mesh = m_input_mesh.value())
		{
			OutputMesh = *input_mesh;
			k3d::replace_selection(m_mesh_selection.value(), OutputMesh);
		}
	}

	const k3d::bounding_box3 extents()
	{
		k3d::bounding_box3 results;

		const k3d::mesh* const output_mesh = m_output_mesh.value();
		return_val_if_fail(output_mesh, results);

		if(output_mesh->points)
		{
			for(k3d::mesh::points_t::const_iterator point = output_mesh->points->begin(); point != output_mesh->points->end(); ++point)
			{
				results.px = std::max(results.px, (*point)[0]);
				results.py = std::max(results.py, (*point)[1]);
				results.pz = std::max(results.pz, (*point)[2]);
				results.nx = std::min(results.nx, (*point)[0]);
				results.ny = std::min(results.ny, (*point)[1]);
				results.nz = std::min(results.nz, (*point)[2]);
			}
		}

		return results;
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::ipipeline_profiler::profile profile(document().pipeline_profiler(), *this, "Draw");
		if(k3d::gl::imesh_painter* const painter = m_gl_painter.value())
		{
			const k3d::mesh* const output_mesh = m_output_mesh.value();
			return_if_fail(output_mesh);

			k3d::gl::painter_render_state render_state(State);

			render_state.node_selection = m_selection_weight.value();
			render_state.show_component_selection = m_show_component_selection.value();

			painter->paint_mesh(*output_mesh, render_state);
		}
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectionState)
	{
		const double selection_weight = m_selection_weight.value();
		if(SelectionState.exclude_unselected_nodes && !selection_weight)
			return;

		k3d::ipipeline_profiler::profile profile(document().pipeline_profiler(), *this, "Select");
		if(k3d::gl::imesh_painter* const painter = m_gl_painter.value())
		{

			const k3d::mesh* const output_mesh = m_output_mesh.value();
			return_if_fail(output_mesh);

			k3d::gl::painter_render_state render_state(State);
			render_state.node_selection = selection_weight;
			render_state.show_component_selection = m_show_component_selection.value();

			k3d::gl::painter_selection_state selection_state(SelectionState);

			// At the top-level, ID the entire instance ...
			k3d::gl::push_selection_token(this);
			// Then, ID the underlying mesh ...
			k3d::gl::push_selection_token(k3d::selection::MESH, 0);
			// Now give the painters a chance ...
			painter->select_mesh(*output_mesh, render_state, selection_state);

			k3d::gl::pop_selection_token(); // mesh
			k3d::gl::pop_selection_token(); // node
		}
	}

	void on_renderman_render(const k3d::ri::render_state& State)
	{
		if(k3d::ri::imesh_painter* const painter = m_ri_painter.value())
		{
			const k3d::mesh* const input_mesh = m_input_mesh.value();
			return_if_fail(input_mesh);

			painter->paint_mesh(*input_mesh, State);
		}
	}

	void on_renderman_render_complete(const k3d::ri::render_state& State)
	{
		if(k3d::ri::imesh_painter* const painter = m_ri_painter.value())
		{
			const k3d::mesh* const input_mesh = m_input_mesh.value();
			return_if_fail(input_mesh);

			painter->paint_complete(*input_mesh, State);
		}
	}

	k3d::iproperty& mesh_source_output()
	{
		return m_output_mesh;
	}

	k3d::iproperty& mesh_sink_input()
	{
		return m_input_mesh;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<new_mesh_instance,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > > > factory(
				k3d::classes::MeshInstance(),
				"MeshInstance",
				_("Renders an instance of a geometric mesh"),
				"Development",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

	k3d_data(k3d::mesh*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_input_mesh;
	k3d_data(k3d::mesh*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::demand_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_output_mesh;
	k3d_data(k3d::gl::imesh_painter*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::with_undo, k3d::data::node_storage, k3d::data::no_constraint, k3d::data::node_property, k3d::data::node_serialization) m_gl_painter;
	k3d_data(k3d::ri::imesh_painter*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::with_undo, k3d::data::node_storage, k3d::data::no_constraint, k3d::data::node_property, k3d::data::node_serialization) m_ri_painter;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_show_component_selection;
};

/////////////////////////////////////////////////////////////////////////////
// new_mesh_instance_factory

k3d::iplugin_factory& new_mesh_instance_factory()
{
	return new_mesh_instance::get_factory();
}

} // namespace libk3ddevelopment

