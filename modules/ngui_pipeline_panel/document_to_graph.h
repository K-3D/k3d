#ifndef DOCUMENT_TO_GRAPH_H
#define DOCUMENT_TO_GRAPH_H

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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "graph_source.h"

namespace k3d { class idocument; }

namespace module
{

namespace ngui
{

namespace pipeline
{

enum edge_type
{
	DATA_EDGE,
	BEHAVIOR_EDGE
};	

class document_to_graph :
	public graph_source
{
public:
	document_to_graph(k3d::idocument& Document);

private:
	void on_initialize_graph(k3d::graph& Graph);
	void on_update_graph(k3d::graph& Graph);

	k3d::idocument& m_document;
};

} // namespace pipeline

} // namespace ngui

} // namespace module

#endif // !DOCUMENT_TO_GRAPH

