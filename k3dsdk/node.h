#ifndef K3DSDK_NODE_H
#define K3DSDK_NODE_H

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
		\brief Declares k3d::node, a default implementation of the k3d::inode interface, for use as a base class for document plugins
		\author Tim Shead (tshead@k-3d.com)
*/

#include "data.h"
#include "idocument.h"
#include "inode.h"
#include "iselectable.h"
#include "property_collection.h"
#include "result.h"

namespace k3d
{

class iplugin_factory;

/// A default implementation of the k3d::inode interface, for use as a base class for document plugins
class node :
	public inode,
	public iselectable,
	public property_collection,
	public sigc::trackable
{
public:
	node(iplugin_factory& Factory, idocument& Document);
	virtual ~node();

	// inode implementation ...
	void set_name(const std::string Name);
	const std::string name();
	idocument& document();
	iplugin_factory& factory();

	deleted_signal_t& deleted_signal();
	name_changed_signal_t& name_changed_signal();

	double get_selection_weight()
	{
		return m_selection_weight.pipeline_value();
	}

	void set_selection_weight(const double Weight)
	{
		m_selection_weight.set_value(Weight);
	}

private:
	void on_deleted();

	/// Stores the factory that created this node
	iplugin_factory& m_factory;
	/// Stores the Document that owns this node
	idocument& m_document;
	/// Stores the name for this node
	k3d_data(std::string, data::immutable_name, data::change_signal, data::with_undo, data::local_storage, data::no_constraint, data::writable_property, data::no_serialization) m_name;
	/// Used to signal observers when this node is deleted
	deleted_signal_t m_deleted_signal;
	/// Used to signal observers when this node's name changes
	name_changed_signal_t m_name_changed_signal;

protected:
	/// Stores the node's selection weight
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_selection_weight;
};

} // namespace k3d

#endif // !K3DSDK_NODE_H


