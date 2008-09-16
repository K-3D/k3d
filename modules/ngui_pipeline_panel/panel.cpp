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

#include "document_to_graph.h"
#include "edge_indices.h"
#include "extract_tree.h"
#include "tree_layout.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/color.h>
#include <k3dsdk/command_tree.h>
#include <k3dsdk/data.h>
#include <k3dsdk/graph.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/ngui/basic_input_model.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/file_chooser_dialog.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/ui_component.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/options.h>
#include <k3dsdk/pipeline.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/rectangle.h>
#include <k3dsdk/result.h>
#include <k3dsdk/vectors.h>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/drawingarea.h>

#include <cairomm/context.h>

#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>

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
	public Gtk::VBox
{
public:
	panel() :
		m_save_png("Save PNG"),
		m_save_pdf("Save PDF"),
		m_save_ps("Save PS"),
		m_save_svg("Save SVG"),
		m_zoom_factor(1.0),
		m_document_state(0),
		m_root_node(0),
		m_node_width(k3d::data::init_value(0.4)),
		m_node_height(k3d::data::init_value(0.07)),
		m_arrow_size(k3d::data::init_value(0.04)),
		m_background_color(k3d::data::init_value(k3d::color(1, 1, 1))),
		m_node_color(k3d::data::init_value(k3d::color(1, 1, 1))),
		m_node_border_color(k3d::data::init_value(k3d::color(0, 0, 0))),
		m_node_label_color(k3d::data::init_value(k3d::color(0, 0, 0))),
		m_connection_color(k3d::data::init_value(k3d::color(0, 0, 0)))
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

		m_input_model.connect_lbutton_double_click(sigc::mem_fun(*this, &panel::on_toggle_node_expansion));
		m_input_model.connect_lbutton_start_drag(sigc::mem_fun(*this, &panel::on_start_pan));
		m_input_model.connect_lbutton_drag(sigc::mem_fun(*this, &panel::on_pan));
		m_input_model.connect_mbutton_start_drag(sigc::mem_fun(*this, &panel::on_start_pan));
		m_input_model.connect_mbutton_drag(sigc::mem_fun(*this, &panel::on_pan));
		m_input_model.connect_rbutton_start_drag(sigc::mem_fun(*this, &panel::on_start_zoom));
		m_input_model.connect_rbutton_drag(sigc::mem_fun(*this, &panel::on_zoom));
		m_input_model.connect_scroll(sigc::mem_fun(*this, &panel::on_scroll_zoom));

		m_drawing_area.add_events(Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);
		m_drawing_area.connect_expose_event(sigc::mem_fun(*this, &panel::on_draw_pipeline));

		m_drawing_area.signal_button_press_event().connect(sigc::bind_return(sigc::mem_fun(m_input_model, &basic_input_model::button_press_event), true));
		m_drawing_area.signal_button_release_event().connect(sigc::bind_return(sigc::mem_fun(m_input_model, &basic_input_model::button_release_event), true));
		m_drawing_area.signal_motion_notify_event().connect(sigc::bind_return(sigc::mem_fun(m_input_model, &basic_input_model::motion_notify_event), true));
		m_drawing_area.signal_scroll_event().connect(sigc::bind_return(sigc::mem_fun(m_input_model, &basic_input_model::scroll_event), true));
	}

	void initialize(libk3dngui::document_state& DocumentState, k3d::icommand_node& Parent)
	{
		k3d::command_tree().add(*this, "pipeline", &Parent);
		m_document_state = &DocumentState;
	
		m_create_graph.reset(new document_to_graph(DocumentState.document()));

		m_edge_indices.reset(new edge_indices());
		
		m_extract_tree.reset(new extract_tree());

		m_tree_layout.reset(new tree_layout());
		k3d::property::set_internal_value(m_tree_layout->column_offset(), 0.6);
		k3d::property::set_internal_value(m_tree_layout->row_offset(), 0.1);
		
		m_graph_pipeline.reset(new k3d::pipeline());
		m_graph_pipeline->connect(m_create_graph->output(), m_edge_indices->input());
		m_graph_pipeline->connect(m_edge_indices->output(), m_extract_tree->input());
		m_graph_pipeline->connect(m_extract_tree->output(), m_tree_layout->input());

		m_tree_layout->output().property_changed_signal().connect(sigc::hide(sigc::mem_fun(*this, &panel::schedule_redraw)));

		m_document_state->document().nodes().add_nodes_signal().connect(sigc::hide(sigc::mem_fun(*this, &panel::reset_graph)));
		m_document_state->document().nodes().remove_nodes_signal().connect(sigc::hide(sigc::mem_fun(*this, &panel::reset_graph)));
		m_document_state->document().nodes().rename_node_signal().connect(sigc::hide(sigc::mem_fun(*this, &panel::schedule_redraw)));

		m_document_state->view_node_properties_signal().connect(sigc::bind_return(sigc::mem_fun(*this, &panel::selected_node_changed), false));

		show_all();
	}

	const k3d::string_t panel_type()
	{
		return get_factory().name();
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
		const k3d::double_t width = 5 * 72.0;
		const k3d::double_t height = width * aspect_ratio();

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
		const k3d::double_t width = 5 * 72.0;
		const k3d::double_t height = width * aspect_ratio();

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
		const k3d::double_t width = 5 * 72.0;
		const k3d::double_t height = width * aspect_ratio();

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

	void on_toggle_node_expansion(const GdkEventButton& Event)
	{
/*
		k3d::graph& graph = get_graph();
		const k3d::graph::adjacency_list& topology = *graph.topology;
		const k3d::uint_t vertex_count = boost::num_vertices(topology);

		k3d::graph::bools& vertex_expanded = k3d::get_array<k3d::graph::bools>(graph.vertex_data, "expanded", vertex_count);
		const k3d::graph::points& vertex_position = k3d::get_array<k3d::graph::points>(graph.vertex_data, "position", vertex_count);

		const k3d::point2 mouse = world_to_user(k3d::point2(Event.x, Event.y));

		for(k3d::uint_t vertex = 0; vertex != vertex_count; ++vertex)
		{
			const k3d::rectangle box(vertex_position[vertex], m_node_width, m_node_height);
			if(box.contains(mouse))
			{
				vertex_expanded[vertex] = !vertex_expanded[vertex];

//				tree_plus_layout(graph, m_root_vertex, m_column_offset, m_row_offset);
				schedule_redraw();
				return;
			}
		}
*/
	}

	void on_start_pan(const GdkEventMotion& Event)
	{
		m_last_mouse = k3d::point2(Event.x, Event.y);
	}

	void on_pan(const GdkEventMotion& Event)
	{
		const k3d::point2 current_mouse = k3d::point2(Event.x, Event.y);
		const k3d::vector2 delta = current_mouse - m_last_mouse;

		Gtk::Allocation allocation = m_drawing_area.get_allocation();
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

		const k3d::double_t sensitivity = 1.008;
		const k3d::double_t zoom_factor = delta[1] < 0 ? std::pow(sensitivity, std::abs(delta[1])) : std::pow(1 / sensitivity, std::abs(delta[1]));
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
		if(Glib::RefPtr<Gdk::Window> window = m_drawing_area.get_window())
		{
			Gdk::Rectangle r(0, 0, m_drawing_area.get_allocation().get_width(), m_drawing_area.get_allocation().get_height());
			window->invalidate_rect(r, false);
		}
	}

	void reset_graph()
	{
		m_create_graph->make_reset_graph_slot()(0);
	}

	void selected_node_changed(k3d::inode* Node)
	{
		if(m_root_node == Node)
			return;

		m_root_node = Node;

		k3d::graph& graph = *k3d::property::pipeline_value<k3d::graph*>(m_create_graph->output());
		const k3d::graph::adjacency_list_t& topology = *graph.topology;
		const k3d::uint_t vertex_count = boost::num_vertices(topology);

		const k3d::graph::nodes_t& vertex_node = *graph.vertex_data.lookup<k3d::graph::nodes_t>("node");
		for(k3d::uint_t vertex = 0; vertex != vertex_count; ++vertex)
		{
			if(vertex_node[vertex] == Node)
			{
				k3d::property::set_internal_value(*m_extract_tree, "root", vertex);
				return;
			}
		}
	}

	const k3d::double_t aspect_ratio()
	{
		Gtk::Allocation allocation = m_drawing_area.get_allocation();
		const k3d::double_t width = allocation.get_width();
		const k3d::double_t height = allocation.get_height();
		return height / width;
	}

	const k3d::point2 world_to_user(const k3d::point2& Point)
	{
		Gtk::Allocation allocation = m_drawing_area.get_allocation();
		const k3d::double_t width = allocation.get_width();
		const k3d::double_t height = allocation.get_height();

		k3d::point2 result = Point;

		if(width > height)
		{
			result -= k3d::vector2((width - height) / 2, 0);
			result /= height;
		}
		else
		{
			result -= k3d::vector2(0, (height - width) / 2);
			result /= width;
		}

		result -= k3d::vector2(0.5, 0.5);
		result /= m_zoom_factor;
		result -= k3d::vector2(m_origin[0], m_origin[1]);

		return result;
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

	void draw_box(const Cairo::RefPtr<Cairo::Context>& Context, const k3d::rectangle& Box)
	{
		Context->move_to(Box.left, Box.top);
		Context->line_to(Box.right, Box.top);
		Context->line_to(Box.right, Box.bottom);
		Context->line_to(Box.left, Box.bottom);
		Context->close_path();

		Context->stroke();
	}

	void draw_filled_box(const Cairo::RefPtr<Cairo::Context>& Context, const k3d::rectangle& Box)
	{
		Context->move_to(Box.left, Box.top);
		Context->line_to(Box.right, Box.top);
		Context->line_to(Box.right, Box.bottom);
		Context->line_to(Box.left, Box.bottom);
		Context->close_path();

		Context->fill();
	}

	void draw_curved_arrow(const Cairo::RefPtr<Cairo::Context>& Context, const k3d::point2& Source, const k3d::point2& Target, const k3d::double_t Size)
	{
		if(!k3d::length(Source - Target))
			return;

		const k3d::vector2 length_vector = k3d::normalize(k3d::vector2(Source[0] - Target[0], 0));
		const k3d::vector2 width_vector = k3d::perpendicular(length_vector);

		const k3d::point2 a = Source;
		const k3d::point2 b = Target + (Size * length_vector);
		const k3d::point2 c = Target + (Size * length_vector) + (0.3 * Size * width_vector);
		const k3d::point2 d = Target;
		const k3d::point2 e = Target + (Size * length_vector) + (-0.3 * Size * width_vector);

		const k3d::point2 ca = Source + k3d::vector2((Target[0] - Source[0]) * 0.4, 0);
		const k3d::point2 cb = Target + k3d::vector2((Source[0] - Target[0]) * 0.4, 0);

		Context->set_line_cap(Cairo::LINE_CAP_BUTT);
		Context->set_line_join(Cairo::LINE_JOIN_MITER);

		// Draw the line ...
		Context->move_to(a[0], a[1]);
		Context->curve_to(ca[0], ca[1], cb[0], cb[1], b[0], b[1]);
		Context->stroke();

		// Draw the arrow ...
		Context->move_to(c[0], c[1]);
		Context->line_to(d[0], d[1]);
		Context->line_to(e[0], e[1]);
		Context->line_to(c[0], c[1]);
		Context->fill();
	}

	void draw_centered_text(const Cairo::RefPtr<Cairo::Context>& Context, const k3d::double_t FontSize, const k3d::point2& Position, const std::string& Text, const k3d::double_t MaxWidth)
	{
		Context->save();

		Cairo::TextExtents extents;
		k3d::double_t font_size = FontSize;
		Context->set_font_size(font_size);
		for(Context->get_text_extents(Text, extents); extents.width > MaxWidth; Context->get_text_extents(Text, extents))
		{
			font_size *= 0.8;
			Context->set_font_size(font_size);
		}

		Context->translate(-extents.width / 2, extents.height / 2);
		Context->move_to(Position[0], Position[1]);
		Context->show_text(Text);

		Context->restore();
	}

	void draw_pipeline(const Cairo::RefPtr<Cairo::Context>& Context, const k3d::double_t Width, const k3d::double_t Height)
	{
		const k3d::double_t node_width = m_node_width.internal_value();
		const k3d::double_t node_height = m_node_height.internal_value();
		const k3d::double_t arrow_size = m_arrow_size.internal_value();
		const k3d::color background_color = m_background_color.internal_value();
		const k3d::color node_color = m_node_color.internal_value();
		const k3d::color node_border_color = m_node_border_color.internal_value();
		const k3d::color node_label_color = m_node_label_color.internal_value();
		const k3d::color connection_color = m_connection_color.internal_value();

		Context->save();

		try
		{
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
			Context->set_source_rgb(background_color.red, background_color.green, background_color.blue);
			Context->paint();
			Context->restore();

			// Get the graph to be rendered ...
			const k3d::graph& graph = *boost::any_cast<k3d::graph*>(k3d::property::pipeline_value(m_tree_layout->output()));

//k3d::log() << debug << "input graph:\n" << graph << std::endl;

			return_if_fail(graph.topology);
			return_if_fail(graph.vertex_data.count("node"));
			return_if_fail(graph.vertex_data.count("position"));
			return_if_fail(graph.edge_data.count("type"));

			const k3d::graph::adjacency_list_t& topology = *graph.topology;
			const k3d::uint_t vertex_count = boost::num_vertices(topology);
			const k3d::uint_t edge_count = boost::num_edges(topology);

			const k3d::graph::nodes_t& vertex_node = *graph.vertex_data.lookup<k3d::graph::nodes_t>("node");
			const k3d::graph::points_t& vertex_position = *graph.vertex_data.lookup<k3d::graph::points_t>("position");
			const k3d::graph::indices_t& edge_type = *graph.edge_data.lookup<k3d::graph::indices_t>("type");
			
			// Render the graph edges ...
			Context->save();
			Context->set_line_width(0.003);
			Context->set_source_rgb(connection_color.red, connection_color.green, connection_color.blue);

			k3d::uint_t edge_index = 0;
			for(std::pair<k3d::graph::edge_iterator_t, k3d::graph::edge_iterator_t> edges = boost::edges(topology); edges.first != edges.second; ++edge_index, ++edges.first)
			{
				const k3d::uint_t source = boost::source(*edges.first, topology);
				const k3d::uint_t target = boost::target(*edges.first, topology);

				switch(edge_type[edge_index])
				{
					case BEHAVIOR_EDGE:
					{
						std::valarray<k3d::double_t> pattern(0.008, 1);
						Context->set_dash(pattern, 0.1);
						break;
					}
					case DATA_EDGE:
					default:
					{
						std::valarray<k3d::double_t> pattern;
						Context->set_dash(pattern, 0.0);
						break;
					}
				}

				k3d::point2 begin = vertex_position[source];
				k3d::point2 end = vertex_position[target];

				begin[0] += end[0] > begin[0] ? node_width * 0.5 : node_width * -0.5;
				end[0] -= end[0] > begin[0] ? node_width * 0.5 : node_width * -0.5;

				draw_curved_arrow(
					Context,
					begin,
					end,
					arrow_size);
			}

			Context->restore();
			
			// Render the graph vertices ...
			Context->save();

			Context->select_font_face("Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
			Context->set_line_width(0.003);

			for(k3d::uint_t vertex = 0; vertex != vertex_count; ++vertex)
			{
				Context->set_source_rgb(node_color.red, node_color.green, node_color.blue);
				draw_filled_box(Context, k3d::rectangle(vertex_position[vertex], node_width, node_height));

				Context->set_source_rgb(node_border_color.red, node_border_color.green, node_border_color.blue);
				draw_box(Context, k3d::rectangle(vertex_position[vertex], node_width, node_height));

				Context->set_source_rgb(node_label_color.red, node_label_color.green, node_label_color.blue);
				draw_centered_text(Context, 0.03, vertex_position[vertex], vertex_node[vertex]->name(), node_width);
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
			"NGUI Panel",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-label", "Pipeline"));

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

	k3d::double_t m_zoom_factor;
	k3d::point2 m_origin;

	k3d::point2 m_last_mouse;

	document_state* m_document_state;
	boost::scoped_ptr<document_to_graph> m_create_graph;
	boost::scoped_ptr<edge_indices> m_edge_indices;
	boost::scoped_ptr<extract_tree> m_extract_tree;
	boost::scoped_ptr<tree_layout> m_tree_layout;
	boost::scoped_ptr<k3d::pipeline> m_graph_pipeline;

	k3d::inode* m_root_node;

	k3d_data(k3d::double_t, k3d::data::no_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::no_property, k3d::data::no_serialization) m_node_width;
	k3d_data(k3d::double_t, k3d::data::no_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::no_property, k3d::data::no_serialization) m_node_height;
	k3d_data(k3d::double_t, k3d::data::no_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::no_property, k3d::data::no_serialization) m_arrow_size;
	k3d_data(k3d::color, k3d::data::no_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::no_property, k3d::data::no_serialization) m_background_color;
	k3d_data(k3d::color, k3d::data::no_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::no_property, k3d::data::no_serialization) m_node_color;
	k3d_data(k3d::color, k3d::data::no_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::no_property, k3d::data::no_serialization) m_node_border_color;
	k3d_data(k3d::color, k3d::data::no_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::no_property, k3d::data::no_serialization) m_node_label_color;
	k3d_data(k3d::color, k3d::data::no_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::no_property, k3d::data::no_serialization) m_connection_color;
	
	basic_input_model m_input_model;
};

} // namespace pipeline

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::pipeline::panel::get_factory());
K3D_MODULE_END

