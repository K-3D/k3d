#ifndef K3DSDK_NGUI_TOOL_SELECTION_H
#define K3DSDK_NGUI_TOOL_SELECTION_H

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
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "document_state.h"
#include "selection.h"
#include "utility.h"
#include "viewport.h"

#include <k3d-i18n-config.h>

#include <k3dsdk/gl.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/rectangle.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/utility_gl.h>

namespace k3d
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// tool_selection

class tool_selection
{
public:
	tool_selection(k3d::idocument& Document, document_state& DocumentState) :
		m_document(Document),
		m_document_state(DocumentState),
		m_current_motion(MOTION_NONE),
		m_box_selection(0, 0, 0, 0)
	{
	}

	k3d::idocument& document()
	{
		return m_document;
	}

	void redraw_all(k3d::iunknown* const Hint = 0)
	{
		k3d::gl::redraw_all(m_document, k3d::gl::irender_viewport::ASYNCHRONOUS);
	}

	/// Enumerates motions
	typedef enum
	{
		MOTION_NONE,
		MOTION_CLICK_DRAG,	// LMB click + drag
		MOTION_DRAG,		// LMB down + drag
		MOTION_BOX_SELECT,	// LMB down + drag draws selection window
		MOTION_PAINT_SELECT
	} motion_t;

protected:
	/// Enumerates selection types
	typedef enum
	{
		SELECTION_ADD,
		SELECTION_SUBTRACT,
		SELECTED_OBJECT,
		DESELECTED_OBJECT,
		NOTHING
	} mouse_down_content_t;

	/// Stores the owning document
	k3d::idocument& m_document;
	document_state& m_document_state;

	/// Saves current selection motion
	motion_t m_current_motion;
	/// Saves mouse down action
	mouse_down_content_t m_mouse_down_content;
	/// Stores the current selection window
	k3d::rectangle m_box_selection;
	/// Saves clicked selection
	k3d::selection::record m_mouse_down_selection;

	void pick_selectables(k3d::selection::records& PickedSelectables, viewport::control& Viewport, const k3d::point2& Coordinates)
	{
		// Check if we need to pick backfacing items
		bool pick_backfacing = m_document_state.pick_backfacing();
		
		// Find what's under the mouse pointer
		m_mouse_down_selection = Viewport.pick_object(Coordinates, PickedSelectables, pick_backfacing);

		// Nullify selection in component mode if the parent node isn't selected
		if(k3d::inode* node = k3d::selection::get_node(m_mouse_down_selection))
		{
			switch(m_document_state.selection_mode().internal_value())
			{
				case SELECT_POINTS:
				case SELECT_SPLIT_EDGES:
				case SELECT_UNIFORM:
					if(!m_document_state.is_selected(node))
						m_mouse_down_selection = k3d::selection::record::empty_record();;
					break;
				case SELECT_NODES:
					break;
				default:
					assert_not_reached();
					break;
			}
		}
	}

	const k3d::rectangle normalize(const k3d::rectangle& Rectangle)
	{
		return k3d::rectangle(
			std::min(Rectangle.left, Rectangle.right),
			std::max(Rectangle.left, Rectangle.right),
			std::min(Rectangle.top, Rectangle.bottom),
			std::max(Rectangle.top, Rectangle.bottom));
	}

	/// Stores a GC for drawing selections
	Glib::RefPtr<Gdk::GC> m_selection_gc;

	/// Returns a GC for drawing selections
	Glib::RefPtr<Gdk::GC> selection_gc(viewport::control& Viewport)
	{
		if(!m_selection_gc)
		{
			Gdk::Color selection_color = convert(k3d::color(0.2, 1.0, 1.0));
			Viewport.get_default_colormap()->alloc_color(selection_color);
			m_selection_gc = Gdk::GC::create(Viewport.get_window());
			m_selection_gc->set_foreground(selection_color);
			m_selection_gc->set_function(Gdk::XOR);
			m_selection_gc->set_line_attributes(1, Gdk::LINE_ON_OFF_DASH, Gdk::CAP_BUTT, Gdk::JOIN_MITER);
		}

		return m_selection_gc;
	}

	/// Draws selection window
	void draw_rubber_band(viewport::control& Viewport)
	{
		Glib::RefPtr<Gdk::Drawable> Drawable = Viewport.get_window();
		Glib::RefPtr<Gdk::GC> GC = selection_gc(Viewport);

		const k3d::rectangle rectangle = normalize(m_box_selection);
		Drawable->draw_rectangle(GC, false, static_cast<int>(rectangle.left), static_cast<int>(rectangle.top), static_cast<int>(rectangle.width()), static_cast<int>(rectangle.height()));
	}

	void on_box_select_motion(viewport::control& Viewport, const k3d::point2& Coordinates)
	{
		draw_rubber_band(Viewport);
		m_box_selection.right = Coordinates[0];
		m_box_selection.bottom = Coordinates[1];
		draw_rubber_band(Viewport);
	}

	void on_box_select_objects(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::rectangle& SelectionRegion)
	{
		bool rubber_band_backfacing = m_document_state.rubber_band_backfacing();
		const k3d::selection::records selection = Viewport.get_object_selectables(normalize(SelectionRegion), rubber_band_backfacing);

		switch(m_mouse_down_content)
		{
			//case MOTION_RUBBER_BAND_SELECT:
			case SELECTION_ADD:
				m_document_state.select(selection);
				break;

			//case MOTION_RUBBER_BAND_DESELECT:
			case SELECTION_SUBTRACT:
				m_document_state.deselect(selection);
				break;

			case SELECTED_OBJECT:
			case DESELECTED_OBJECT:
			case NOTHING:
				m_document_state.deselect_all();
				m_document_state.select(selection);
				break;
			default:
				assert_not_reached();
		}
	}
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_TOOL_SELECTION_H

