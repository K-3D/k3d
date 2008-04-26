#ifndef NGUI_SNAP_TOOL_DETAIL_H
#define NGUI_SNAP_TOOL_DETAIL_H

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
	\author Tim Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/ngui/basic_viewport_input_model.h>
#include <k3dsdk/ngui/navigation_input_model.h>
#include <k3dsdk/ngui/selection.h>
#include <k3dsdk/ngui/tool_selection.h>
#include <k3dsdk/ngui/transform_tool.h>
#include <k3dsdk/ngui/utility.h>
#include <k3dsdk/ngui/viewport.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/transform.h>

using namespace libk3dngui;

namespace k3d { class isnap_source; }
namespace k3d { class isnap_target; }
namespace k3d { class isnappable; }

namespace module
{

namespace ngui
{

namespace snap
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// abstract_tool

class abstract_tool
{
public:
	virtual ~abstract_tool() {}

	// Functions executed by transform_tool that rely on transform_tool's derived classes functions
	virtual std::string manipulator_name(const k3d::selection::id ID) = 0;
	typedef std::vector<std::string> manipulators_t;
	virtual std::string get_manipulator(const manipulators_t& Manipulators) = 0;
	virtual void set_manipulator(const std::string ManipulatorName) = 0;
	virtual std::string get_constraint_name() = 0;

	virtual void begin_mouse_move(const k3d::point2& Coordinates) = 0;

	virtual void update_constraint(viewport::control& Viewport, const k3d::point2& Coordinates) = 0;
	virtual void reset() = 0;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// snap_tool_detail

class snap_tool_detail :
	public tool_selection,
	public k3d::property_collection
{
	typedef tool_selection base;

public:
	snap_tool_detail(k3d::idocument& Document, document_state& DocumentState, detail::abstract_tool& AbstractTool) :
		base(Document, DocumentState),
		k3d::property_collection(),
		m_document(Document),
		m_document_state(DocumentState),
		m_abstract_tool(AbstractTool),
		m_navigation_model(DocumentState),
		m_current_target(0),
		m_deleted_target(false),
		m_coordinate_system(init_owner(*this) + init_name("coordinate_system") + init_label(_("Coordinate system")) + init_description(_("Coordinate system in which the transformation occurs")) + init_value(GLOBAL) + init_enumeration(coordinate_system_values())),
		m_visible_manipulators(init_owner(*this) + init_name("visible_manipulators") + init_label(_("Visible manipulators")) + init_description(_("Shows/hides manipulators")) + init_value(true))
	{
		m_coordinate_system.changed_signal().connect(sigc::mem_fun(*this, &snap_tool_detail::update_coordinate_system));
		m_visible_manipulators.changed_signal().connect(sigc::mem_fun(*this, &snap_tool_detail::redraw_all));

		connect_navigation_input_model();
	}

	~snap_tool_detail()
	{
		clear_targets();
	}

	// Interface required for data containers
	k3d::idocument& document()
	{
		return m_document;
	}

	navigation_input_model& navigation_model()
	{
		return m_navigation_model;
	}

	basic_viewport_input_model& input_model()
	{
		return m_input_model;
	}

protected:
	sigc::connection m_mbutton_start_drag_signal;
	sigc::connection m_mbutton_drag_signal;
	sigc::connection m_mbutton_end_drag_signal;
	sigc::connection m_rbutton_start_drag_signal;
	sigc::connection m_rbutton_drag_signal;
	sigc::connection m_rbutton_end_drag_signal;
	sigc::connection m_scroll_signal;

	void connect_navigation_input_model()
	{
		m_mbutton_start_drag_signal = m_input_model.connect_mbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_start_drag));
		m_mbutton_drag_signal = m_input_model.connect_mbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_drag));
		m_mbutton_end_drag_signal = m_input_model.connect_mbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button1_end_drag));
		m_rbutton_start_drag_signal = m_input_model.connect_rbutton_start_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_start_drag));
		m_rbutton_drag_signal = m_input_model.connect_rbutton_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_drag));
		m_rbutton_end_drag_signal = m_input_model.connect_rbutton_end_drag(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_button2_end_drag));
		m_scroll_signal = m_input_model.connect_scroll(sigc::mem_fun(m_navigation_model, &navigation_input_model::on_scroll));
	}

	void disconnect_navigation_input_model()
	{
		m_mbutton_start_drag_signal.disconnect();
		m_mbutton_drag_signal.disconnect();
		m_mbutton_end_drag_signal.disconnect();
		m_rbutton_start_drag_signal.disconnect();
		m_rbutton_drag_signal.disconnect();
		m_rbutton_end_drag_signal.disconnect();
		m_scroll_signal.disconnect();
	}

	/// Mouse handling functions
	void lbutton_down(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::key_modifiers& Modifiers);
	void lmb_down_add();
	void lmb_down_subtract();
	void lmb_down_manipulator(const std::string& ManipulatorName);
	void lmb_down_selected();
	void lmb_down_deselected();
	void lmb_down_nothing();
	void lbutton_click(const viewport::control& Viewport, const k3d::point2& Coordinates);
	void lmb_click_add();
	void lmb_click_subtract();
	void lmb_click_replace();
	void lmb_click_start_motion(const k3d::point2& Coordinates);
	void lmb_click_stop_motion();
	void lmb_click_deselect_all();
	void lbutton_start_drag(viewport::control& Viewport, const k3d::point2& Coordinates);
	void lmb_start_drag_start_motion(const k3d::point2& Coordinates);
	void lmb_start_drag_box_select(viewport::control& Viewport, const k3d::point2& Coordinates);
	void lmb_drag_box_select(viewport::control& Viewport, const k3d::point2& Coordinates);
	void lbutton_end_drag(viewport::control& Viewport, const k3d::point2& Coordinates);
	void lmb_end_drag_stop_motion();
	void lmb_end_drag_box_select(viewport::control& Viewport, const k3d::point2& Coordinates);
	void mbutton_click(viewport::control& Viewport, const k3d::point2& Coordinates, const k3d::key_modifiers& Modifiers);
	void mmb_click_toggle_manipulators_visibility();
	void mmb_click_manipulators_next_selection();
	void mmb_click_switch_coordinate_system();
	void mmb_click_next_constraint(viewport::control& Viewport, const k3d::point2& Coordinates);
	void rbutton_click(const viewport::control& Viewport, const k3d::point2& Coordinates);
	void rmb_click_selection_tool();
	void rmb_click_cancel_move();

	void cancel_mouse_move();
	std::string complete_mouse_move();

	// Common functions
