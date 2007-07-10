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

#include <ngui/file_chooser_dialog.h>
#include <ngui/panel.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/log.h>
#include <k3dsdk/options.h>
#include <k3dsdk/result.h>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/drawingarea.h>

#include <cairomm/context.h>

#include <boost/assign/list_of.hpp>
#include <iostream>

namespace module
{

namespace ngui_pipeline
{

namespace detail
{

class drawing_area :
	public Gtk::DrawingArea
{
public:
	sigc::connection connect_expose_event(const sigc::slot<void, GdkEventExpose*>& Slot)
	{
		return expose_event_signal.connect(Slot);
	}

private:
	bool on_expose_event(GdkEventExpose* event)
	{
		expose_event_signal.emit(event);
		return true;
	}

	sigc::signal<void, GdkEventExpose*> expose_event_signal;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// pipeline_panel

class pipeline_panel :
	public libk3dngui::panel::control,
	public Gtk::VBox,
	public k3d::ideletable
{
public:
	pipeline_panel() :
		m_radius(0.42),
		m_lineWidth(0.05),
		m_save_png("Save PNG"),
		m_save_pdf("Save PDF"),
		m_save_ps("Save PS"),
		m_save_svg("Save SVG")
	{
		m_hbox.pack_start(m_save_png, Gtk::PACK_SHRINK);
		m_hbox.pack_start(m_save_pdf, Gtk::PACK_SHRINK);
		m_hbox.pack_start(m_save_ps, Gtk::PACK_SHRINK);
		m_hbox.pack_start(m_save_svg, Gtk::PACK_SHRINK);

		pack_start(m_hbox, Gtk::PACK_SHRINK);
		pack_start(m_drawing_area);

		m_save_png.signal_clicked().connect(sigc::mem_fun(*this, &pipeline_panel::on_save_png));
		m_save_pdf.signal_clicked().connect(sigc::mem_fun(*this, &pipeline_panel::on_save_pdf));
		m_save_ps.signal_clicked().connect(sigc::mem_fun(*this, &pipeline_panel::on_save_ps));
		m_save_svg.signal_clicked().connect(sigc::mem_fun(*this, &pipeline_panel::on_save_svg));
		m_drawing_area.connect_expose_event(sigc::mem_fun(*this, &pipeline_panel::on_draw_clock));
		Glib::signal_timeout().connect(sigc::mem_fun(*this, &pipeline_panel::on_second_elapsed), 1000);

		show_all();
	}

	~pipeline_panel()
	{
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
	{
	        return m_focus_signal.connect(Slot);
	}

	void on_save_png()
	{
		const unsigned long width = 512;
		const unsigned long height = 512;

		libk3dngui::file_chooser_dialog dialog(_("Save PNG Image:"), k3d::options::path::bitmaps(), Gtk::FILE_CHOOSER_ACTION_SAVE);
		dialog.add_pattern_filter(_("PNG Image (*.png)"), "*.png");
		dialog.add_all_files_filter();
		dialog.append_extension(".png");

		k3d::filesystem::path file_path;
		if(!dialog.get_file_path(file_path))
			return;
		dialog.hide_all();

		const Cairo::RefPtr<Cairo::Surface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, width, height);
		return_if_fail(surface);

		const Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);
		return_if_fail(context);

		context->reset_clip();
		context->scale(width, height);
		draw_clock(context);

		surface->write_to_png(file_path.native_filesystem_string());
	}

	void on_save_pdf()
	{
		const double width = 5 * 72.0;
		const double height = 5 * 72.0;

		libk3dngui::file_chooser_dialog dialog(_("Save PDF Document:"), k3d::options::path::bitmaps(), Gtk::FILE_CHOOSER_ACTION_SAVE);
		dialog.add_pattern_filter(_("PDF Document (*.pdf)"), "*.pdf");
		dialog.add_all_files_filter();
		dialog.append_extension(".pdf");

		k3d::filesystem::path file_path;
		if(!dialog.get_file_path(file_path))
			return;
		dialog.hide_all();

		const Cairo::RefPtr<Cairo::Surface> surface = Cairo::PdfSurface::create(file_path.native_filesystem_string(), width, height);
		return_if_fail(surface);

		const Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);
		return_if_fail(context);

		context->reset_clip();
		context->scale(width, height);
		draw_clock(context);
	}

