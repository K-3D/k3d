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

namespace k3d { class graph; }
namespace libk3dngui { class document_state; }

namespace module
{

namespace ngui_pipeline
{

enum edge_type
{
	DATA_EDGE,
	BEHAVIOR_EDGE
};	

void create_graph(libk3dngui::document_state& DocumentState, k3d::graph& Graph);

void random_layout(k3d::graph& Graph);
void circular_layout(k3d::graph& Graph);
void icicle_layout(k3d::graph& Graph);

} // namespace ngui_pipeline

} // namespace module

