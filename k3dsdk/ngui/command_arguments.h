#ifndef NGUI_COMMAND_ARGUMENTS_H
#define NGUI_COMMAND_ARGUMENTS_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
*/

#include <gdk/gdkevents.h>

#include <k3dsdk/algebra.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/keyboard.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/vectors.h>

namespace k3d { class icommand_node; }
namespace k3d { class idocument; }
namespace k3d { class inode; }
namespace k3d { namespace xml { class element; } }

namespace libk3dngui
{

namespace viewport { class control; }

/////////////////////////////////////////////////////////////////////////////
// command_arguments

/// Helper class to streamline serializing command-node arguments for tutorials
class command_arguments
{
public:
	/// Use this constructor when serializing arguments
	command_arguments();
	/// Use this constructor when parsing arguments
	command_arguments(const std::string& Arguments);

	~command_arguments();

	void append_viewport(viewport::control& Viewport);
	void append_viewport_coordinates(const std::string& Name, viewport::control& Viewport, const GdkEventButton& Event);
	void append_viewport_coordinates(const std::string& Name, viewport::control& Viewport, const GdkEventMotion& Event);
	void append_viewport_coordinates(const std::string& Name, viewport::control& Viewport, const GdkEventScroll& Event);
	void append_viewport_coordinates(const std::string& Name, viewport::control& Viewport, const k3d::rectangle& Value);
	void append(const std::string& Name, const bool& Value);
	void append(const std::string& Name, const char* const Value);
	void append(const std::string& Name, const double& Value);
	void append(const std::string& Name, const k3d::angle_axis& Value);
	void append(const std::string& Name, const k3d::matrix4& Value);
	void append(const std::string& Name, const k3d::point2& Value);
	void append(const std::string& Name, const k3d::point3& Value);
	void append(const std::string& Name, const k3d::rectangle& Value);
	void append(const std::string& Name, const k3d::selection::record& Value);
	void append(const std::string& Name, const k3d::selection::records& Value);
	void append(const std::string& Name, const k3d::vector2& Value);
	void append(const std::string& Name, const k3d::vector3& Value);
	void append(const std::string& Name, const std::string& Value);
	void append(const std::string& Name, k3d::inode* const Value);
	void append(const k3d::xml::element& Value);

	/// Serializes arguments as a string
	operator std::string() const;

	/// Returns the viewport, or throws std::runtime_error
	viewport::control& get_viewport() const;
	/// Returns viewport coordinates, or throws std::runtime_error
	const k3d::point2 get_viewport_point2(const std::string& Name = "mouse") const;
	/// Returns viewport coordinates, or throws std::runtime_error
	const k3d::rectangle get_viewport_rectangle(const std::string& Name) const;
	/// Returns a bool, or throws std::runtime_error
	const bool get_bool(const std::string& Name) const;
	/// Returns a double, or throws std::runtime_error
	const double get_double(const std::string& Name) const;
	/// Returns a matrix4, or throws std::runtime_error
	const k3d::matrix4 get_matrix4(const std::string& Name) const;
	/// Returns a point2, or throws std::runtime_error
	const k3d::point2 get_point2(const std::string& Name) const;
	/// Returns a point3, or throws std::runtime_error
	const k3d::point3 get_point3(const std::string& Name) const;
	/// Returns a vector2, or throws std::runtime_error
	const k3d::vector2 get_vector2(const std::string& Name) const;
	/// Returns a vector3, or throws std::runtime_error
	const k3d::vector3 get_vector3(const std::string& Name) const;
	/// Returns a rectangle, or throws std::runtime_error
	const k3d::rectangle get_rectangle(const std::string& Name) const;
	/// Returns an angle_axis, or throws std::runtime_error
	const k3d::angle_axis get_angle_axis(const std::string& Name) const;
	/// Returns a string
	const std::string get_string(const std::string& Name) const;
	/// Returns a selection record, or throws std::runtime_error
	const k3d::selection::record get_selection_record(k3d::idocument& Document, const std::string& Name) const;
	/// Returns a set of selection records, or throws std::runtime_error
	const k3d::selection::records get_selection_records(k3d::idocument& Document, const std::string& Name) const;
	/// Returns a node, or throws std::runtime_error
	k3d::inode* const get_node(k3d::idocument& Document, const std::string& Name) const;
	/// Returns an XML element, or throws std::runtime_error
	const k3d::xml::element get_element(const std::string& Name) const;

private:
	k3d::xml::element* get_storage(const std::string& Name) const;
	k3d::xml::element* const m_storage;
};

} // namespace libk3dngui

#endif // NGUI_COMMAND_ARGUMENTS_H