	void on_save_ps()
	{
		const double width = 5 * 72.0;
		const double height = 5 * 72.0;

		libk3dngui::file_chooser_dialog dialog(_("Save Postscript Document:"), k3d::options::path::bitmaps(), Gtk::FILE_CHOOSER_ACTION_SAVE);
		dialog.add_pattern_filter(_("Postscript Document (*.ps)"), "*.ps");
		dialog.add_all_files_filter();
		dialog.append_extension(".ps");

		k3d::filesystem::path file_path;
		if(!dialog.get_file_path(file_path))
			return;
		dialog.hide_all();

		const Cairo::RefPtr<Cairo::Surface> surface = Cairo::PsSurface::create(file_path.native_filesystem_string(), width, height);
		return_if_fail(surface);

		const Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);
		return_if_fail(context);

		context->reset_clip();
		context->scale(width, height);
		draw_clock(context);
	}

	void on_save_svg()
	{
		const double width = 5 * 72.0;
		const double height = 5 * 72.0;

		libk3dngui::file_chooser_dialog dialog(_("Save SVG Document:"), k3d::options::path::bitmaps(), Gtk::FILE_CHOOSER_ACTION_SAVE);
		dialog.add_pattern_filter(_("SVG Document (*.svg)"), "*.svg");
		dialog.add_all_files_filter();
		dialog.append_extension(".svg");

		k3d::filesystem::path file_path;
		if(!dialog.get_file_path(file_path))
			return;
		dialog.hide_all();

		const Cairo::RefPtr<Cairo::Surface> surface = Cairo::SvgSurface::create(file_path.native_filesystem_string(), width, height);
		return_if_fail(surface);

		const Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create(surface);
		return_if_fail(context);

		context->reset_clip();
		context->scale(width, height);
		draw_clock(context);
	}

	bool on_second_elapsed()
	{
		if(Glib::RefPtr<Gdk::Window> window = m_drawing_area.get_window())
		{
			Gdk::Rectangle r(0, 0, m_drawing_area.get_allocation().get_width(), m_drawing_area.get_allocation().get_height());
			window->invalidate_rect(r, false);
		}

		return true;
	}

	void on_draw_clock(GdkEventExpose* event)
	{
		if(Glib::RefPtr<Gdk::Window> window = m_drawing_area.get_window())
		{
			Gtk::Allocation allocation = m_drawing_area.get_allocation();
			const int width = allocation.get_width();
			const int height = allocation.get_height();

			Cairo::RefPtr<Cairo::Context> context = window->create_cairo_context();

			if(event)
			{
				// clip to the area indicated by the expose event so that we only
				// redraw the portion of the window that needs to be redrawn
				context->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
				context->clip();
			}

			context->scale(width, height);

			draw_clock(context);
		}
	}

	void draw_clock(const Cairo::RefPtr<Cairo::Context> cr)
	{
		cr->save();

		// Translate (0, 0) to be (0.5, 0.5), i.e. the center of the window
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

/*
		// Draw some text
		cr->select_font_face("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
		cr->set_font_size(0.15);
		cr->move_to(-0.4, 0);
		cr->show_text("Howdy!");
*/

		cr->restore();
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

	Gtk::HBox m_hbox;
	Gtk::Button m_save_png;
	Gtk::Button m_save_pdf;
	Gtk::Button m_save_ps;
	Gtk::Button m_save_svg;
	detail::drawing_area m_drawing_area;
};

/////////////////////////////////////////////////////////////////////////////
// pipeline_panel_factory

k3d::iplugin_factory& pipeline_panel_factory()
{
	return pipeline_panel::get_factory();
}

} // namespace ngui_pipeline

} // namespace module

