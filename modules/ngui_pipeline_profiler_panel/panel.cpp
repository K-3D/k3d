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
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/box.h>

#include <k3dsdk/ngui/asynchronous_update.h>
#include <k3dsdk/ngui/document_state.h>
#include <k3dsdk/ngui/hotkey_cell_renderer_text.h>
#include <k3dsdk/ngui/icons.h>
#include <k3dsdk/ngui/panel.h>
#include <k3dsdk/ngui/ui_component.h>

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/module.h>

#include <boost/assign/list_of.hpp>

namespace module
{

namespace ngui
{

namespace pipeline_profiler
{

/////////////////////////////////////////////////////////////////////////////
// panel

class panel :
	public libk3dngui::panel::control,
	public libk3dngui::ui_component,
	public libk3dngui::asynchronous_update,
	public Gtk::VBox
{
	typedef Gtk::VBox base;

public:
	panel() :
		base(false, 0),
		ui_component("pipeline_profiler", 0)
	{
		m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		m_scrolled_window.add(m_view);

		m_model = Gtk::TreeStore::create(m_columns);

		m_view.set_model(m_model);
		m_view.set_headers_visible(true);
		m_view.set_reorderable(true);
		m_view.get_selection()->set_mode(Gtk::SELECTION_NONE);

		Gtk::TreeView::Column* const node_column = Gtk::manage(new Gtk::TreeView::Column(_("Node"))); 
		node_column->pack_start(m_columns.icon, false); //false = don't expand.
		node_column->pack_start(m_columns.name);
		m_view.append_column(*Gtk::manage(node_column));

		m_view.append_column(_("Task"), m_columns.task);
		m_view.append_column(_("Time"), m_columns.time);
		m_view.append_column_numeric(_("%"), m_columns.percent, _("%.2f%%"));

//		m_view.get_column(0)->add_attribute(m_view.get_column(0)->get_first_cell_renderer()->property_cell_background_gdk(), m_columns.color);
//		m_view.get_column(1)->add_attribute(m_view.get_column(1)->get_first_cell_renderer()->property_cell_background_gdk(), m_columns.color);
		m_view.get_column(2)->add_attribute(m_view.get_column(2)->get_first_cell_renderer()->property_cell_background_gdk(), m_columns.color);
		m_view.get_column(3)->add_attribute(m_view.get_column(3)->get_first_cell_renderer()->property_cell_background_gdk(), m_columns.color);
		
		schedule_update();

		m_view.signal_focus_in_event().connect(sigc::bind_return(sigc::hide(m_panel_grab_signal.make_slot()), false), false);

		pack_start(m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);
		show_all();
	}

	void initialize(libk3dngui::document_state& DocumentState, k3d::icommand_node& Parent)
	{
		ui_component::set_parent("pipeline_profiler", &Parent);

		DocumentState.document().pipeline_profiler().connect_node_execution_signal(sigc::mem_fun(*this, &panel::on_node_execution));
		DocumentState.document().nodes().rename_node_signal().connect(sigc::mem_fun(*this, &panel::on_node_renamed));
		
	}

	const k3d::string_t panel_type()
	{
		return get_factory().name();
	}

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot)
	{
		return m_panel_grab_signal.connect(Slot);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<panel> factory(
			k3d::uuid(0xcaadef85, 0xb848e17d, 0x9f4c8a8b, 0x15ed410f),
			"NGUIPipelineProfilerPanel",
			_("Provides a panel for profiling execution of the visualization pipeline"),
			"NGUI Panels",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("ngui:component-type", "panel")("ngui:panel-label", "Pipeline Profiler"));

		return factory;
	}

private:
	/// Called by the signal system when profile data arrives
	void on_node_execution(k3d::inode& Node, const std::string& Task, double Time)
	{
		new_records.push_back(new_record(Node, Task, Time));
		schedule_update();
	}
	
	/// Called by the signal system anytime a node is renamed
	void on_node_renamed(k3d::inode* const Node)
	{
		return_if_fail(Node);

		Gtk::TreeNodeChildren node_rows = m_model->children();
		for(Gtk::TreeIter node_row = node_rows.begin(); node_row != node_rows.end(); ++node_row)
		{
			if(node_row->get_value(m_columns.node) == Node)
			{
				(*node_row)[m_columns.name] = Node->name();
			}
		}
	}

	/// Looks-up the row for a given node
	bool get_node_row(k3d::inode* const Node, Gtk::TreeRow& Row)
	{
		Gtk::TreeNodeChildren rows = m_model->children();
		for(Gtk::TreeIter row = rows.begin(); row != rows.end(); ++row)
		{
			if(row->get_value(m_columns.node) == Node)
			{
				Row = *row;
				return true;
			}
		}

		return false;
	}

