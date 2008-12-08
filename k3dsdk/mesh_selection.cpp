// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

#include "legacy_mesh.h"
#include "log.h"
#include "mesh_selection.h"
#include "selection.h"

namespace k3d
{

namespace detail
{

/// Stores mesh component selection data in the given collection of selection records
class store_selection
{
public:
	store_selection(mesh_selection::records_t& Records) :
		records(Records),
		index(0)
	{
	}

	/** \todo Consolidate ranges of identical selection data */
	template<typename component_t>
	void operator()(component_t Component)
	{
		records.push_back(mesh_selection::record(index, index+1, Component->selection_weight));
		++index;
	}

private:
	mesh_selection::records_t& records;
	uint_t index;
};

/// Updates mesh components using stored selection data.  Note: if selection data doesn't exist for a given component, leaves it alone
class merge_selection
{
public:
	merge_selection(const mesh_selection::records_t& Records) :
		records(Records),
		current_record(Records.begin()),
		end_record(Records.end()),
		index(0)
	{
	}

	void operator()(legacy::polyhedron&)
	{
	}

	void operator()(legacy::linear_curve_group&)
	{
	}

	void operator()(legacy::nucurve_group&)
	{
	}

	template<typename component_t>
	void operator()(component_t& Component)
	{
		for(mesh_selection::records_t::const_iterator record = records.begin(); record != records.end(); ++record)
		{
			if(record->begin <= index && index < record->end)
				Component.selection_weight = record->weight;
		}

		++index;
	}

private:
	const mesh_selection::records_t& records;
	mesh_selection::records_t::const_iterator current_record;
	const mesh_selection::records_t::const_iterator end_record;
	uint_t index;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// mesh_selection

const mesh_selection mesh_selection::select_all()
{
	mesh_selection result;

	result.points = component_select_all();
	result.edges = component_select_all();
	result.faces = component_select_all();
	result.nurbs_curves = component_select_all();
	result.nurbs_patches = component_select_all();

	result.add_record(selection::CONSTANT, 0, uint_t(-1), 0, uint_t(-1), 1.0);
	result.add_record(selection::UNIFORM, 0, uint_t(-1), 0, uint_t(-1), 1.0);
	result.add_record(selection::VARYING, 0, uint_t(-1), 0, uint_t(-1), 1.0);
	result.add_record(selection::FACE_VARYING, 0, uint_t(-1), 0, uint_t(-1), 1.0);
	result.add_record(selection::SPLIT_EDGE, 0, uint_t(-1), 0, uint_t(-1), 1.0);
	result.add_record(selection::VERTEX, 0, uint_t(-1), 0, uint_t(-1), 1.0);

	return result;
}

const mesh_selection mesh_selection::deselect_all()
{
	mesh_selection result;

	result.points = component_deselect_all();
	result.edges = component_deselect_all();
	result.faces = component_deselect_all();
	result.nurbs_curves = component_deselect_all();
	result.nurbs_patches = component_deselect_all();

	result.add_record(selection::CONSTANT, 0, uint_t(-1), 0, uint_t(-1), 0.0);
	result.add_record(selection::UNIFORM, 0, uint_t(-1), 0, uint_t(-1), 0.0);
	result.add_record(selection::VARYING, 0, uint_t(-1), 0, uint_t(-1), 0.0);
	result.add_record(selection::FACE_VARYING, 0, uint_t(-1), 0, uint_t(-1), 0.0);
	result.add_record(selection::SPLIT_EDGE, 0, uint_t(-1), 0, uint_t(-1), 0.0);
	result.add_record(selection::VERTEX, 0, uint_t(-1), 0, uint_t(-1), 0.0);

	return result;
}

const mesh_selection mesh_selection::select_null()
{
	return mesh_selection();
}

void mesh_selection::add_record(const selection::type Type, const uint_t PrimitiveBegin, const uint_t PrimitiveEnd, const uint_t IndexBegin, const uint_t IndexEnd, const double_t Weight)
{
	type.push_back(Type);
	primitive_begin.push_back(PrimitiveBegin);
	primitive_end.push_back(PrimitiveEnd);
	index_begin.push_back(IndexBegin);
	index_end.push_back(IndexEnd);
	weight.push_back(Weight);
}

void mesh_selection::clear()
{
	points.clear();
	edges.clear();
	faces.clear();
	nurbs_curves.clear();
	nurbs_patches.clear();

	type.clear();
	primitive_begin.clear();
	primitive_end.clear();
	index_begin.clear();
	index_end.clear();
	weight.clear();
}

bool mesh_selection::empty() const
{
	return
		points.empty() &&
		edges.empty() &&
		faces.empty() &&
		nurbs_curves.empty() &&
		nurbs_patches.empty() &&
		type.empty();
}

bool mesh_selection::operator==(const mesh_selection& RHS) const
{
	return
		points == RHS.points &&
		edges == RHS.edges &&
		faces == RHS.faces &&
		nurbs_curves == RHS.nurbs_curves &&
		nurbs_patches == RHS.nurbs_patches &&
		type == RHS.type &&
		primitive_begin == RHS.primitive_begin &&
		primitive_end == RHS.primitive_end &&
		index_begin == RHS.index_begin &&
		index_end == RHS.index_end &&
		weight == RHS.weight;
}

bool mesh_selection::operator!=(const mesh_selection& RHS) const
{
	return !(operator==(RHS));
}

const mesh_selection::records_t mesh_selection::component_select_all()
{
	mesh_selection::records_t result;
	result.push_back(record(0, uint_t(-1), 1.0));

	return result;
}

const mesh_selection::records_t mesh_selection::component_deselect_all()
{
	mesh_selection::records_t result;
	result.push_back(record(0, uint_t(-1), 0.0));

	return result;
}

// very slow debug output of selection list. Uncomment calls below if needed
void show_selection_list(const mesh_selection::records_t& Selections)
{
	k3d::log() << debug << "------start selection list-------" << std::endl;
	for (mesh_selection::records_t::const_iterator selection = Selections.begin(); selection != Selections.end(); ++selection)
		k3d::log() << debug <<  selection->begin << "->" << selection->end << " with weight " << selection->weight << std::endl;
	k3d::log() << debug << "--------end selection list-------" << std::endl;
}

std::ostream& operator<<(std::ostream& Stream, const mesh_selection::records_t& RHS)
{
	for(mesh_selection::records_t::const_iterator record = RHS.begin(); record != RHS.end(); ++record)
	{
		if(record != RHS.begin())
			Stream << " ";

		Stream << "[" << record->begin << ", " << record->end << ") " << record->weight;
	}

	return Stream;
}

std::ostream& operator<<(std::ostream& Stream, const mesh_selection& RHS)
{
	Stream << "points:           " << RHS.points << "\n";
	Stream << "edges:            " << RHS.edges << "\n";
	Stream << "faces:            " << RHS.faces << "\n";
	Stream << "nurbs_curves:     " << RHS.nurbs_curves << "\n";
	Stream << "nurbs_patches:    " << RHS.nurbs_patches << "\n";

	for(uint_t i = 0; i != RHS.type.size(); ++i)
	{
		Stream << RHS.type[i];
		Stream << " " << RHS.primitive_begin[i];
		Stream << " " << RHS.primitive_end[i];
		Stream << " " << RHS.index_begin[i];
		Stream << " " << RHS.index_end[i];
		Stream << " " << RHS.weight[i];
		Stream << "\n";
	}

	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// store_selection

void store_selection(const legacy::mesh& Mesh, mesh_selection& MeshSelection)
{
	MeshSelection.points.clear();
	MeshSelection.edges.clear();
	MeshSelection.faces.clear();
	MeshSelection.nurbs_curves.clear();
	MeshSelection.nurbs_patches.clear();

	std::for_each(Mesh.points.begin(), Mesh.points.end(), detail::store_selection(MeshSelection.points));

	detail::store_selection store_edge_selection(MeshSelection.edges);
	for(legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
	{
		for(legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
		{
			legacy::split_edge* edge = (*face)->first_edge;
			do
			{
				store_edge_selection(edge);
				edge = edge->face_clockwise;
			}
			while(edge != (*face)->first_edge);
		}
	}

	detail::store_selection store_face_selection(MeshSelection.faces);
	for(legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		std::for_each((*polyhedron)->faces.begin(), (*polyhedron)->faces.end(), store_face_selection);

	detail::store_selection store_nurbs_curve_selection(MeshSelection.nurbs_curves);
	for(legacy::mesh::nucurve_groups_t::const_iterator group = Mesh.nucurve_groups.begin(); group != Mesh.nucurve_groups.end(); ++group)
		std::for_each((*group)->curves.begin(), (*group)->curves.end(), store_nurbs_curve_selection);

	std::for_each(Mesh.nupatches.begin(), Mesh.nupatches.end(), detail::store_selection(MeshSelection.nurbs_patches));
}

/////////////////////////////////////////////////////////////////////////////
// merge_selection

void merge_selection(const mesh_selection& MeshSelection, legacy::mesh& Mesh)
{
	// This is a policy decision that may belong elsewhere ... if the mesh_selection is
	// empty, don't modify the mesh at all.  This makes it possible for a plugin to have an
	// "empty" selection property, in which-case the plugin will alter its input using whatever
	// selection the input already carries.
	if(MeshSelection.empty())
		return;

	k3d::legacy::for_each_point(Mesh, detail::merge_selection(MeshSelection.points));

	detail::merge_selection replace_edge_selection(MeshSelection.edges);
	for(legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
	{
		for(legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
		{
			legacy::split_edge* edge = (*face)->first_edge;
			do
			{
				replace_edge_selection(*edge);
				edge = edge->face_clockwise;
			}
			while(edge != (*face)->first_edge);

			for(legacy::face::holes_t::iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
			{
				legacy::split_edge* edge = *hole;
				do
				{
					replace_edge_selection(*edge);
					edge = edge->face_clockwise;
				}
				while(edge != (*hole));
			}
		}
	}

	k3d::legacy::for_each_face(Mesh, detail::merge_selection(MeshSelection.faces));
	k3d::legacy::for_each_nucurve(Mesh, detail::merge_selection(MeshSelection.nurbs_curves));
	k3d::legacy::for_each_nupatch(Mesh, detail::merge_selection(MeshSelection.nurbs_patches));
}

} // namespace k3d

