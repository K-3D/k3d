#ifndef K3DSDK_QTUI_NODE_LIST_MODEL_H
#define K3DSDK_QTUI_NODE_LIST_MODEL_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
*/

#include <k3dsdk/signal_system.h>
#include <QAbstractListModel>
#include <vector>

namespace k3d
{

class inode;

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// node_list_model

/// Exposes a list of K-3D nodes as a flat list suitable for use with Qt views.
class node_list_model :
        public QAbstractListModel,
	public sigc::trackable
{
	typedef QAbstractListModel base;

	Q_OBJECT;
public:
	node_list_model(QObject* Parent = 0);
	virtual ~node_list_model();

	/// Adds new nodes to the model
	void add_nodes(const std::vector<inode*>& Nodes);
	/// Removes nodes from the model
	void remove_nodes(const std::vector<inode*>& Nodes);
  /// Maps from an index to a node (could return NULL)
  inode* node(const QModelIndex& Index);

	virtual int rowCount(const QModelIndex& Parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& Index, int role = Qt::DisplayRole) const;

private:
	std::vector<inode*> nodes;
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_NODE_LIST_MODEL_H

