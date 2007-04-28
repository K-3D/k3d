#ifndef NGUI_NODE_LIST_H
#define NGUI_NODE_LIST_H

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

#include "panel.h"
#include "ui_component.h"

#include <k3dsdk/uuid.h>
#include <gtkmm/box.h>
#include <vector>

namespace k3d { class iunknown; }

namespace libk3dngui
{

class document_state;

namespace node_list
{

/// Encapsulates an "edge" (a directed connection between two nodes) to be visualized by the control
class edge
{
public:
	k3d::iunknown* from;
	k3d::iunknown* to;
};

/// Encapsulates a "node" (zero-to-many document nodes) to be visualized by the control
class node
{
public:
	std::string label;

	typedef std::vector<k3d::iunknown*> nodes_t;
	nodes_t nodes;
};

/// Defines a collection of nodes and dependencies
class graph
{
public:
	~graph();

	typedef std::vector<node*> nodes_t;
	nodes_t nodes;

	typedef std::vector<edge*> edges_t;
	edges_t edges;
};

/// Abstract interface for nodes that populate graphs - these control what the user ends-up seeing
class filter_policy
{
public:
	virtual ~filter_policy() {}

	virtual void populate_graph(graph& Graph) = 0;
};

/// Concrete implementation of filter_policy that does nothing
class null_filter_policy :
	public filter_policy
{
public:
	virtual ~null_filter_policy() {}

	void populate_graph(graph& Graph);
};

/// Concrete implementation of filter_policy that displays every node in the document
class all_nodes_filter_policy :
	public filter_policy
{
public:
	all_nodes_filter_policy(k3d::idocument& Document);
	virtual ~all_nodes_filter_policy() {}

	void populate_graph(graph& Graph);

private:
	k3d::idocument& m_document;
};

/// Concrete implementation of filter_policy that displays every node that matches given class ID
class class_id_filter_policy :
	public filter_policy
{
public:
	class_id_filter_policy(k3d::idocument& Document, const k3d::uuid& ClassID);
	~class_id_filter_policy() {}

	void populate_graph(graph& Graph);

private:
	k3d::idocument& m_document;
	const k3d::uuid m_class_id;
};

/// Abstract interface for nodes that adjust the layout of a graph
class layout_policy
{
public:
	virtual ~layout_policy() {}

	virtual void update_layout(graph& Graph) = 0;
};

/// Concrete implementation of layout_policy that does nothing
class null_layout_policy :
	public layout_policy
{
public:
	virtual ~null_layout_policy() {}

	void update_layout(graph& Graph);
};

/// Concrete implementation of layout_policy that sorts nodes alphabetically by label
class sort_by_label_layout_policy :
	public layout_policy
{
public:
	~sort_by_label_layout_policy() {}

	void update_layout(graph& Graph);
};

/// Concrete implementation of layout_policy that sorts nodes based on the type of nodes they front for
class sort_by_type_layout_policy :
	public layout_policy
{
public:
	~sort_by_type_layout_policy() {}

	void update_layout(graph& Graph);
};

/////////////////////////////////////////////////////////////////////////////
// control

class control :
	public Gtk::VBox,
	public panel::control,
	public ui_component
{
	typedef Gtk::VBox base;

public:
	control(document_state& DocumentState, k3d::icommand_node& Parent);
	~control();

	sigc::connection connect_focus_signal(const sigc::slot<void>& Slot);

	/// Sets a new policy for filtering the contents of the control - note: the control assumes responsibility for the lifetime of the policy node
	void set_filter_policy(filter_policy* const Policy);
	/// Sets a new policy for automatic layout of the contents of the control - note: the control assumes responsibility for the lifetime of the policy node
	void set_layout_policy(layout_policy* const Policy);

	const k3d::icommand_node::result execute_command(const std::string& Command, const std::string& Arguments);
	
private:
	class implementation;
	implementation* const m_implementation;
};

} // namespace node_list

} // namespace libk3dngui

#endif // !NGUI_NODE_LIST_H

