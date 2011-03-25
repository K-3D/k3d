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

#include <k3dsdk/color.h>
#include <k3dsdk/data.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/types.h>
#include <k3dsdk/vector3.h>

namespace module
{

namespace qtui
{

namespace move
{

class move_manipulators
{
public:
	enum constraints_t {NO, XX, YY, ZZ, XY, XZ, YZ, SC};
	
	move_manipulators();
	~move_manipulators();
	
	void draw(const constraints_t Constraints);
	
	void set_camera(k3d::icamera* Camera);
	
private:
	/// Calculate scaling factor for constant size
	k3d::double_t scale(const k3d::point3& Position);
	
	/// Draws a single axis along the given vector
	void draw_axis(const k3d::color& Color, const k3d::vector3& Vector, const k3d::bool_t Selected);
	
	k3d_data(k3d::point3, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_position;
	k3d_data(k3d::double_t, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_size;
	
	k3d_data(k3d::vector3, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_x_axis;
	k3d_data(k3d::vector3, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_y_axis;
	k3d_data(k3d::vector3, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_z_axis;
	
	k3d_data(k3d::color, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_x_color;
	k3d_data(k3d::color, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_y_color;
	k3d_data(k3d::color, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_z_color;
	k3d_data(k3d::color, no_name, change_signal, no_undo, local_storage, no_constraint, no_property, no_serialization) m_selected_color;
	
	k3d_data(k3d::icamera*, no_name, change_signal, no_undo, node_storage, no_constraint, no_property, no_serialization) m_camera;
	
	GLUquadric* m_quadric;
	
	// TODO: These should be properties
	k3d::double_t m_axis_arrow_radius;
	k3d::double_t m_axis_arrow_length;
	k3d::int32_t m_axis_arrow_slices;
	k3d::double_t m_axis_line_width;
};

} // namespace move

} // namespace qtui

} // namespace module
