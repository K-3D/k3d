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

#include <string>

namespace k3d { class graph; }
namespace k3d { class inode; }
namespace libk3dngui { class document_state; }

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

void create_graph(libk3dngui::document_state& DocumentState, k3d::graph& Graph);

void random_layout(k3d::graph& Graph);
void circular_layout(k3d::graph& Graph);
void force_directed_layout(k3d::graph& Graph);
void tree_plus_layout(k3d::graph& Graph, k3d::inode* Root);

template<typename array_t, typename map_t>
array_t& get_array(map_t& Map, const std::string& Name, const size_t Count)
{
	array_t* result = 0;
	if(Map.count(Name))
		result = dynamic_cast<array_t*>(Map[Name].get());

	if(!result)
	{
		result = new array_t();
		Map[Name].reset(result);
	}

	if(result->size() != Count)
		result->resize(Count);

	return *result;
}

} // namespace pipeline

} // namespace ngui

} // namespace module

