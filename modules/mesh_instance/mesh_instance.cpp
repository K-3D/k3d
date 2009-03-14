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

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/ibounded.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/parentable.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/snappable.h>
#include <k3dsdk/transformable.h>

#include <boost/any.hpp>

#include <list>

namespace module
{

namespace mesh_instance
{

/////////////////////////////////////////////////////////////////////////////
// mesh_instance

class mesh_instance :
	public k3d::snappable<k3d::gl::renderable<k3d::ri::renderable<k3d::mesh_selection_sink<k3d::parentable<k3d::transformable<k3d::node > > > > > >,
	public k3d::ibounded,
	public k3d::imesh_sink,
	public k3d::imesh_source
{
	typedef k3d::snappable<k3d::gl::renderable<k3d::ri::renderable<k3d::mesh_selection_sink<k3d::parentable<k3d::transformable<k3d::node > > > > > > base;
public:
	mesh_instance(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<k3d::mesh*>(0)),
		m_output_mesh(init_owner(*this) + init_name("output_mesh") + init_label(_("Output Mesh")) + init_description(_("Output mesh"))),
		m_gl_painter(init_owner(*this) + init_name("gl_painter") + init_label(_("OpenGL Mesh Painter")) + init_description(_("OpenGL Mesh Painter")) + init_value(static_cast<k3d::gl::imesh_painter*>(0))),
		m_ri_painter(init_owner(*this) + init_name("ri_painter") + init_label(_("RenderMan Mesh Painter")) + init_description(_("RenderMan Mesh Painter")) + init_value(static_cast<k3d::ri::imesh_painter*>(0))),
		m_show_component_selection(init_owner(*this) + init_name("show_component_selection") + init_label(_("Show Component Selection")) + init_description(_("Show component selection")) + init_value(false)),
		m_output_mesh_pointer(0),
		m_document_closed(false),
		m_updating(false)
	{
		m_input_mesh.changed_signal().connect(make_mesh_changed_slot());
		m_mesh_selection.changed_signal().connect(make_selection_changed_slot());

		m_input_matrix.changed_signal().connect(make_async_redraw_slot());
		m_gl_painter.changed_signal().connect(make_async_redraw_slot());
		m_show_component_selection.changed_signal().connect(make_async_redraw_slot());
		
		m_output_mesh.set_initialize_slot(sigc::mem_fun(*this, &mesh_instance::create_mesh));
		m_output_mesh.set_update_slot(sigc::mem_fun(*this, &mesh_instance::update_mesh));
		
		m_delete_connection = deleted_signal().connect(sigc::mem_fun(*this, &mesh_instance::on_instance_delete));
		document().close_signal().connect(sigc::mem_fun(*this, &mesh_instance::disconnect));
	}

	sigc::slot<void, k3d::ihint*> make_mesh_changed_slot()
	{
		return sigc::mem_fun(*this, &mesh_instance::mesh_changed);
	}
	
	sigc::slot<void, k3d::ihint*> make_selection_changed_slot()
	{
		return sigc::mem_fun(*this, &mesh_instance::selection_changed);
	}
	
	/// Disconnect the delete signal to prevent signal propagation to deleted nodes during document close, and stop drawing
	void disconnect()
	{
		m_delete_connection.disconnect();
		m_document_closed = true;
	}
	
	/// Executed when the input mesh changed
	void mesh_changed(k3d::ihint* Hint)
	{
		if (m_document_closed)
			return;
		// Use stored pointer as key for the mesh painters, avoiding pipeline execution here...
		if(m_output_mesh_pointer)
		{
			if(k3d::gl::imesh_painter* const painter = m_gl_painter.pipeline_value())
				painter->mesh_changed(*m_output_mesh_pointer, Hint);
		}
		m_output_mesh.update(Hint);
		async_redraw(0);
	}
	
	/// Needed to notify caches that we are deleted
	void on_instance_delete()
	{
		mesh_changed(k3d::hint::mesh_deleted::instance());
	}

	void selection_changed(k3d::ihint* const Hint)
	{
		mesh_changed(k3d::hint::selection_changed::instance());
	}

	void create_mesh(k3d::mesh& OutputMesh)
	{
		update_mesh(OutputMesh);
	}
	
	void update_mesh(k3d::mesh& OutputMesh)
	{
		m_updating = true;
		k3d::ipipeline_profiler::profile profile(document().pipeline_profiler(), *this, "Update mesh");
		// we store this to avoid executing the pipeline in mesh_changed
		m_output_mesh_pointer = &OutputMesh;
		if(const k3d::mesh* const input_mesh = m_input_mesh.pipeline_value())
		{
			OutputMesh = *input_mesh;
			k3d::mesh_selection::merge(m_mesh_selection.pipeline_value(), OutputMesh);
		}
		else
		{
			OutputMesh = k3d::mesh();
		}
		m_updating = false;
	}

	const k3d::bounding_box3 extents()
	{
		k3d::bounding_box3 results;

		const k3d::mesh* const output_mesh = k3d::property::pipeline_value<k3d::mesh*>(m_output_mesh);
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
		if (m_updating)
			return;
		if (m_document_closed)
			return;
		k3d::ipipeline_profiler::profile profile(document().pipeline_profiler(), *this, "Draw");
		if(k3d::gl::imesh_painter* const painter = m_gl_painter.pipeline_value())
		{
			const k3d::mesh* const output_mesh = k3d::property::pipeline_value<k3d::mesh*>(m_output_mesh);
			return_if_fail(output_mesh);
			
			k3d::gl::painter_render_state render_state(State, matrix(), m_show_component_selection.pipeline_value());
			try
			{
				painter->paint_mesh(*output_mesh, render_state);
			}
			catch(std::runtime_error& E) // VBO painters throw an exception if the VBO state is corrupted. We force regeneration using mesh_changed.
			{
				k3d::log() << error << E.what() << std::endl;
				mesh_changed(0);
			}
		}
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectionState)
	{
		if (m_updating)
			return;
		if (m_document_closed)
			return;

		k3d::ipipeline_profiler::profile profile(document().pipeline_profiler(), *this, "Select");
		if(k3d::gl::imesh_painter* const painter = m_gl_painter.pipeline_value())
		{
			const k3d::mesh* const output_mesh = k3d::property::pipeline_value<k3d::mesh*>(m_output_mesh);
			return_if_fail(output_mesh);

			k3d::gl::painter_render_state render_state(State, matrix(), m_show_component_selection.pipeline_value());
			k3d::gl::painter_selection_state selection_state(SelectionState);

			// At the top-level, ID the entire instance ...
			k3d::gl::push_selection_token(this);
			// Then, ID the underlying mesh ...
			k3d::gl::push_selection_token(k3d::selection::MESH, 0);
			// Now give the painters a chance ...
			try
			{
				painter->select_mesh(*output_mesh, render_state, selection_state);
			}
			catch(std::runtime_error& E) // VBO painters throw an exception if the VBO state is corrupted. We force regeneration using mesh_changed.
			{
				k3d::log() << error << E.what() << std::endl;
				mesh_changed(0);
			}

			k3d::gl::pop_selection_token(); // mesh
			k3d::gl::pop_selection_token(); // node
		}
	}

	void on_renderman_render(const k3d::ri::render_state& State)
	{
		if(k3d::ri::imesh_painter* const painter = m_ri_painter.pipeline_value())
		{
			const k3d::mesh* const input_mesh = m_input_mesh.pipeline_value();
			return_if_fail(input_mesh);

			painter->paint_mesh(*input_mesh, State);
		}
	}

	void on_renderman_render_complete(const k3d::ri::render_state& State)
	{
		if(k3d::ri::imesh_painter* const painter = m_ri_painter.pipeline_value())
		{
			const k3d::mesh* const input_mesh = m_input_mesh.pipeline_value();
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
		static k3d::document_plugin_factory<mesh_instance,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > > > factory(
				k3d::classes::MeshInstance(),
				"MeshInstance",
				_("Renders an instance of a geometric mesh"),
				"Mesh",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}


private:
	k3d_data(k3d::mesh*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_input_mesh;
	k3d_data(k3d::mesh*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::pointer_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_output_mesh;
	k3d_data(k3d::gl::imesh_painter*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::with_undo, k3d::data::node_storage, k3d::data::no_constraint, k3d::data::node_property, k3d::data::node_serialization) m_gl_painter;
	k3d_data(k3d::ri::imesh_painter*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::with_undo, k3d::data::node_storage, k3d::data::no_constraint, k3d::data::node_property, k3d::data::node_serialization) m_ri_painter;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_show_component_selection;
	const k3d::mesh* m_output_mesh_pointer;
	sigc::connection m_delete_connection;
	bool m_document_closed;
	bool m_updating;
};

/////////////////////////////////////////////////////////////////////////////
// mesh_instance_factory

k3d::iplugin_factory& mesh_instance_factory()
{
	return mesh_instance::get_factory();
}

} // namespace mesh_instance

} // namespace module
