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

#include "graph_operations.h"

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

#include <iostream>
#include <valarray>

// Temporary hack ...
using namespace libk3dngui;

namespace module
{

namespace ngui
{

namespace pipeline
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

		m_drawing_area.add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK | Gdk::BUTTON3_MOTION_MASK | Gdk::SCROLL_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
		m_drawing_area.connect_expose_event(sigc::mem_fun(*this, &panel::on_draw_pipeline));
		m_drawing_area.signal_button_press_event().connect(sigc::bind_return(sigc::mem_fun(*this, &panel::on_button_press), true));
		m_drawing_area.signal_motion_notify_event().connect(sigc::bind_return(sigc::mem_fun(*this, &panel::on_mouse_motion), true));
		m_drawing_area.signal_scroll_event().connect(sigc::bind_return(sigc::mem_fun(*this, &panel::on_mouse_scroll), true));
	}

	void initialize(libk3dngui::document_state& DocumentState, k3d::icommand_node& Parent)
	{
		ui_component::set_parent("pipeline", &Parent);
		m_document_state = &DocumentState;
		
		m_document_state->document().nodes().add_nodes_signal().connect(sigc::hide(sigc::mem_fun(*this, &panel::reset_graph)));
		m_document_state->document().nodes().remove_nodes_signal().connect(sigc::hide(sigc::mem_fun(*this, &panel::reset_graph)));
		m_document_state->document().nodes().rename_node_signal().connect(sigc::hide(sigc::mem_fun(*this, &panel::reset_graph)));

		Glib::signal_timeout().connect(sigc::bind_return(sigc::mem_fun(*this, &panel::update_layout), true), 100);

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
		const unsigned long height = static_cast<unsigned long>(width * aspect_ratio());

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
		const double height = width * aspect_ratio();

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
		const double height = width * aspect_ratio();

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
		const double height = width * aspect_ratio();

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

	void reset_graph()
	{
		m_graph.reset();
		redraw_panel();
	}

	k3d::graph& get_graph()
	{
		// Create the visualization graph if it doesn't exist ...
		if(!m_graph)
		{
			m_graph.reset(new k3d::graph());
			create_graph(*m_document_state, *m_graph);
			random_layout(*m_graph);
		}

		return *m_graph;
	}

	void update_layout()
	{
		force_directed_layout(get_graph());
		redraw_panel();
	}

	const double aspect_ratio()
	{
		Gtk::Allocation allocation = m_drawing_area.get_allocation();
		const double width = allocation.get_width();
		const double height = allocation.get_height();
		return height / width;
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
			const k3d::vector2 delta = current_mouse - m_last_mouse;

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

	void draw_arrow(const Cairo::RefPtr<Cairo::Context>& Context, const k3d::point2& Source, const k3d::point2& Target, const double Size)
	{
		if(!k3d::length(Source - Target))
			return;

		const k3d::vector2 length_vector = k3d::normalize(Source - Target);
		const k3d::vector2 width_vector = k3d::perpendicular(length_vector);

		const k3d::point2 a = Source;
		const k3d::point2 b = Target + (Size * length_vector);
		const k3d::point2 c = Target + (Size * length_vector) + (0.3 * Size * width_vector);
		const k3d::point2 d = Target;
		const k3d::point2 e = Target + (Size * length_vector) + (-0.3 * Size * width_vector);

		Context->set_line_cap(Cairo::LINE_CAP_BUTT);
		Context->set_line_join(Cairo::LINE_JOIN_MITER);

		// Draw the line ...
		Context->move_to(a[0], a[1]);
		Context->line_to(b[0], b[1]);
		Context->stroke();

		// Draw the arrow ...
		Context->move_to(c[0], c[1]);
		Context->line_to(d[0], d[1]);
		Context->line_to(e[0], e[1]);
		Context->line_to(c[0], c[1]);
		Context->fill();
	}

	void draw_box_to_box_arrow(const Cairo::RefPtr<Cairo::Context>& Context, const k3d::point2& Source, const double SourceWidth, const double SourceHeight, const k3d::point2& Target, const double TargetWidth, const double TargetHeight, const double Size)
	{
		k3d::point2 source(Source);
		k3d::point2 target(Target);

/*
		if(source[1] < target[1])
		{
			source[1] += SourceHeight / 2;
			target[1] -= TargetHeight / 2;
		}
		else
		{
			source[1] -= SourceHeight / 2;
			target[1] += TargetHeight / 2;
		}
*/

		draw_arrow(Context, source, target, Size);
	}

	void draw_centered_text(const Cairo::RefPtr<Cairo::Context>& Context, const k3d::point2& Position, const std::string& Text, double& Width, double& Height)
	{
		Context->save();

		Cairo::TextExtents extents;
		Context->get_text_extents(Text, extents);

		Context->translate(-extents.width / 2, extents.height / 2);
		Context->move_to(Position[0], Position[1]);
		Context->show_text(Text);

		Context->restore();

		Width = extents.width;
		Height = extents.height;
	}

	void draw_pipeline(const Cairo::RefPtr<Cairo::Context>& Context, const double Width, const double Height)
	{
		return_if_fail(m_document_state);

		k3d::graph& graph = get_graph();

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

		// Render the graph vertices ...
		try
		{
			return_if_fail(graph.topology);
			return_if_fail(graph.vertex_data.count("label"));
			return_if_fail(graph.vertex_data.count("position"));

			const k3d::graph::topology_t& topology = *graph.topology;

			const k3d::graph::strings_t& vertex_label = dynamic_cast<k3d::graph::strings_t&>(*graph.vertex_data["label"].get());
			const k3d::graph::points_t& vertex_position = dynamic_cast<k3d::graph::points_t&>(*graph.vertex_data["position"].get());
			k3d::graph::doubles_t& vertex_width = get_array<k3d::graph::doubles_t>(graph.vertex_data, "width", boost::num_vertices(topology));
			k3d::graph::doubles_t& vertex_height = get_array<k3d::graph::doubles_t>(graph.vertex_data, "height", boost::num_vertices(topology));

			Context->save();

			Context->select_font_face("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
			Context->set_font_size(0.03);

			for(size_t i = 0; i != vertex_label.size(); ++i)
				draw_centered_text(Context, vertex_position[i], vertex_label[i], vertex_width[i], vertex_height[i]);

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

		// Render the graph edges ...
		try
		{
			return_if_fail(graph.topology);
			return_if_fail(graph.vertex_data.count("position"));
			return_if_fail(graph.vertex_data.count("width"));
			return_if_fail(graph.vertex_data.count("height"));
			return_if_fail(graph.edge_data.count("type"));

			const k3d::graph::topology_t& topology = *graph.topology;

			const k3d::graph::points_t& vertex_position = dynamic_cast<k3d::graph::points_t&>(*graph.vertex_data["position"].get());
			return_if_fail(boost::num_vertices(topology) == vertex_position.size());
			const k3d::graph::doubles_t& vertex_width = dynamic_cast<k3d::graph::doubles_t&>(*graph.vertex_data["width"].get());
			const k3d::graph::doubles_t& vertex_height = dynamic_cast<k3d::graph::doubles_t&>(*graph.vertex_data["height"].get());

			const k3d::graph::indices_t& edge_type = dynamic_cast<k3d::graph::indices_t&>(*graph.edge_data["type"].get());
			return_if_fail(boost::num_edges(topology) == edge_type.size());

			Context->save();
			Context->set_line_width(0.003);
			Context->set_source_rgb(0.0, 0.5, 0.7);

			size_t edge_index = 0;
			typedef boost::graph_traits<k3d::graph::topology_t>::edge_iterator iter_t;
			for(std::pair<iter_t, iter_t> edges = boost::edges(topology); edges.first != edges.second; ++edge_index, ++edges.first)
			{
				const size_t source = boost::source(*edges.first, topology);
				const size_t target = boost::target(*edges.first, topology);

				switch(edge_type[edge_index])
				{
					case BEHAVIOR_EDGE:
					{
						std::valarray<double> pattern(0.002, 1);
						Context->set_dash(pattern, 0.1);
						break;
					}
					case DATA_EDGE:
					default:
					{
						std::valarray<double> pattern;
						Context->set_dash(pattern, 0.0);
						break;
					}
				}

				draw_box_to_box_arrow(
					Context,
					vertex_position[source],
					vertex_width[source],
					vertex_height[source] * 1.5,
					vertex_position[target],
					vertex_width[target],
					vertex_height[target] * 1.5,
					0.04);
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

		Context->restore();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<panel> factory(
				k3d::uuid(0xf37f08af, 0xf542d517, 0xde317099, 0x79394ed8),
				"NGUIPipelinePanel",
				_("Displays the visualization pipeline"),
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
	boost::shared_ptr<k3d::graph> m_graph;
};

} // namespace pipeline

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::pipeline::panel::get_factory());
K3D_MODULE_END

