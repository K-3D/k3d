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

#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/ui_component.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/graph.h>
#include <k3dsdk/ideletable.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/log.h>
#include <k3dsdk/options.h>
#include <k3dsdk/module.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/result.h>
#include <k3dsdk/vectors.h>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/drawingarea.h>

#include <cairomm/context.h>

#include <boost/assign/list_of.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <cmath>
#include <iostream>

// Temporary hack ...
using namespace libk3dngui;

namespace module
{

namespace ngui_pipeline
{

namespace detail
{

/// Drawing area widget that emits a signal when it needs to be redrawn
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

struct point
{
	double x;
	double y;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// panel

class panel :
	public libk3dngui::panel::control,
	public libk3dngui::ui_component,
	public k3d::ideletable,
	public Gtk::VBox
{
public:
	panel() :
		ui_component("pipeline", 0),
		m_save_png("Save PNG"),
		m_save_pdf("Save PDF"),
		m_save_ps("Save PS"),
		m_save_svg("Save SVG"),
		m_zoom_factor(1.0),
		m_document_state(0)
	{
		m_hbox.pack_start(m_save_png, Gtk::PACK_SHRINK);
		m_hbox.pack_start(m_save_pdf, Gtk::PACK_SHRINK);
		m_hbox.pack_start(m_save_ps, Gtk::PACK_SHRINK);
		m_hbox.pack_start(m_save_svg, Gtk::PACK_SHRINK);

		pack_start(m_hbox, Gtk::PACK_SHRINK);
		pack_start(m_drawing_area);

		m_save_png.signal_clicked().connect(sigc::mem_fun(*this, &panel::on_save_png));
		m_save_pdf.signal_clicked().connect(sigc::mem_fun(*this, &panel::on_save_pdf));
		m_save_ps.signal_clicked().connect(sigc::mem_fun(*this, &panel::on_save_ps));
		m_save_svg.signal_clicked().connect(sigc::mem_fun(*this, &panel::on_save_svg));

		m_drawing_area.add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK | Gdk::BUTTON3_MOTION_MASK | Gdk::SCROLL_MASK);
		m_drawing_area.connect_expose_event(sigc::mem_fun(*this, &panel::on_draw_pipeline));
		m_drawing_area.signal_button_press_event().connect(sigc::bind_return(sigc::mem_fun(*this, &panel::on_button_press), true));
		m_drawing_area.signal_motion_notify_event().connect(sigc::bind_return(sigc::mem_fun(*this, &panel::on_mouse_motion), true));
		m_drawing_area.signal_scroll_event().connect(sigc::bind_return(sigc::mem_fun(*this, &panel::on_mouse_scroll), true));
	}

	void initialize(libk3dngui::document_state& DocumentState, k3d::icommand_node& Parent)
	{
		ui_component::set_parent("pipeline", &Parent);
		m_document_state = &DocumentState;
		
		show_all();
	}

	const std::string panel_type()
	{
		return "pipeline";
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

		draw_pipeline(context, width, height);

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

		draw_pipeline(context, width, height);
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

		draw_pipeline(context, width, height);
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

		draw_pipeline(context, width, height);
	}

	void redraw_panel()
	{
		if(Glib::RefPtr<Gdk::Window> window = m_drawing_area.get_window())
		{
			Gdk::Rectangle r(0, 0, m_drawing_area.get_allocation().get_width(), m_drawing_area.get_allocation().get_height());
			window->invalidate_rect(r, false);
		}
	}

	void on_draw_pipeline(GdkEventExpose* event)
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

			draw_pipeline(context, width, height);
		}
	}

	void on_button_press(GdkEventButton* Event)
	{
		m_last_mouse = k3d::point2(Event->x, Event->y);
	}

	void on_mouse_motion(GdkEventMotion* Event)
	{
//		if(Event->button == 2)
		{
			const k3d::point2 current_mouse = k3d::point2(Event->x, Event->y);
			const k3d::point2 delta = current_mouse - m_last_mouse;

			Gtk::Allocation allocation = m_drawing_area.get_allocation();
			const double width = allocation.get_width();
			const double height = allocation.get_height();

			m_origin += delta * (1.0 / m_zoom_factor) * (1.0 / std::min(width, height));
			m_last_mouse = current_mouse;

			redraw_panel();
		}
	}

	void on_mouse_scroll(GdkEventScroll* Event)
	{
		static const double zoom_sensitivity = 1.2;

		switch(Event->direction)
		{
			case GDK_SCROLL_UP:
				m_zoom_factor *= zoom_sensitivity;
				redraw_panel();
				break;
			case GDK_SCROLL_DOWN:
				m_zoom_factor *= (1.0 / zoom_sensitivity);
				redraw_panel();
				break;
			default:
				break;
		}
	}

