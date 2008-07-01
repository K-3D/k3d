// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
// License along with this program; if not, read to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Timothy M. Shead
*/

#include <k3d-i18n-config.h>

#include <gtkmm/drawingarea.h>
#include <gtkmm/menu.h>
#include <gtkmm/box.h>
#include <gtkmm/widget.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/file_range.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/iselectable.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/module.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/property_collection.h>
#include <k3dsdk/rectangle.h>
#include <k3dsdk/selection_state_gl.h>
#include <k3dsdk/texture3.h>
#include <k3dsdk/time_source.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/utility_gl.h>

#include <k3dsdk/ngui/basic_input_model.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/menubar.h>
#include <k3dsdk/ngui/enumeration_chooser.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/ui_component.h>

#include <boost/assign/list_of.hpp>

#include <gtk/gtkgl.h>
#include <gtk/gtkmain.h>

#include <cassert>
#include <iomanip>
#include <sstream>
#include <set>

namespace module
{

namespace ngui
{

namespace uveditor
{

namespace detail
{

std::vector<const k3d::mesh*> get_selected_meshes( libk3dngui::document_state& DocumentState )
{
	const k3d::nodes_t selected_nodes = DocumentState.selected_nodes();
	k3d::imesh_source* mesh_source = 0;
	std::vector<const k3d::mesh*> meshes;
	for(k3d::nodes_t::const_iterator node = selected_nodes.begin(); node != selected_nodes.end(); ++node)
	{
		mesh_source = dynamic_cast<k3d::imesh_source*>(*node);
		if( mesh_source )
			meshes.push_back(k3d::property::pipeline_value<k3d::mesh*>(mesh_source->mesh_source_output()));
	}
	return meshes;
}


class uv_set_model : public libk3dngui::enumeration_chooser::imodel
{
public:
	uv_set_model( libk3dngui::document_state& DocumentState )
		: m_document_state( DocumentState )
	{
		m_document_state.document_selection_change_signal().connect(sigc::mem_fun(*this, &uv_set_model::on_document_selection_changed));
	}

	void on_document_selection_changed()
	{
		m_values_changed_signal.emit();
	}

	/// Returns a human-readible label for the underlying data
	virtual const Glib::ustring label()
	{
		return "UV Set";
	}

	/// Returns the set of allowed values
	virtual const k3d::ienumeration_property::enumeration_values_t enumeration_values()
	{
		std::set<std::string> texcoord_sets;
		std::vector<const k3d::mesh*> meshes = get_selected_meshes(m_document_state);
		for(std::vector<const k3d::mesh*>::const_iterator mesh_it = meshes.begin(); mesh_it != meshes.end(); ++mesh_it)
		{
			const k3d::mesh* mesh = *mesh_it;
			if(!mesh->polyhedra)
				continue;
			k3d::typed_array<k3d::texture3>* texcoords = 0;
			for(k3d::named_arrays::const_iterator array_it = mesh->polyhedra->face_varying_data.begin(); array_it != mesh->polyhedra->face_varying_data.end(); ++array_it)
			{
				texcoords = dynamic_cast< k3d::typed_array<k3d::texture3>* >(array_it->second.get());
				if( texcoords != 0 )
				{
					texcoord_sets.insert( array_it->first );
				}
			}
		}

		std::vector<k3d::ienumeration_property::enumeration_value_t> values;
		for(std::set<std::string>::const_iterator texcoord_set_it = texcoord_sets.begin(); texcoord_set_it != texcoord_sets.end(); ++texcoord_set_it)
		{
			values.push_back( k3d::ienumeration_property::enumeration_value_t( *texcoord_set_it, *texcoord_set_it, *texcoord_set_it ) );
		}
		return values;
	}

	/// Called to return the underlying data value
	virtual const k3d::string_t value()
	{
		return m_value;
	}

	/// Called to set a new data value
	virtual void set_value(const k3d::string_t& Value)
	{
		m_value = Value;
	}

	/// Makes a connection to a signal that will be emitted if the underlying data changes
	virtual sigc::connection connect_changed(const sigc::slot<void, k3d::iunknown*>& Slot)
	{
		return m_data_changed_signal.connect(Slot);
	}

	/// Makes a connection to a signal that will be emitted if the set of allowed values changes
	virtual sigc::connection connect_enumeration_values_changed(const sigc::slot<void>& Slot)
	{
		return m_values_changed_signal.connect(Slot);
	}

private:
	libk3dngui::document_state& m_document_state;