	/// Looks-up the row for a given task
	bool get_task_row(Gtk::TreeRow NodeRow, const std::string& Task, Gtk::TreeRow& Row)
	{
		Gtk::TreeNodeChildren rows = NodeRow.children();
		for(Gtk::TreeIter row = rows.begin(); row != rows.end(); ++row)
		{
			if(row->get_value(m_columns.task) == Task)
			{
				Row = *row;
				return true;
			}
		}

		return false;
	}

	/// Computes a color based on a percentage
	const Gdk::Color get_color(const double percentage)
	{	
		Gdk::Color result;
	
		if(percentage > 0.5)
		{
			result.set_hsv(0.0, percentage - 0.5, 1.0);
		}
		else
		{
			result.set_hsv(120.0, 0.5 - percentage, 1.0);
		}

		return result;
	}

	/// Iterate over all rows and calculate totals / percentages / colors
	void calculate_totals()
	{
		Gdk::Color white;
		white.set_rgb_p(1, 1, 1);

		double total_time = 0;

		Gtk::TreeNodeChildren node_rows = m_model->children();
		for(Gtk::TreeIter node_row = node_rows.begin(); node_row != node_rows.end(); ++node_row)
		{
			(*node_row)[m_columns.time] = 0.0;

			Gtk::TreeNodeChildren task_rows = node_row->children();
			for(Gtk::TreeIter task_row = task_rows.begin(); task_row != task_rows.end(); ++task_row)
			{
				const double task_time = (*task_row)[m_columns.time];

				(*node_row)[m_columns.time] = (*node_row)[m_columns.time] + task_time;
				total_time += task_time;
			}
		}

		for(Gtk::TreeIter node_row = node_rows.begin(); node_row != node_rows.end(); ++node_row)
		{
			if(total_time)
			{
				const double percent = (*node_row)[m_columns.time] / total_time;

				(*node_row)[m_columns.percent] = 100.0 * percent;
				(*node_row)[m_columns.color] = get_color(percent);
			}
			else
			{
				(*node_row)[m_columns.percent] = 0.0;
				(*node_row)[m_columns.color] = white;
			}

			Gtk::TreeNodeChildren task_rows = node_row->children();
			for(Gtk::TreeIter task_row = task_rows.begin(); task_row != task_rows.end(); ++task_row)
			{
				if(total_time)
				{
					const double percent = (*task_row)[m_columns.time] / total_time;

					(*task_row)[m_columns.percent] = 100.0 * (*task_row)[m_columns.time] / total_time;
					(*task_row)[m_columns.color] = get_color(percent);
				}
				else
				{
					(*task_row)[m_columns.percent] = 0.0;
					(*task_row)[m_columns.color] = white;
				}
			}
		}
	}

	/// Updates the contents of the control
	void on_update()
	{
		for(unsigned long i = 0; i != new_records.size(); ++i)
		{
			Gtk::TreeRow node_row;
			if(!get_node_row(new_records[i].node, node_row))
			{
				node_row = *m_model->append();

				node_row[m_columns.node] = new_records[i].node;
				node_row[m_columns.icon] = libk3dngui::quiet_load_icon(new_records[i].node->factory().name(), Gtk::ICON_SIZE_MENU);
				node_row[m_columns.name] = new_records[i].node->name();
			}

			Gtk::TreeRow task_row;
			if(!get_task_row(node_row, new_records[i].task, task_row))
			{
				task_row = *m_model->append(node_row.children());

				task_row[m_columns.node] = new_records[i].node;
				task_row[m_columns.task] = new_records[i].task;
			}

			task_row[m_columns.time] = new_records[i].time;
		}
		new_records.clear();

		calculate_totals();
	}

	class columns :
		public Gtk::TreeModelColumnRecord
	{
	public:
		columns()
		{
			add(node);
			add(icon);
			add(name);
			add(task);
			add(time);
			add(percent);
			add(color);
		}

		Gtk::TreeModelColumn<k3d::inode*> node;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> task;
		Gtk::TreeModelColumn<double> time;
		Gtk::TreeModelColumn<double> percent;
		Gtk::TreeModelColumn<Gdk::Color> color;
	};
	columns m_columns;
	
	Glib::RefPtr<Gtk::TreeStore> m_model;
	Gtk::TreeView m_view;
	Gtk::ScrolledWindow m_scrolled_window;

	/// Signal that will be emitted whenever this control should grab the panel focus
	sigc::signal<void> m_panel_grab_signal;

	class new_record
	{
	public:
		new_record()
		{
		}

		new_record(k3d::inode& Node, const std::string& Task, double Time) :
			node(&Node),
			task(Task),
			time(Time)
		{
		}

		k3d::inode* node;
		std::string task;
		double time;
	};

	std::vector<new_record> new_records;
};

} // namespace pipeline_profiler

} // namespace ngui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ngui::pipeline_profiler::panel::get_factory());
K3D_MODULE_END