	void draw_pipeline(const Cairo::RefPtr<Cairo::Context> Context, const double Width, const double Height)
	{
return_if_fail(m_document_state);

		Context->save();

		// Setup the viewport ...
		if(Width > Height)
		{
			Context->translate((Width - Height) / 2, 0);
			Context->scale(Height, Height);
		}
		else
		{
			Context->translate(0, (Height - Width) / 2);
			Context->scale(Width, Width);
		}

		Context->translate(0.5, 0.5);
		Context->scale(m_zoom_factor, m_zoom_factor);
		Context->translate(m_origin[0], m_origin[1]);

		// Clear the background ...
		Context->save();
		Context->set_source_rgb(0.8, 0.8, 0.8);
		Context->paint();
		Context->restore();

		// Generate a sample graph ...
		k3d::graph graph;
		{
/*
			std::vector<std::string> sample_vertices = boost::assign::list_of("a")("b")("c")("d");
			std::multimap<size_t, size_t> sample_edges = boost::assign::map_list_of(0, 1)(0, 2)(1, 2)(2, 3)(0, 1);
*/
			boost::shared_ptr<k3d::graph::topology_t> topology(new k3d::graph::topology_t());
			boost::shared_ptr<k3d::graph::strings_t> vertex_labels(new k3d::graph::strings_t());

			const k3d::nodes_t nodes = m_document_state->document().nodes().collection();
			for(k3d::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
			{
				boost::add_vertex(*topology);
				vertex_labels->push_back((*node)->name());
			}
/*
			for(size_t i = 0; i != sample_vertices.size(); ++i)
			{
				boost::add_vertex(*topology);
				vertex_labels->push_back(sample_vertices[i]);
			}

			for(std::multimap<size_t, size_t>::iterator edge = sample_edges.begin(); edge != sample_edges.end(); ++edge)
			{
				boost::add_edge(edge->first, edge->second, *topology);
			}
*/

			graph.topology = topology;
			graph.vertex_data["labels"] = vertex_labels;
		}

/*
		// Create a (dirt simple) layout for the graph ...
		{
			boost::shared_ptr<k3d::graph::points_t> vertex_positions(new k3d::graph::points_t());

			const size_t vertex_count = boost::num_vertices(*graph.topology);
			for(size_t i = 0; i != vertex_count; ++i)
				vertex_positions->push_back(k3d::point2(i * 0.1, 0));

			graph.vertex_data["positions"] = vertex_positions;
		}
*/

		// Do a simple layout for the graph ...
		{
			return_if_fail(graph.topology);

			const k3d::graph::topology_t& topology = *graph.topology;
			boost::shared_ptr<k3d::typed_array<detail::point> > vertex_positions(new k3d::typed_array<detail::point>(boost::num_vertices(topology)));
			k3d::typed_array<detail::point>& position_map = *vertex_positions;

			const double pi = 3.14159;
			const size_t n = boost::num_vertices(topology);
			for(size_t i = 0; i != n; ++i)
			{
				position_map[i].x = 0.5 * std::cos(i * 2 * pi / n);
				position_map[i].y = 0.5 * std::sin(i * 2 * pi / n);
			}

//			boost::circle_graph_layout(topology, position_map, 0.5);
//			boost::mt19937 rng;
//			boost::random_graph_layout(topology, position_map, 0.0, 1.0, 0.0, 1.0, rng);

			graph.vertex_data["positions"] = vertex_positions;
		}

		// Render the graph edges ...
		try
		{
			return_if_fail(graph.topology);
			return_if_fail(graph.vertex_data.count("positions"));

			const k3d::graph::topology_t& topology = *graph.topology;
			const k3d::typed_array<detail::point>& vertex_positions = dynamic_cast<k3d::typed_array<detail::point>&>(*graph.vertex_data["positions"].get());
			return_if_fail(boost::num_vertices(topology) == vertex_positions.size());

			Context->save();
			Context->set_line_width(0.003);
			Context->set_source_rgba(0, 0, 0, 0.3);
			Context->set_line_cap(Cairo::LINE_CAP_ROUND);

			typedef boost::graph_traits<k3d::graph::topology_t>::edge_iterator iter_t;
			for(std::pair<iter_t, iter_t> edges = boost::edges(topology); edges.first != edges.second; ++edges.first)
			{
				const size_t source = boost::source(*edges.first, topology);
				const size_t target = boost::target(*edges.first, topology);

				Context->move_to(vertex_positions[source].x, vertex_positions[source].y);
				Context->line_to(vertex_positions[target].x, vertex_positions[target].y);
				Context->stroke();
			}

			Context->restore();
		}
		catch(std::exception& e)
		{
			k3d::log() << error << "caught exception: " << e.what() << std::endl;
		}
		catch(...)
		{
			k3d::log() << error << "caught unknown exception" << std::endl;
		}

		// Render the graph vertices ...
		try
		{
			return_if_fail(graph.vertex_data.count("labels"));
			return_if_fail(graph.vertex_data.count("positions"));

			const k3d::graph::strings_t& vertex_labels = dynamic_cast<k3d::graph::strings_t&>(*graph.vertex_data["labels"].get());
			const k3d::typed_array<detail::point>& vertex_positions = dynamic_cast<k3d::typed_array<detail::point>&>(*graph.vertex_data["positions"].get());
			return_if_fail(vertex_labels.size() == vertex_positions.size());

			Context->save();

			Context->select_font_face("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
			Context->set_font_size(0.03);
			
			for(size_t i = 0; i != vertex_labels.size(); ++i)
			{
				Context->move_to(vertex_positions[i].x, vertex_positions[i].y);
				Context->show_text(vertex_labels[i]);
			}

			Context->restore();
		}
		catch(std::exception& e)
		{
			k3d::log() << error << "caught exception: " << e.what() << std::endl;
		}
		catch(...)
		{
			k3d::log() << error << "caught unknown exception" << std::endl;
		}

/*
		Context->set_line_width(m_lineWidth);

		Context->arc(0, 0, m_radius, 0, 2 * M_PI);
		Context->save();
		Context->set_source_rgba(1.0, 1.0, 1.0, 0.8);
		Context->fill_preserve();
		Context->restore();
		Context->stroke_preserve();
		Context->clip();

		//clock ticks
		for (int i = 0; i < 12; i++)
		{
			double inset = 0.05;

			Context->save();
			Context->set_line_cap(Cairo::LINE_CAP_ROUND);

			if (i % 3 != 0)
			{
				inset *= 0.8;
				Context->set_line_width(0.03);
			}

			Context->move_to(
			(m_radius - inset) * cos (i * M_PI / 6),
			(m_radius - inset) * sin (i * M_PI / 6));
			Context->line_to (
			m_radius * cos (i * M_PI / 6),
			m_radius * sin (i * M_PI / 6));
			Context->stroke();
			Context->restore(); 
		}

		// store the current time
		time_t rawtime;
		time(&rawtime);
		struct tm * timeinfo = localtime (&rawtime);

		// compute the angles of the indicators of our clock
		double minutes = timeinfo->tm_min * M_PI / 30;
		double hours = timeinfo->tm_hour * M_PI / 6;
		double seconds= timeinfo->tm_sec * M_PI / 30;

		Context->save();
		Context->set_line_cap(Cairo::LINE_CAP_ROUND);

		// draw the seconds hand
		Context->save();
		Context->set_line_width(m_lineWidth / 3);
		Context->set_source_rgba(0.7, 0.7, 0.7, 0.8); // gray
		Context->move_to(0, 0);
		Context->line_to(sin(seconds) * (m_radius * 0.9), 
		-cos(seconds) * (m_radius * 0.9));
		Context->stroke();
		Context->restore();

		// draw the minutes hand
		Context->set_source_rgba(0.117, 0.337, 0.612, 0.9);   // blue
		Context->move_to(0, 0);
		Context->line_to(sin(minutes + seconds / 60) * (m_radius * 0.8),
		-cos(minutes + seconds / 60) * (m_radius * 0.8));
		Context->stroke();

		// draw the hours hand
		Context->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
		Context->move_to(0, 0);
		Context->line_to(sin(hours + minutes / 12.0) * (m_radius * 0.5),
		-cos(hours + minutes / 12.0) * (m_radius * 0.5));
		Context->stroke();
		Context->restore();

		// draw a little dot in the middle
		Context->arc(0, 0, m_lineWidth / 3.0, 0, 2 * M_PI);
		Context->fill();

		// Draw some text
		Context->select_font_face("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
		Context->set_font_size(0.15);
		Context->move_to(-0.4, 0);
		Context->show_text("Howdy!");
*/

		Context->restore();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<panel> factory(
				k3d::uuid(0xf37f08af, 0xf542d517, 0xde317099, 0x79394ed8),
				"NGUIPipelinePanel",
				_("Provides a panel for displaying the visualization pipeline"),
				"NGUI Panels",
				k3d::iplugin_factory::EXPERIMENTAL,
				boost::assign::map_list_of("NextGenerationUI", "true")("component_type", "panel")("panel_type", "pipeline")("panel_label", "Pipeline"));

		return factory;
	}

private:
	sigc::signal<void> m_focus_signal;

	Gtk::HBox m_hbox;
	Gtk::Button m_save_png;
	Gtk::Button m_save_pdf;
	Gtk::Button m_save_ps;
	Gtk::Button m_save_svg;
	detail::drawing_area m_drawing_area;

	double m_zoom_factor;
	k3d::point2 m_origin;

	k3d::point2 m_last_mouse;

	document_state* m_document_state;
};

} // namespace ngui_pipeline

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui_pipeline::panel::get_factory());
K3D_MODULE_END

