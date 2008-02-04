#ifndef MESH_TOPOLOGY_H_
#define MESH_TOPOLOGY_H_

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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

/** \file Declares structures to add extra "convenience" data to a mesh
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/mesh.h>

namespace module
{

namespace development
{

/// Base class for the stored mesh data. data_t must adhere to the std::vector interface.
template<class data_t>
class mesh_topology_data
{
public:
	/// Generate the required data for the give Mesh
	mesh_topology_data(const k3d::mesh& Mesh) : m_mesh(Mesh) {}
	
	/// Get a reference to the ith element
	const typename data_t::value_type& operator[](k3d::uint_t i) const
	{
		return m_data[i];
	}
	
	/// Get the number of elements
	const k3d::uint_t size() const
	{
		return m_data.size();
	}
	
protected:
	const k3d::mesh& m_mesh;
	data_t m_data;
};

/// Stores the per-edge companion edges
typedef k3d::mesh::indices_t companions_t;

/// Calculates and stores edge companion data
class companions : public mesh_topology_data<companions_t>
{
	typedef mesh_topology_data<companions_t> base;
public:
	companions(const k3d::mesh& Mesh);
};

/// Stores vertex valencies (number of outgoing edges)
typedef k3d::mesh::counts_t valencies_t;

/// Calculates and stores vertex valency data
class valencies : public mesh_topology_data<valencies_t>
{
	typedef mesh_topology_data<companions_t> base;
public:
	valencies(const k3d::mesh& Mesh);
};

} // namespace development

} // namespace module

#endif /*MESH_TOPOLOGY_H_*/
