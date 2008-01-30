// K-3D
// Copyright (c) 2005-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <gdkmm/cursor.h>
#include <gtkmm/widget.h>

#include "basic_viewport_input_model.h"
#include "command_arguments.h"
#include "document_state.h"
#include "icons.h"
#include "interactive.h"
#include "keyboard.h"
#include "knife_tool.h"
#include "modifiers.h"
#include "navigation_input_model.h"
#include "selection.h"
#include "utility.h"
#include "viewport.h"

#include <k3dsdk/classes.h>
#include <k3dsdk/color.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/legacy_mesh.h>
#include "k3dsdk/mesh_selection.h"
#include <k3dsdk/properties.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/utility_gl.h>

#include <set>

namespace libk3dngui
{

namespace detail
{

/// Returns whether Edge belongs to Face
bool edge_in_loop(k3d::legacy::split_edge* Edge, k3d::legacy::split_edge* Loop)
{
	k3d::legacy::split_edge* edge = Loop;
	do
	{
		if(edge == Edge)
			return true;

		edge = edge->face_clockwise;
	}
	while(edge != Loop);

	return false;
}

/// Returns the face and polyhedron containing parameter edge
bool edge_face(const k3d::legacy::split_edge* Edge, const k3d::legacy::mesh& Mesh, k3d::legacy::face*& Face, k3d::legacy::polyhedron*& Polyhedron)
{
	for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
	{
		for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
		{
			k3d::legacy::split_edge* edge = (*face)->first_edge;
			do
			{
				if(edge == Edge)
				{
					Face = *face;
					Polyhedron = *polyhedron;
					return true;
				}

				edge = edge->face_clockwise;
			}
			while(edge != (*face)->first_edge);
		}
	}

	return false;
}

k3d::legacy::split_edge* subdivide_edge(k3d::legacy::split_edge* Edge, const double Ratio, k3d::legacy::mesh& Mesh)
{
	return_val_if_fail(Ratio > 0 && Ratio < 1, 0);

	// Create new point
	k3d::legacy::point* new_point = new k3d::legacy::point(Edge->vertex->position + Ratio * (Edge->face_clockwise->vertex->position - Edge->vertex->position));
	Mesh.points.push_back(new_point);

	// Subdivide edge
	k3d::legacy::split_edge* new_edge = new k3d::legacy::split_edge(new_point);
	new_edge->face_clockwise = Edge->face_clockwise;
	Edge->face_clockwise = new_edge;

	if(!Edge->companion)
		return new_edge;

	// Subdivide companion
	k3d::legacy::split_edge* companion = Edge->companion;

	k3d::legacy::split_edge* new_companion = new k3d::legacy::split_edge(new_point);
	new_companion->face_clockwise = companion->face_clockwise;
	companion->face_clockwise = new_companion;

	// Join edges
	k3d::legacy::join_edges(*Edge, *new_companion);
	k3d::legacy::join_edges(*new_edge, *companion);

	return new_edge;
}

// Store all edges around given edge's vertex
typedef std::set<k3d::legacy::split_edge*> edges_t;
void vertex_edges(k3d::legacy::split_edge* Edge, edges_t& Edges)
{
	k3d::legacy::split_edge* edge = Edge;
	do
	{
		if(!edge->companion)
		{
			/** \todo Handle border edges */
			return;
		}

		Edges.insert(edge);

		edge = edge->companion->face_clockwise;
	}
	while(edge != Edge);
}

/// Connects two vertices
bool connect_vertices(k3d::legacy::split_edge* Edge1, k3d::legacy::split_edge* Edge2, const k3d::legacy::mesh& Mesh)
{
	// Find split_edges belonging to the same face (to link Edge1->vertex and Edge2->vertex)
	k3d::legacy::split_edge* edge1 = 0;
	k3d::legacy::split_edge* edge2 = 0;
	edges_t edges1;
	vertex_edges(Edge1, edges1);
	edges_t edges2;
	vertex_edges(Edge2, edges2);

	for(edges_t::const_iterator e = edges1.begin(); e != edges1.end(); ++e)
	{
		for(edges_t::const_iterator e2 = edges2.begin(); e2 != edges2.end(); ++e2)
		{
			if(edge_in_loop(*e, *e2))
			{
				edge1 = *e;
				edge2 = *e2;

				goto found;
			}
		}
	}

	// Couldn't link vertices
	return false;

	found:

	// If both clicks were on the same edge, reset path
	if(edge1 == edge2 || edge1->face_clockwise == edge2 || edge2->face_clockwise == edge1)
		return false;

	k3d::legacy::face* face;
	k3d::legacy::polyhedron* polyhedron;
	return_val_if_fail(edge_face(edge1, Mesh, face, polyhedron), false);

	// Make new edge loops
	k3d::legacy::split_edge* new_edge1 = new k3d::legacy::split_edge(edge1->vertex);
	k3d::legacy::split_edge* new_edge2 = new k3d::legacy::split_edge(edge2->vertex);

	k3d::legacy::split_edge* anti_edge1 = k3d::legacy::face_anticlockwise(edge1);
	k3d::legacy::split_edge* anti_edge2 = k3d::legacy::face_anticlockwise(edge2);

	anti_edge1->face_clockwise = new_edge1;
	new_edge1->face_clockwise = edge2;
	anti_edge2->face_clockwise = new_edge2;
	new_edge2->face_clockwise = edge1;

	k3d::legacy::join_edges(*new_edge1, *new_edge2);

	// Reset face's first edge (in case it's in the ear cut)
	face->first_edge = new_edge1;

	// Create a second face for ear cut
	k3d::legacy::face* new_face = new k3d::legacy::face(new_edge2, face->material);
	polyhedron->faces.push_back(new_face);

	return true;
}

/// Calculates the center point (as an average) for a face
k3d::point3 center_point(const k3d::legacy::face& Face)
{
	k3d::point3 center(0, 0, 0);

	unsigned long count = 0;
	for(k3d::legacy::split_edge* edge = Face.first_edge; edge; edge = edge->face_clockwise)
	{
		count++;
		center += to_vector(edge->vertex->position);
		if(Face.first_edge == edge->face_clockwise)
			break;
	}
	if(count)
		center /= count;

	return center;
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// knife_tool::implementation

struct knife_tool::implementation :
	public k3d::property_collection
{
	struct split_edge_t
	{
		split_edge_t() :
			edge1(0), edge2(0), mesh(0),
			subdivided(false)
		{
		}

		void reset()
		{
			edge1 = edge2 = 0;
			mesh = 0;
			subdivided = false;
		}

		k3d::legacy::split_edge* edge1;
		k3d::legacy::split_edge* edge2;
		k3d::legacy::mesh* mesh;
		bool subdivided;
	};

	implementation(document_state& DocumentState, knife_tool& Tool) :
		m_document_state(DocumentState),
		m_tool(Tool),
		m_navigation_model(DocumentState)
	{
		m_previous_edge = 0;

		m_input_model.connect_lbutton_down(sigc::mem_fun(*this, &knife_tool::implementation::on_lbutton_down));
		m_input_model.connect_lbutton_click(sigc::mem_fun(*this, &knife_tool::implementation::on_lbutton_click));
		m_input_model.connect_rbutton_click(sigc::mem_fun(*this, &knife_tool::implementation::on_rbutton_click));

		m_input_model.connect_mbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_start_drag));
		m_input_model.connect_mbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_drag));
		m_input_model.connect_mbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_end_drag));
		m_input_model.connect_rbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_start_drag));
		m_input_model.connect_rbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_drag));
		m_input_model.connect_rbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_end_drag));
		m_input_model.connect_scroll(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_scroll));
	}

	k3d::idocument& document()
	{
		return m_document_state.document();
	}

	void on_activate()
	{
		// Reset tool
		m_previous_edge = 0;

		m_document_state.set_cursor_signal().emit(load_icon("knife_cursor", Gtk::ICON_SIZE_BUTTON));
		k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	void on_deactivate()
	{
		m_document_state.clear_cursor_signal().emit();
		k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	void on_lbutton_down(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		lbutton_down(Viewport, coordinates, convert(Event.state));
	}

	void on_lbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		lbutton_click(Viewport, coordinates);

		// Record the command for tutorials
		assert_not_implemented();
//		m_tool.record_command("selection_click", command_arguments(Viewport, widget_to_ndc(Viewport, coordinates), convert(Event.state)));
	}

	void on_rbutton_click(viewport::control& Viewport, const GdkEventButton& Event)
	{
		const k3d::point2 coordinates(Event.x, Event.y);

		rbutton_click(Viewport, coordinates);

		// Record the command for tutorials
		assert_not_implemented();
	}

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments)
	{
		try
		{
			if(Command == "selection_click")
			{
				command_arguments arguments(Arguments);

				interactive::move_pointer(arguments.get_viewport(), arguments.get_viewport_point2("mouse"));

				//lbutton_down(arguments.get_viewport(), arguments.get_viewport_point2("mouse"), arguments.key_modifiers());
				//lbutton_click(arguments.get_viewport(), arguments.get_viewport_point2("mouse"));

				return RESULT_CONTINUE;
			}
		}
		catch(std::exception& e)
		{
			k3d::log() << k3d_file_reference << ": caught exception: " << e.what() << std::endl;
		}

		return RESULT_ERROR;
	}

	k3d::iunknown* interactive_target(viewport::control& Viewport)
	{
		return Viewport.camera() ? &Viewport.camera()->navigation_target() : 0;
	}

	void lbutton_down(viewport::control& Viewport, const k3d::point2& NDC, const k3d::key_modifiers& Modifiers)
	{
		if(Modifiers.shift() && m_previous_edge)
		{
			// Shift key adds a point within the clicked face
			m_start_selection = k3d::selection::record();
			m_start_selection = Viewport.pick_face(NDC, m_document_state.pick_backfacing());
			m_pick_mode = SELECT_FACES;
		}
		else
		{
			// Find an edge under the mouse cursor
			m_start_selection = k3d::selection::record();
			m_start_selection = Viewport.pick_line(NDC, m_document_state.pick_backfacing());
			m_pick_mode = SELECT_LINES;
		}
	}

	void lbutton_click(viewport::control& Viewport, const k3d::point2& Coordinates)
	{
		assert_not_implemented();
/*
		// Check for empty selection
		k3d::inode* node = get_node(m_start_selection);
		if(!node)
			return;

		if(m_start_selection.empty())
			return;

		// Insert a FrozenMesh or get existing one
		k3d::inode* frozen_mesh = 0;
		frozen_meshes_t::const_iterator mesh_i = m_frozen_meshes.find(node);
		if(mesh_i != m_frozen_meshes.end())
		{
			frozen_mesh = mesh_i->second;
		}
		else
		{
			// Insert a FrozenMesh
			frozen_mesh = insert_mesh_modifier(*node, k3d::classes::FrozenMesh(), "Knife tool");
			m_frozen_meshes.insert(std::make_pair(node, frozen_mesh));
		}

		// Get node's mesh
		k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(frozen_mesh);
		return_if_fail(mesh_source);

		k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(mesh_source->mesh_source_output().property_internal_value());

		// Process picked edge or face
		if(SELECT_FACES == m_pick_mode)
		{
			const k3d::selection::token absolute_face = m_start_selection.get_token(k3d::selection::ABSOLUTE_FACE);
			return_if_fail(k3d::selection::null_id() != absolute_face.id);

			k3d::legacy::face* face = k3d::selection::get_face(*mesh, m_start_selection);
			return_if_fail(face);

			// Find the edge around the vertex the belongs to the selected face
			detail::edges_t edges1;
			detail::vertex_edges(m_previous_edge, edges1);
			k3d::legacy::split_edge* previous_edge = 0;
			for(detail::edges_t::const_iterator edge = edges1.begin(); edge != edges1.end(); ++edge)
			{
				if(detail::edge_in_loop(*edge, face->first_edge))
				{
					previous_edge = *edge;
					break;
				}
			}

			if(previous_edge)
			{
				k3d::point3 center = detail::center_point(*face);
				k3d::legacy::point* new_point = new k3d::legacy::point(center);
				mesh->points.push_back(new_point);

				k3d::legacy::split_edge* anti_clockwise = k3d::legacy::face_anticlockwise(previous_edge);
				k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(previous_edge->vertex);
				k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(new_point);

				anti_clockwise->face_clockwise = edge1;
				edge1->face_clockwise = edge2;
				edge2->face_clockwise = previous_edge;

				k3d::legacy::join_edges(*edge1, *edge2);

				m_previous_edge = edge2;
			}
		}
		else // SELECT_LINES == m_pick_mode
		{
			const k3d::selection::token absolute_edge = m_start_selection.get_token(k3d::selection::ABSOLUTE_SPLIT_EDGE);
			return_if_fail(k3d::selection::null_id() != absolute_edge.id);

			k3d::legacy::split_edge* edge = k3d::selection::get_split_edge(*mesh, m_start_selection);
			return_if_fail(edge);

			const k3d::point2 vertex1 = Viewport.project(edge->vertex->position);
			const k3d::point2 vertex2 = Viewport.project(edge->face_clockwise->vertex->position);

			// Select edge to connect to previous one
			k3d::legacy::split_edge* new_edge = 0;

			const double vertex_click_threshold = 3.0;
			if(k3d::distance(Coordinates, vertex1) < vertex_click_threshold)
			{
				// Edge's vertex was clicked
				new_edge = edge;
			}
			else if(k3d::distance(Coordinates, vertex2) < vertex_click_threshold)
			{
				// Edge's other vertex was clicked
				new_edge = edge->face_clockwise;
			}
			else
			{
				// Edge was clicked (not on either end)
				const double edge_length = k3d::distance(vertex2, vertex1);
				return_if_fail(edge_length);

				const double vertex1_distance = k3d::distance(Coordinates, vertex1);
				const double ratio = vertex1_distance / edge_length;

				new_edge = detail::subdivide_edge(edge, ratio, *mesh);
			}
			return_if_fail(new_edge);

			if(m_previous_edge)
			{
				// Try to connect previous and current vertices
				detail::connect_vertices(m_previous_edge, new_edge, *mesh);
			}

			m_previous_edge = new_edge;
		}

		// Update instance
		mesh_source->mesh_source_output().property_changed_signal().emit(0);
*/
	}

	void rbutton_click(viewport::control& Viewport, const k3d::point2& Coordinates)
	{
		// Reset selection
		m_previous_edge = 0;
		k3d::gl::redraw_all(m_document_state.document(), k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	void on_redraw(viewport::control& Viewport)
	{
		// Sanity checks
		return_if_fail(Viewport.gl_engine());
		return_if_fail(Viewport.camera());

		// Highlight previous vertex
		if(m_previous_edge)
		{
			const k3d::point3 position = m_previous_edge->vertex->position;

			k3d::gl::store_attributes attributes;
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			glClear(GL_DEPTH_BUFFER_BIT);
			glDisable(GL_LIGHTING);

			k3d::gl::color3d(k3d::color(0, 0, 1));

			glBegin(GL_POINTS);
				glVertex3dv(position.n);
			glEnd();

			glPopMatrix();
		}
	}

	/// Stores split_edge pointing to previous clicked vertex
	k3d::legacy::split_edge* m_previous_edge;
	/// Stores pick mode
	selection_mode_t m_pick_mode;


	/// Stores a reference to the owning document
	document_state& m_document_state;
	/// Stores a reference to the owning base tool
	knife_tool& m_tool;
	/// Stores the selection under the pointer when selection started
	k3d::selection::record m_start_selection;
	/// Stores the FrozenMeshes being cut
	typedef std::map<k3d::inode*, k3d::inode*> frozen_meshes_t;
	frozen_meshes_t m_frozen_meshes;

	/// Provides interactive navigation behavior
	navigation_input_model m_navigation_model;
	/// Dispatches incoming user input events
	basic_viewport_input_model m_input_model;
};

/////////////////////////////////////////////////////////////////////////////
// knife_tool

knife_tool::knife_tool(document_state& DocumentState, const std::string& Name) :
	base(DocumentState, Name),
	m_implementation(new implementation(DocumentState, *this))
{
}

knife_tool::~knife_tool()
{
	delete m_implementation;
}

void knife_tool::on_activate()
{
	m_implementation->on_activate();
}

void knife_tool::on_deactivate()
{
	m_implementation->on_deactivate();
}

const k3d::icommand_node::result knife_tool::execute_command(const std::string& Command, const std::string& Arguments)
{
	return m_implementation->execute_command(Command, Arguments);
}

void knife_tool::on_redraw(viewport::control& Viewport)
{
	m_implementation->on_redraw(Viewport);
}

viewport_input_model& knife_tool::get_input_model()
{
	return m_implementation->m_input_model;
}

} // namespace libk3dngui

