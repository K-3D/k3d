#ifndef SELECTION_CACHE_H_
#define SELECTION_CACHE_H_

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

/** \file Caches for selection data
 * 	\author Bart Janssens (bart.janssens@lid.kviv.be)
 */

namespace module
{

namespace opengl
{

namespace painters
{

/// Storage for selection data
typedef std::vector<k3d::mesh_selection::record> selection_records_t;

/// Keep track of component selections
class component_selection : public k3d::scheduler
{
public:
	/// Provide access to the stored selection records
	const selection_records_t& records() const
	{
		return m_selection_records;
	}
protected:
	/// Executes the selection update based on the mesh array selection
	void on_execute(const k3d::mesh& Mesh)
	{
		if (!m_selection_array)
			on_schedule(Mesh, 0);
		const k3d::mesh::selection_t& selection_array = *m_selection_array;
		for (size_t i = 0; i < selection_array.size();)
		{
			size_t start = i;
			k3d::mesh_selection::record record(start, i+1, selection_array[i]);
			while (i < selection_array.size() && record.weight == selection_array[i])
			{
				record.end = i+1;
				++i;
			}
			m_selection_records.push_back(record);
		}
	}

	selection_records_t m_selection_records;
	boost::shared_ptr<const k3d::mesh::selection_t> m_selection_array;
};

/// Implement component_selection::on_schedule for a point selection
class point_selection : public component_selection
{
protected:
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_records.clear();
		m_selection_array=Mesh.point_selection;
	}
};

/// Implement component_selection::on_schedule for an edge selection
class edge_selection : public component_selection
{
protected:
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_records.clear();
		m_selection_array=Mesh.polyhedra->edge_selection;
	}
};

/// Implement component_selection::on_schedule for a face selection
class face_selection : public component_selection
{
protected:
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_records.clear();
		m_selection_array=Mesh.polyhedra->face_selection;
	}
};

} // namespace painters

} // namespace opengl

} // namespace module

#endif /*SELECTION_CACHE_H_*/
