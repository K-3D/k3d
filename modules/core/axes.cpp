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
		\brief Implements the Axes K-3D object, which provides a configurable set of axes to help in visualizing the 3D workspace
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/ibounded.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_gl.h>
#include <k3dsdk/snap_target.h>
#include <k3dsdk/snappable.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/transformable.h>
#include <k3dsdk/vectors.h>

namespace libk3dcore
{

/////////////////////////////////////////////////////////////////////////////
// axes

class axes :
	public k3d::snappable<k3d::gl::renderable<k3d::transformable<k3d::persistent<k3d::node> > > >,
	public k3d::ibounded
{
	typedef k3d::snappable<k3d::gl::renderable<k3d::transformable<k3d::persistent<k3d::node> > > > base;

public:
	axes(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_axes(init_owner(*this) + init_name("axes") + init_label(_("Axes")) + init_description(_("Display XYZ axes")) + init_value(true)),
		m_xy_plane(init_owner(*this) + init_name("xyplane") + init_label(_("XY Plane")) + init_description(_("Display XY plane as a grid")) + init_value(true)),
		m_yz_plane(init_owner(*this) + init_name("yzplane") + init_label(_("YZ Plane")) + init_description(_("Display YZ plane as a grid")) + init_value(false)),
		m_xz_plane(init_owner(*this) + init_name("xzplane") + init_label(_("XZ Plane")) + init_description(_("Display XZ plane as a grid")) + init_value(false)),
		m_grid_size(init_owner(*this) + init_name("gridsize") + init_label(_("Grid Size")) + init_description(_("The size of each grid square")) + init_value(2.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_grid_count(init_owner(*this) + init_name("gridcount") + init_label(_("Grid Count")) + init_description(_("Number of squares along each grid")) + init_value(5) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
		m_x_color(init_owner(*this) + init_name("xcolor") + init_label(_("X Color")) + init_description(_("X axis color")) + init_value(k3d::color(1, 0, 0))),
		m_y_color(init_owner(*this) + init_name("ycolor") + init_label(_("Y Color")) + init_description(_("Y axis color")) + init_value(k3d::color(0, 0.7, 0))),
		m_z_color(init_owner(*this) + init_name("zcolor") + init_label(_("Z Color")) + init_description(_("Z axis color")) + init_value(k3d::color(0, 0, 0.7))),
		m_grid_color(init_owner(*this) + init_name("gridcolor") + init_label(_("Grid Color")) + init_description(_("Grid color")) + init_value(k3d::color(0.4, 0.4, 0.4)))
	{
		m_axes.changed_signal().connect(make_async_redraw_slot());
		m_xy_plane.changed_signal().connect(make_async_redraw_slot());
		m_yz_plane.changed_signal().connect(make_async_redraw_slot());
		m_xz_plane.changed_signal().connect(make_async_redraw_slot());
		m_grid_size.changed_signal().connect(make_async_redraw_slot());
		m_grid_count.changed_signal().connect(make_async_redraw_slot());
		m_x_color.changed_signal().connect(make_async_redraw_slot());
		m_y_color.changed_signal().connect(make_async_redraw_slot());
		m_z_color.changed_signal().connect(make_async_redraw_slot());
		m_grid_color.changed_signal().connect(make_async_redraw_slot());
		m_input_matrix.changed_signal().connect(make_async_redraw_slot());

		add_snap_target(new k3d::snap_target(_("Grid"), sigc::mem_fun(*this, &axes::grid_target_position), sigc::mem_fun(*this, &axes::grid_target_orientation)));
	}

	bool grid_target_position(const k3d::point3& Position, k3d::point3& TargetPosition)
	{
		const double grid_size = m_grid_size.pipeline_value();

		TargetPosition = k3d::point3(
			k3d::round(Position[0] / grid_size) * grid_size,
			k3d::round(Position[1] / grid_size) * grid_size,
			k3d::round(Position[2] / grid_size) * grid_size);

		return true;
	}

	bool grid_target_orientation(const k3d::point3& Position, k3d::vector3& Look, k3d::vector3& Up)
	{
		return false;
	}

	const k3d::bounding_box3 extents()
	{
		const double size = m_grid_size.pipeline_value() * m_grid_count.pipeline_value();
		return k3d::bounding_box3(size, -size, size, -size, size, -size);
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		const long grid_count = m_grid_count.pipeline_value();
		const double grid_size = m_grid_size.pipeline_value();
		const k3d::color x_color = m_x_color.pipeline_value();
		const k3d::color y_color = m_y_color.pipeline_value();
		const k3d::color z_color = m_z_color.pipeline_value();
		const k3d::color grid_color = m_grid_color.pipeline_value();

		k3d::gl::store_attributes attributes;

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);

		glLineWidth(1.0f);
		glDisable(GL_LINE_STIPPLE);

		const double size = grid_count * grid_size;

		// Draw axes and labels
		if(m_axes.pipeline_value())
		{
			// Draw X axis
			k3d::gl::color3d(x_color);
			glBegin(GL_LINE_LOOP);
			glVertex3d(-size, 0.0, 0.0);
			glVertex3d(size, 0.0, 0.0);
			glEnd();

			// Draw Y axis
			k3d::gl::color3d(y_color);
			glBegin(GL_LINE_LOOP);
			glVertex3d(0.0, -size, 0.0);
			glVertex3d(0.0, size, 0.0);
			glEnd();

			// Draw Z axis
			k3d::gl::color3d(z_color);
			glBegin(GL_LINE_LOOP);
			glVertex3d(0.0, 0.0, -size);
			glVertex3d(0.0, 0.0, size);
			glEnd();

			// Draw axis labels ...
			k3d::gl::color3d(grid_color);

			double labelposition = size * 1.1;

			glListBase(State.gl_ascii_font_list_base);

			glRasterPos3d(labelposition, 0, 0);
			glCallLists(2, GL_UNSIGNED_BYTE, "+X");
			glRasterPos3d(0, labelposition, 0);
			glCallLists(2, GL_UNSIGNED_BYTE, "+Y");
			glRasterPos3d(0, 0, labelposition);
			glCallLists(2, GL_UNSIGNED_BYTE, "+Z");

			glRasterPos3d(-labelposition, 0, 0);
			glCallLists(2, GL_UNSIGNED_BYTE, "-X");
			glRasterPos3d(0, -labelposition, 0);
			glCallLists(2, GL_UNSIGNED_BYTE, "-Y");
			glRasterPos3d(0, 0, -labelposition);
			glCallLists(2, GL_UNSIGNED_BYTE, "-Z");
		}

		// Setup grid color
		k3d::gl::color3d(grid_color);

		// Draw XY plane
		if(m_xy_plane.pipeline_value())
		{
			glBegin(GL_LINES);
			for(long i = -grid_count; i <= grid_count; ++i)
			{
				glVertex3d(i * grid_size, -size, 0.0);
				glVertex3d(i * grid_size, size, 0.0);
				glVertex3d(-size, i * grid_size, 0.0);
				glVertex3d(size, i * grid_size, 0.0);
			}
			glEnd();
		}

		// Draw YZ plane
		if(m_yz_plane.pipeline_value())
		{
			glBegin(GL_LINES);
			for(long i = -grid_count; i <= grid_count; ++i)
			{
				glVertex3d(0.0, i * grid_size, -size);
				glVertex3d(0.0, i * grid_size, size);
				glVertex3d(0.0, -size, i * grid_size);
				glVertex3d(0.0, size, i * grid_size);
			}
			glEnd();
		}

		// Draw XZ plane
		if(m_xz_plane.pipeline_value())
		{
			glBegin(GL_LINES);
			for(long i = -grid_count; i <= grid_count; ++i)
			{
				glVertex3d(i * grid_size, 0.0, -size);
				glVertex3d(i * grid_size, 0.0, size);
				glVertex3d(-size, 0.0, i * grid_size);
				glVertex3d(size, 0.0, i * grid_size);
			}
			glEnd();
		}
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
	}

	void snap(const k3d::point3& InputCoordinates, k3d::point3& SnapCoordinates, std::string& SnapDescription)
	{
		// Convert coordinates to our frame ...
		k3d::point3 input_coordinates = k3d::inverse(k3d::node_to_world_matrix(*this)) * InputCoordinates;

		// Snap coordinates ...
		input_coordinates[0] = k3d::round(input_coordinates[0] / m_grid_size.pipeline_value()) * m_grid_size.pipeline_value();
		input_coordinates[1] = k3d::round(input_coordinates[1] / m_grid_size.pipeline_value()) * m_grid_size.pipeline_value();
		input_coordinates[2] = k3d::round(input_coordinates[2] / m_grid_size.pipeline_value()) * m_grid_size.pipeline_value();

		// Convert coordinates back to world frame ...
		SnapCoordinates = k3d::node_to_world_matrix(*this) * input_coordinates;
		SnapDescription = "Axes Grid";
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<axes,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > >factory(
			k3d::classes::Axes(),
			"Axes",
			_("Configurable set of axes to help in visualizing the 3D workspace"),
			"Utility",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_axes;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_xy_plane;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_yz_plane;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_xz_plane;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_grid_size;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_grid_count;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_x_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_y_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_z_color;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_grid_color;
};

/////////////////////////////////////////////////////////////////////////////
// axes_factory

k3d::iplugin_factory& axes_factory()
{
	return axes::get_factory();
}

} // namespace libk3dcore

