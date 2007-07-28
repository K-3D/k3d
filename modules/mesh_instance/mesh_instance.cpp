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
#include <k3dsdk/ipipeline.h>
#include <k3d-i18n-config.h>
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

#include <boost/any.hpp>

#include <list>

namespace libk3dmeshinstance
{

// Helper class to store a copy of the hints
class hint_cache : public k3d::hint::hint_processor
{
public:
	typedef std::list<boost::any> hint_list_t;
	
	/// Copy Hint and append it to the list of received events
	void append_hint(k3d::iunknown* Hint)
	{
		process(k3d::mesh(), Hint); // mesh is not used at this point
	}
	
	hint_list_t::iterator begin()
	{
		return m_hint_list.begin();
	}
	
	hint_list_t::iterator end()
	{
		return m_hint_list.end();
	}
	
	void clear()
	{
		m_hint_list.clear();
	}
	
protected:
	// hint processor implementation
	void on_geometry_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		k3d::hint::mesh_geometry_changed_t* casted_hint = dynamic_cast<k3d::hint::mesh_geometry_changed_t*>(Hint);
		return_if_fail(casted_hint);
		{
			boost::any to_append(*casted_hint);
			m_hint_list.push_back(to_append);
		}
	}
	
	void on_selection_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		if (m_hint_list.empty() || !boost::any_cast<k3d::hint::selection_changed_t>(&(m_hint_list.back())))
		{
			boost::any to_append(*(k3d::hint::selection_changed()));
			m_hint_list.push_back(to_append);
		}
	}
	
	void on_topology_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		clear();
		boost::any to_append(*(k3d::hint::mesh_topology_changed()));
		m_hint_list.push_back(to_append);
	}
	
	void on_mesh_deleted(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		clear();
		boost::any to_append(*(k3d::hint::mesh_deleted()));
		m_hint_list.push_back(to_append);
	}
	
	void on_address_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		boost::any to_append(*(k3d::hint::mesh_address_changed()));
		m_hint_list.push_back(to_append);
	}
	
private:
	hint_list_t m_hint_list;
};


/////////////////////////////////////////////////////////////////////////////
// mesh_instance