	sigc::signal<void, k3d::iunknown*> m_data_changed_signal;
	sigc::signal<void> m_values_changed_signal;

	k3d::string_t m_value;
};

}

/////////////////////////////////////////////////////////////////////////////
// panel

class panel :
	public libk3dngui::panel::control,
	public libk3dngui::ui_component,
	public Gtk::VBox
{
public:
	panel() :
		libk3dngui::ui_component("uveditor", 0),
		m_glew_context( 0 ),
		m_zoom_factor(1.0),
		m_origin(-0.5, -0.5)
	{
	}

	void initialize(libk3dngui::document_state& DocumentState, k3d::icommand_node& Parent)
	{
		m_document_state = &DocumentState;

		ui_component::set_parent("uveditor", &Parent);

		//libk3dngui::enumeration_chooser::proxy(property, state_recorder, property_name)
		m_uv_set_model = new detail::uv_set_model( DocumentState );
		m_set_chooser = Gtk::manage(new libk3dngui::enumeration_chooser::control( Parent, "Test", m_uv_set_model));
		m_drawing_area = Gtk::manage(new Gtk::DrawingArea());
		//pack_end(*Gtk::manage(new Gtk::Label(k3d::string_cast(_("Test")))), Gtk::PACK_SHRINK);
		pack_start(*m_set_chooser, Gtk::PACK_SHRINK);
		pack_start(*m_drawing_area, Gtk::PACK_EXPAND_WIDGET);

		GdkGLConfig* const config = gdk_gl_config_new_by_mode(
			static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
		return_if_fail(config);

		return_if_fail(
			gtk_widget_set_gl_capability(GTK_WIDGET(m_drawing_area->gobj()), config, m_document_state->gdkgl_share_list(), true, GDK_GL_RGBA_TYPE));

		m_drawing_area->signal_expose_event().connect(sigc::hide(sigc::mem_fun(*this, &panel::on_redraw)));

		m_document_state->document_selection_change_signal().connect(sigc::mem_fun(*this, &panel::on_document_selection_changed));

		m_input_model.connect_mbutton_start_drag(sigc::mem_fun(*this, &panel::on_start_pan));
		m_input_model.connect_mbutton_drag(sigc::mem_fun(*this, &panel::on_pan));
		m_input_model.connect_rbutton_start_drag(sigc::mem_fun(*this, &panel::on_start_zoom));
		m_input_model.connect_rbutton_drag(sigc::mem_fun(*this, &panel::on_zoom));
		m_input_model.connect_scroll(sigc::mem_fun(*this, &panel::on_scroll_zoom));

		m_drawing_area->add_events(Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);

		m_drawing_area->signal_button_press_event().connect(sigc::bind_return(sigc::mem_fun(m_input_model, &libk3dngui::basic_input_model::button_press_event), true));
		m_drawing_area->signal_button_release_event().connect(sigc::bind_return(sigc::mem_fun(m_input_model, &libk3dngui::basic_input_model::button_release_event), true));
		m_drawing_area->signal_motion_notify_event().connect(sigc::bind_return(sigc::mem_fun(m_input_model, &libk3dngui::basic_input_model::motion_notify_event), true));
		m_drawing_area->signal_scroll_event().connect(sigc::bind_return(sigc::mem_fun(m_input_model, &libk3dngui::basic_input_model::scroll_event), true));

        show_all();
	}

	const std::string panel_type()
	{
		return "uveditor";
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
	{
	        return m_focus_signal.connect(Slot);
	}

    void on_document_selection_changed()
    {
        m_drawing_area->queue_draw();
    }

	void on_start_pan(const GdkEventMotion& Event)
	{
		m_last_mouse = k3d::point2(Event.x, Event.y);
	}

	void on_pan(const GdkEventMotion& Event)
	{
		const k3d::point2 current_mouse = k3d::point2(Event.x, Event.y);
		const k3d::vector2 delta = current_mouse - m_last_mouse;

		Gtk::Allocation allocation = m_drawing_area->get_allocation();
		const k3d::double_t width = allocation.get_width();
		const k3d::double_t height = allocation.get_height();

		m_origin += delta * (1.0 / m_zoom_factor) * (1.0 / std::min(width, height));
		m_last_mouse = current_mouse;

		schedule_redraw();
	}

	void on_start_zoom(const GdkEventMotion& Event)
	{
		m_last_mouse = k3d::point2(Event.x, Event.y);
	}

	void on_zoom(const GdkEventMotion& Event)
	{
		const k3d::point2 current_mouse = k3d::point2(Event.x, Event.y);
		const k3d::vector2 delta = current_mouse - m_last_mouse;

		k3d::double_t deltaSum = -delta[0] + delta[1];
		const k3d::double_t sensitivity = 1.008;
		const k3d::double_t zoom_factor = deltaSum < 0 ? std::pow(sensitivity, std::abs(deltaSum)) : std::pow(1 / sensitivity, std::abs(deltaSum));
		m_last_mouse = current_mouse;

		m_zoom_factor *= zoom_factor;
		schedule_redraw();
	}

	void on_scroll_zoom(const GdkEventScroll& Event)
	{
		static const k3d::double_t zoom_sensitivity = 1.2;

		switch(Event.direction)
		{
			case GDK_SCROLL_UP:
				m_zoom_factor *= zoom_sensitivity;
				schedule_redraw();
				break;
			case GDK_SCROLL_DOWN:
				m_zoom_factor *= (1.0 / zoom_sensitivity);
				schedule_redraw();
				break;
			default:
				break;
		}
	}

	void schedule_redraw()
	{
		if(Glib::RefPtr<Gdk::Window> window = m_drawing_area->get_window())
		{
			Gdk::Rectangle r(0, 0, m_drawing_area->get_allocation().get_width(), m_drawing_area->get_allocation().get_height());
			window->invalidate_rect(r, false);
		}
	}

    bool on_redraw()
    {
        // If we're minimized, we're done ...
        const unsigned long width = m_drawing_area->get_width();
        const unsigned long height = m_drawing_area->get_height();
        if(!width || !height)
            return true;

        if(!is_realized())
            return true;

        GdkGLDrawable* const drawable = gtk_widget_get_gl_drawable(GTK_WIDGET(m_drawing_area->gobj()));
        return_val_if_fail(drawable, true);

        GdkGLContext* const context = gtk_widget_get_gl_context(GTK_WIDGET(m_drawing_area->gobj()));
        return_val_if_fail(context, true);

        return_val_if_fail(gdk_gl_drawable_gl_begin(drawable, context), true);

        if (!m_glew_context)
        {
            m_glew_context = new GLEWContext();
            glew_context::instance().set_context(m_glew_context);
            GLenum err = glewInit(); // needs to be called after context creation
            if (GLEW_OK != err)
            {
                k3d::log() << error << "GLEW init failed: " << glewGetErrorString(err) << std::endl;
                assert_not_reached();
            }
        }

        glew_context::instance().set_context(m_glew_context);

        glViewport(0, 0, width, height);

		// Background color shouldn't be hardcoded
        glClearColor(0.7f, 0.7f, 0.7f, 0.7f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode( GL_PROJECTION );
        glPushMatrix();
        glLoadIdentity();

		// Set a projection matrix that keeps the -1.0 .. 1.0 post transform area visible in X and Y with a fixed aspect
		if( width > height )
			glOrtho( -0.5*width/height, 0.5*width/height, 0.5, -0.5, 0.1f, 1.0f );
		else
			glOrtho( -0.5, 0.5, 0.5*height/width, -0.5*height/width, 0.1f, 1.0f );

		// Translate & scale according to pan & zoom
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glLoadIdentity();
		glScaled( m_zoom_factor, m_zoom_factor, 1.0 );
        glTranslated( m_origin[0], m_origin[1], -0.5 );
        glPointSize( 4.0f );

		// Draw a box for the 0..1 area
		glBegin( GL_LINES );
			glColor3d( 0.5, 0.5, 0.5 );

			glVertex2d( -0.1,  0.0 );
			glVertex2d(  1.1,  0.0 );

			glVertex2d( -0.1,  1.0 );
			glVertex2d(  1.1,  1.0 );

			glVertex2d(  0.0, -0.1 );
			glVertex2d(  0.0,  1.1 );

			glVertex2d(  1.0, -0.1 );
			glVertex2d(  1.0,  1.1 );
		glEnd();

		std::vector<const k3d::mesh*> meshes = detail::get_selected_meshes(*m_document_state);
		for(std::vector<const k3d::mesh*>::const_iterator mesh_it = meshes.begin(); mesh_it != meshes.end(); ++mesh_it)
        {
			const k3d::mesh* mesh = *mesh_it;
			if(!mesh->polyhedra)
				continue;

            k3d::typed_array<k3d::texture3>* texcoords = 0;
            k3d::named_arrays::const_iterator array_it = mesh->polyhedra->face_varying_data.find( m_uv_set_model->value() );
            if( array_it != mesh->polyhedra->face_varying_data.end() )
              texcoords = dynamic_cast< k3d::typed_array<k3d::texture3>* >(array_it->second.get());

            if( texcoords )
            {
				k3d::color point_color( 1.0, 1.0, 0.0 );
				k3d::color selected_point_color( 1.0, 0.0, 0.0 );
				k3d::color edge_color( 1.0, 1.0, 1.0 );
				k3d::color selected_edge_color( 1.0, 0.0, 0.0 );

				const k3d::mesh::indices_t& edge_points = *mesh->polyhedra->edge_points;
				const k3d::mesh::selection_t& edge_selection = *mesh->polyhedra->edge_selection;
				const k3d::mesh::selection_t& point_selection = *mesh->point_selection;

				const size_t edge_count = mesh->polyhedra->edge_points->size();

				// Draw polygon edges
				glBegin(GL_LINES);
				const k3d::mesh::indices_t& clockwise_edges = *mesh->polyhedra->clockwise_edges;
				k3d::gl::color3d(edge_color);
				for(size_t edge = 0; edge != edge_count; ++edge)
				{
					if( !edge_selection[edge] )
					{
						k3d::texture3 begin = (*texcoords)[edge];
						k3d::texture3 end = (*texcoords)[clockwise_edges[edge]];
						glVertex2d( begin[0], begin[1] );
						glVertex2d( end[0], end[1] );
					}
				}
				k3d::gl::color3d(selected_edge_color);
				for(size_t edge = 0; edge != edge_count; ++edge)
				{
					if( edge_selection[edge] )
					{
						k3d::texture3 begin = (*texcoords)[edge];
						k3d::texture3 end = (*texcoords)[clockwise_edges[edge]];
						glVertex2d( begin[0], begin[1] );
						glVertex2d( end[0], end[1] );
					}
				}
				glEnd();


				// Draw points on vertices
                glColor3d( 1.0, 1.0, 1.0 );
                glBegin( GL_POINTS );
				k3d::gl::color3d(point_color);
				for(size_t edge = 0; edge != edge_count; ++edge)
                {
					if( !point_selection[edge_points[edge]] )
						glVertex2d( (*texcoords)[edge].n[0], (*texcoords)[edge].n[1] );
                }
				k3d::gl::color3d(selected_point_color);
				for(size_t edge = 0; edge != edge_count; ++edge)
                {
					if( point_selection[edge_points[edge]] )
						glVertex2d( (*texcoords)[edge].n[0], (*texcoords)[edge].n[1] );
                }
                glEnd();
            }
        }

        glMatrixMode( GL_MODELVIEW );
        glPopMatrix();
        glMatrixMode( GL_PROJECTION );
        glPopMatrix();

        glFlush();

        if(gdk_gl_drawable_is_double_buffered(drawable))
            gdk_gl_drawable_swap_buffers(drawable);

        gdk_gl_drawable_gl_end(drawable);

        return true;
    }


	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<panel> factory(
			k3d::uuid(0xfc49ed0c, 0x4fc0b829, 0xb9f08d9e, 0xf924718f),
			"NGUIUVEditor",
			_("Used for editing of UV coordinates"),
			"NGUI Panels",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-type", "uveditor")("ngui:panel-label", "UVEditor"));

		return factory;
	}

private:
    sigc::signal<void> m_focus_signal;

    /// Keep track of glew initialisation
    GLEWContext* m_glew_context;

    Gtk::DrawingArea* m_drawing_area;
    libk3dngui::enumeration_chooser::control* m_set_chooser;
    detail::uv_set_model* m_uv_set_model;
	
    libk3dngui::document_state* m_document_state;
    k3d::icommand_node* m_parent;

	k3d::double_t m_zoom_factor;
	k3d::point2 m_origin;

	k3d::point2 m_last_mouse;

	libk3dngui::basic_input_model m_input_model;
};

} // namespace pipeline

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::uveditor::panel::get_factory());
K3D_MODULE_END