/** \todo standardize set_motion in selection tool and transform tools */
	void set_motion(const motion_t Motion);

	/// Manipulators scale to adjust to viewport size
	double m_manipulators_scale;
	/// Updates manipulators scale to show at constant size after camera update
	void update_manipulators_scale(viewport::control& Viewport, const k3d::point3& Origin);
	/// Returns whether given direction faces the screen
	bool front_facing(viewport::control& Viewport, const k3d::vector3& Normal, const k3d::point3& Origin);

	/// Handles off-screen pointer warping during mouse drag
	void off_screen_warp(viewport::control& Viewport, k3d::point2& NewCoordinates);
	/// Keep an offset to compute mouse coordinates in an infinite virtual screen
	k3d::point2 m_off_screen_offset;

	/// Deletes all targets
	void clear_targets();
	/// Mark : one of the target nodes was deleted
	void target_deleted() { m_deleted_target = true; }

	/// Retrieves the current document selection, refreshing the target list
	void get_current_selection();
	/// Returns the number of selected targets (nodes or components depending on selection mode)
	unsigned long target_number();
	/// Updates target list whenever necessary (e.g. : one of them was deleted)
	void update_targets();
	/// Transform targets
	void start_transform();
	void transform_targets(k3d::isnappable* const Target, k3d::isnap_target* const SnapTarget, const double SnapDistance, const bool SnapOrientation, const bool MatchGroups, const k3d::matrix4& Transform);