class mesh_instance :
	public k3d::snappable<k3d::bounded<k3d::gl::drawable<k3d::ri::renderable<k3d::mesh_selection_sink<k3d::parentable<k3d::transformable<k3d::persistent<k3d::node> > > > > > > >,
	public k3d::imesh_sink,
	public k3d::imesh_source,
	public k3d::hint::hint_processor
{
	typedef k3d::snappable<k3d::bounded<k3d::gl::drawable<k3d::ri::renderable<k3d::mesh_selection_sink<k3d::parentable<k3d::transformable<k3d::persistent<k3d::node> > > > > > > > base;
public:
	mesh_instance(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<k3d::mesh*>(0)),
		m_output_mesh(init_owner(*this) + init_name("output_mesh") + init_label(_("Output Mesh")) + init_description(_("Output mesh")) + init_slot(sigc::mem_fun(*this, &mesh_instance::create_mesh))),
		m_gl_painter(init_owner(*this) + init_name("gl_painter") + init_label(_("OpenGL Mesh Painter")) + init_description(_("OpenGL Mesh Painter")) + init_value(static_cast<k3d::gl::imesh_painter*>(0))),
		m_ri_painter(init_owner(*this) + init_name("ri_painter") + init_label(_("RenderMan Mesh Painter")) + init_description(_("RenderMan Mesh Painter")) + init_value(static_cast<k3d::ri::imesh_painter*>(0))),
		m_show_component_selection(init_owner(*this) + init_name("show_component_selection") + init_label(_("Show Component Selection")) + init_description(_("Show component selection")) + init_value(false))
	{
		connect();
		// connect to the redo signal, in case we get deleted because of undo this is our last chance to be saved!
		if (document().state_recorder().current_change_set())
		{
			document().state_recorder().current_change_set()->connect_redo_signal(sigc::mem_fun(*this, &mesh_instance::connect));
		}
	}
	
	/// Make the connections
	void connect()
	{
		m_connections.push_back(m_input_mesh.changed_signal().connect(make_append_hint_slot()));
		m_connections.push_back(m_mesh_selection.changed_signal().connect(make_selection_changed_slot()));
		m_connections.push_back(m_selection_weight.changed_signal().connect(make_selection_changed_slot()));

		m_connections.push_back(m_input_matrix.changed_signal().connect(make_async_redraw_slot()));
		m_connections.push_back(m_gl_painter.changed_signal().connect(make_async_redraw_slot()));
		m_connections.push_back(m_show_component_selection.changed_signal().connect(make_async_redraw_slot()));
		
		m_connections.push_back(deleted_signal().connect(sigc::mem_fun(*this, &mesh_instance::on_instance_delete)));
		
		m_connections.push_back(document().pipeline().dependency_signal().connect(sigc::mem_fun(*this, &mesh_instance::on_dependency_change)));
	}

	sigc::slot<void, iunknown*> make_append_hint_slot()
	{
		return sigc::mem_fun(*this, &mesh_instance::append_hint);
	}

	sigc::slot<void, iunknown*> make_selection_changed_slot()
	{
		return sigc::mem_fun(*this, &mesh_instance::selection_changed);
	}
	
	/// Copy Hint and append it to the list of received events
	void append_hint(k3d::iunknown* Hint)
	{
		m_hint_cache.append_hint(Hint);
		async_redraw(0);
	}
	
	/// Needed to notify caches that we are deleted
	void on_instance_delete()
	{
		// explicit delete, notify painter immediately, otherwise the address is lost
		m_hint_cache.clear();
		const k3d::mesh* const output_mesh = m_output_mesh.value();
		if (m_gl_painter.value() && output_mesh)
			m_gl_painter.value()->mesh_changed(*output_mesh, k3d::hint::mesh_deleted());
		// clear connections to prevent crash on document close
		if(!document().state_recorder().current_change_set()) // no undo recording: this is the end, my friend
		{
			for (size_t i = 0; i != m_connections.size(); ++i)
				m_connections[i].disconnect();
			m_connections.clear();
		}
	}
	
	/// Elimination of nodes in the pipeline needs to be passed to the painters for correct cache clean-up
	void on_dependency_change(const k3d::ipipeline::dependencies_t& Dependencies)
	{
		k3d::ipipeline::dependencies_t::const_iterator new_mesh_it = Dependencies.find(&m_input_mesh);
		if (new_mesh_it != Dependencies.end())
		{
			for (k3d::ipipeline::dependencies_t::const_iterator dep = Dependencies.begin(); dep != Dependencies.end(); ++dep)
			{
				if (!dep->second) // a node was deleted from the mesh tree, so any cached data needs to be flushed
				{
					append_hint(k3d::hint::mesh_deleted());
					return;
				}
			}
		} 
	}

	void selection_changed(iunknown* const Hint)
	{
		append_hint(k3d::hint::selection_changed());
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
	
	void process_changes()
	{
		const k3d::mesh* const input_mesh = m_input_mesh.value(); // Make sure we trigger any updates that were made to the input mesh 
		const k3d::mesh* output_mesh = m_output_mesh.value();

		if (!input_mesh || !output_mesh)
			return;
		
		// Process all hints in the order ther were received
		for (hint_cache::hint_list_t::iterator hint = m_hint_cache.begin(); hint != m_hint_cache.end(); ++hint)
		{
			output_mesh = m_output_mesh.value(); // output mesh may have been reset during hint processing
			process(*output_mesh, *hint);
		}

		m_hint_cache.clear();
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		k3d::ipipeline_profiler::profile profile(document().pipeline_profiler(), *this, "Draw");
		if(k3d::gl::imesh_painter* const painter = m_gl_painter.value())
		{
			process_changes();
			const k3d::mesh* const output_mesh = m_output_mesh.value();
			return_if_fail(output_mesh);

			k3d::gl::painter_render_state render_state(State, matrix(), m_selection_weight.value(), m_show_component_selection.value());
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
			process_changes();
			const k3d::mesh* const output_mesh = m_output_mesh.value();
			return_if_fail(output_mesh);

			k3d::gl::painter_render_state render_state(State, matrix(), selection_weight, m_show_component_selection.value());
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
		static k3d::document_plugin_factory<mesh_instance,
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
	
	
protected:
	// hint_processor implementation
	void on_geometry_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_output_mesh.changed_signal().emit(Hint);
		m_gl_painter.value()->mesh_changed(Mesh, Hint);
	}
	
	void on_selection_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		return_if_fail(k3d::validate_points(Mesh));
		k3d::replace_selection(m_mesh_selection.value(), const_cast<k3d::mesh&>(Mesh));
		m_output_mesh.changed_signal().emit(Hint);
		m_gl_painter.value()->mesh_changed(Mesh, Hint);
	}
	
	void on_topology_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_gl_painter.value()->mesh_changed(Mesh, k3d::hint::mesh_deleted()); // notify painter it needs to delete the old mesh
		m_output_mesh.reset(0, Hint);
		if (const k3d::mesh* const output_mesh = m_output_mesh.value())
			m_gl_painter.value()->mesh_changed(*output_mesh, Hint);
	}
	
	void on_address_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		k3d::hint::mesh_address_changed_t* address_hint = dynamic_cast<k3d::hint::mesh_address_changed_t*>(Hint);
		address_hint->old_points_address.reset();
		address_hint->old_edge_points_address.reset();
		address_hint->old_face_first_loops_address.reset();
		address_hint->old_points_address = Mesh.points;
		if (Mesh.polyhedra) {
			address_hint->old_edge_points_address = Mesh.polyhedra->edge_points;
			address_hint->old_face_first_loops_address = Mesh.polyhedra->face_first_loops;
		}
		m_output_mesh.reset(0, Hint);
		if (const k3d::mesh* const output_mesh = m_output_mesh.value())
			m_gl_painter.value()->mesh_changed(*output_mesh, Hint);
	}
	
	void on_mesh_deleted(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_gl_painter.value()->mesh_changed(Mesh, k3d::hint::mesh_deleted()); // notify painter it needs to delete the old mesh
		m_output_mesh.reset(0, Hint);
		if (const k3d::mesh* const output_mesh = m_output_mesh.value())
			m_gl_painter.value()->mesh_changed(*output_mesh, Hint);
	}
	
private:
	hint_cache m_hint_cache;
	std::vector<sigc::connection> m_connections;
};

/////////////////////////////////////////////////////////////////////////////
// mesh_instance_factory

k3d::iplugin_factory& mesh_instance_factory()
{
	return mesh_instance::get_factory();
}

} // namespace libk3dmeshinstance

