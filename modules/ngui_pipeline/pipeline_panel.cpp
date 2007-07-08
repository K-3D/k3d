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

#include <ngui/panel.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/log.h>

#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>

#include <boost/assign/list_of.hpp>
#include <iostream>

namespace module
{

namespace ngui_pipeline
{

/////////////////////////////////////////////////////////////////////////////
// pipeline_panel

class pipeline_panel :
	public libk3dngui::panel::control,
	public Gtk::DrawingArea,
	public k3d::ideletable
{
public:
	pipeline_panel() :
		m_radius(0.42), m_lineWidth(0.05)
	{
		Glib::signal_timeout().connect(sigc::mem_fun(*this, &pipeline_panel::onSecondElapsed), 1000);
		show_all();
	}

	~pipeline_panel()
	{
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
	{
	        return m_focus_signal.connect(Slot);
	}

	bool onSecondElapsed()
	{
		Glib::RefPtr<Gdk::Window> win = get_window();
		if (win)
		{
			Gdk::Rectangle r(0, 0, get_allocation().get_width(),
			get_allocation().get_height());
			win->invalidate_rect(r, false);
		}

		return true;
	}

	bool on_expose_event(GdkEventExpose* event)
	{
		Glib::RefPtr<Gdk::Window> window = get_window();
		if(window)
		{
			Gtk::Allocation allocation = get_allocation();
			const int width = allocation.get_width();
			const int height = allocation.get_height();

			Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

			if (event)
			{
				// clip to the area indicated by the expose event so that we only
				// redraw the portion of the window that needs to be redrawn
				cr->rectangle(event->area.x, event->area.y,
				event->area.width, event->area.height);
				cr->clip();
			}

			// scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
			// the center of the window
			cr->scale(width, height);
			cr->translate(0.5, 0.5);
			cr->set_line_width(m_lineWidth);

			cr->save();
			cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
			cr->paint();
			cr->restore();
			cr->arc(0, 0, m_radius, 0, 2 * M_PI);
			cr->save();
			cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
			cr->fill_preserve();
			cr->restore();
			cr->stroke_preserve();
			cr->clip();

			//clock ticks
			for (int i = 0; i < 12; i++)
			{
				double inset = 0.05;

				cr->save();
				cr->set_line_cap(Cairo::LINE_CAP_ROUND);

				if (i % 3 != 0)
				{
					inset *= 0.8;
					cr->set_line_width(0.03);
				}

				cr->move_to(
				(m_radius - inset) * cos (i * M_PI / 6),
				(m_radius - inset) * sin (i * M_PI / 6));
				cr->line_to (
				m_radius * cos (i * M_PI / 6),
				m_radius * sin (i * M_PI / 6));
				cr->stroke();
				cr->restore(); 
			}

			// store the current time
			time_t rawtime;
			time(&rawtime);
			struct tm * timeinfo = localtime (&rawtime);

			// compute the angles of the indicators of our clock
			double minutes = timeinfo->tm_min * M_PI / 30;
			double hours = timeinfo->tm_hour * M_PI / 6;
			double seconds= timeinfo->tm_sec * M_PI / 30;

			cr->save();
			cr->set_line_cap(Cairo::LINE_CAP_ROUND);

			// draw the seconds hand
			cr->save();
			cr->set_line_width(m_lineWidth / 3);
			cr->set_source_rgba(0.7, 0.7, 0.7, 0.8); // gray
			cr->move_to(0, 0);
			cr->line_to(sin(seconds) * (m_radius * 0.9), 
			-cos(seconds) * (m_radius * 0.9));
			cr->stroke();
			cr->restore();

			// draw the minutes hand
			cr->set_source_rgba(0.117, 0.337, 0.612, 0.9);   // blue
			cr->move_to(0, 0);
			cr->line_to(sin(minutes + seconds / 60) * (m_radius * 0.8),
			-cos(minutes + seconds / 60) * (m_radius * 0.8));
			cr->stroke();

			// draw the hours hand
			cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
			cr->move_to(0, 0);
			cr->line_to(sin(hours + minutes / 12.0) * (m_radius * 0.5),
			-cos(hours + minutes / 12.0) * (m_radius * 0.5));
			cr->stroke();
			cr->restore();

			// draw a little dot in the middle
			cr->arc(0, 0, m_lineWidth / 3.0, 0, 2 * M_PI);
			cr->fill();
		}

		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<pipeline_panel> factory(
				k3d::uuid(0xf37f08af, 0xf542d517, 0xde317099, 0x79394ed8),
				"NGUIPipelinePanel",
				_("Provides a panel for displaying the visualization pipeline"),
				"NGUI Panels",
				k3d::iplugin_factory::EXPERIMENTAL,
				boost::assign::map_list_of("NextGenerationUI", "true")("component_type", "panel")("panel_name", "pipeline_panel")("panel_label", "Pipeline"));

		return factory;
	}

private:
	sigc::signal<void> m_focus_signal;
	double m_radius;
	double m_lineWidth;
};

/////////////////////////////////////////////////////////////////////////////
// pipeline_panel_factory

k3d::iplugin_factory& pipeline_panel_factory()
{
	return pipeline_panel::get_factory();
}

} // namespace ngui_pipeline

} // namespace module