private:
	/// Stores an object to be moved interactively
	class itarget
	{
	public:
		itarget() :
			current_system_type(GLOBAL),
			modifier(0)
		{
		}

		virtual ~itarget() {}

		// Returns selected target number
		virtual unsigned long target_number() = 0;

		// Resets move
		virtual void reset(k3d::iunknown*) = 0;

		// Update coordinate system
		void set_local() { current_system_type = LOCAL; reset(0); }
		void set_global() { current_system_type = GLOBAL; reset(0); }
		void set_parent() { current_system_type = PARENT; reset(0); }

		// World parameters
		k3d::matrix4 world_orientation()
		{
			if(LOCAL == current_system_type)
				return k3d::extract_rotation(k3d::node_to_world_matrix(*node));

			if(PARENT == current_system_type)
				return k3d::extract_rotation(k3d::parent_to_world_matrix(*node));

			return k3d::identity3D();
		}

		// Modifier
		void set_transform_modifier(k3d::inode* Modifier)
		{
			modifier = Modifier;
			Modifier->deleted_signal().connect(sigc::mem_fun(*this, &itarget::reset_transform_modifier));
		}
		void reset_transform_modifier()
		{
			modifier = 0;
		}

		// Actions
		virtual void start_transform() = 0;
		virtual void transform(k3d::isnappable* const Target, k3d::isnap_target* const SnapTarget, const double SnapDistance, const bool SnapOrientation, const bool MatchGroups, const k3d::matrix4& Transform) = 0;

	protected:
		typedef enum
		{
			GLOBAL,
			LOCAL,
			PARENT
		} system_t;

		/// Stores the transformation type
		system_t current_system_type;
		/// Stores the target node
		k3d::inode* node;
		/// Stores the transformation to be applied
		k3d::inode* modifier;

		/// Stores initial position
		k3d::point3 m_origin;
		/// Stores manipulators matrix
		k3d::matrix4 m_manipulator_orientation;
		/// Stores orientation matrix and its inverse
		k3d::matrix4 m_system_orientation;
		k3d::matrix4 m_system_orientation_inverse;
	};

	class transform_target :
		public itarget
	{
	public:
		transform_target(k3d::inode* Node);

		unsigned long target_number();
		virtual void reset(k3d::iunknown*);

		virtual void start_transform();
		const k3d::matrix4 snap(k3d::isnappable* const Target, k3d::isnap_target* const SnapTarget, const double SnapDistance, const bool SnapOrientation, const bool MatchGroups, const k3d::matrix4& Transform);
		virtual void transform(k3d::isnappable* const Target, k3d::isnap_target* const SnapTarget, const double SnapDistance, const bool SnapOrientation, const bool MatchGroups, const k3d::matrix4& Transform);

		bool create_transform_modifier(const std::string& Name);

	private:
		/// Stores un-snapped transformation
		k3d::matrix4 m_world_transformation;
		/// Stores original rotation matrix
		k3d::matrix4 m_original_rotation;
	};

	class mesh_target :
		public itarget
	{
	public:
		mesh_target(document_state& DocumentState, k3d::inode* Node, k3d::iproperty& MeshSourceProperty);
		~mesh_target()
		{
			m_modifier_change_signal.disconnect();
		}

		virtual unsigned long target_number();
		virtual void reset(k3d::iunknown*);

		virtual void start_transform();
		virtual void transform(k3d::isnappable* const Target, k3d::isnap_target* const SnapTarget, const double SnapDistance, const bool SnapOrientation, const bool MatchGroups, const k3d::matrix4& Transform);

	private:
		void reset_selection();
		void update_mesh_modifier();

		/// Inserts a tweak modifier
		void create_mesh_modifier(const std::string& Name);
		/// Initializes component transformation
		void init_transformation();

		/// Stores the owning document_state
		document_state& m_document_state;
		/// Stores the mesh_source property instead of the mesh itself because the k3d::mesh pointer can change
		k3d::iproperty& mesh_source_property;

		/// Stores the average component position
		k3d::point3 component_center;
		/// Stores the list of selected points with their index
		libk3dngui::detail::component_points_t selected_points;
		/// Stores the tweak array when moving components
		typedef std::vector<k3d::point3> tweaks_t;
		tweaks_t tweaks;
		/// Mutex to prevent unnecessary tweaks updates
		bool m_tweaks_mutex;
		/// Tweak change signal
		sigc::connection m_modifier_change_signal;
	};

	/// Defines a collection of objects to be manipulated interactively
	typedef std::vector<itarget*> targets_t;
	/// Stores the current set of objects to be manipulated interactively
	targets_t m_targets;

	/// Enumerates coordinate-system behaviors
	typedef enum
	{
		LOCAL,
		GLOBAL,
		PARENT
	} coordinate_system_t;

	void set_coordinate_system(const coordinate_system_t CoordinateSystem);
	void update_coordinate_system(k3d::iunknown*);

protected:
	/// Stores the owning document
	k3d::idocument& m_document;
	document_state& m_document_state;
	/// Stores a reference to the derived tool
	detail::abstract_tool& m_abstract_tool;
	/// Provides interactive navigation behavior
	navigation_input_model m_navigation_model;
	/// Dispatches incoming user input events
	basic_viewport_input_model m_input_model;

	/// Stores manipulators size
	double m_manipulators_size;

	/// Stores the target number used to provide local coordinates
	unsigned long m_current_target;
	/// Set to true when one of the targets is deleted
	bool m_deleted_target;

	/// Defines coordinate system enumeration property
	friend std::ostream& operator << (std::ostream& Stream, const coordinate_system_t& Value)
	{
		switch(Value)
		{
			case LOCAL:
				Stream << "local";
				break;
			case GLOBAL:
				Stream << "global";
				break;
			case PARENT:
				Stream << "parent";
				break;
		}
		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, coordinate_system_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "local")
			Value = LOCAL;
		else if(text == "global")
			Value = GLOBAL;
		else if(text == "parent")
			Value = PARENT;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& coordinate_system_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Local", "local", "Moves objects in local coordinate system"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Global", "global", "Moves objects in global coordinate system"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Parent", "parent", "Moves objects in parent's coordinate system"));
		}

		return values;
	}

	/// Stores the current coordinate system behavior
	k3d_data(coordinate_system_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, no_serialization) m_coordinate_system;

	/// Stores the enabled/disabled state for the manipulators
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_visible_manipulators;
};

} // namespace snap

} // namespace ngui

} // namespace module

#endif // !NGUI_SNAP_TOOL_DETAIL_H

