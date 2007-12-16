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

namespace detail
{

inline void copy_selection(const k3d::mesh::selection_t& Selection, selection_records_t& Records)
{
	for (k3d::uint_t i = 0; i < Selection.size();)
	{
		k3d::uint_t start = i;
		k3d::mesh_selection::record record(start, i+1, Selection[i]);
		while (i < Selection.size() && record.weight == Selection[i])
		{
			record.end = i+1;
			++i;
		}
		Records.push_back(record);
	}
}

} // namespace detail

/// Keep track of component selections
class component_selection : public k3d::scheduler
{
public:
	component_selection(const k3d::idocument& Document) : m_document(Document) {}
	/// Provide access to the stored selection records
	const selection_records_t& records() const
	{
		return m_selection_records;
	}

protected:
	selection_records_t m_selection_records;
	const k3d::idocument& m_document;
};

/// Implement component_selection::on_schedule for a point selection
class point_selection : public component_selection
{
public:
	point_selection(const k3d::idocument& Document) : component_selection(Document) {}
protected:
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_records.clear();
	}
	void on_execute(const k3d::mesh& Mesh)
	{
		detail::copy_selection(*Mesh.point_selection, m_selection_records);
	}
};

/// Implement component_selection::on_schedule for an edge selection
class edge_selection : public component_selection
{
public:
	edge_selection(const k3d::idocument& Document) : component_selection(Document) {}
protected:
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_records.clear();
	}
	void on_execute(const k3d::mesh& Mesh)
	{
		detail::copy_selection(*Mesh.polyhedra->edge_selection, m_selection_records);
	}
};

/// Implement component_selection::on_schedule for a face selection
class face_selection : public component_selection
{
public:
	face_selection(const k3d::idocument& Document) : component_selection(Document) {}
protected:
	void on_schedule(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_records.clear();
	}
	void on_execute(const k3d::mesh& Mesh)
	{
		detail::copy_selection(*Mesh.polyhedra->face_selection, m_selection_records);
	}
};

} // namespace painters

} // namespace opengl

} // namespace module

#endif /*SELECTION_CACHE_H_*/
