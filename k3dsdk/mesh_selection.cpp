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

	void operator()(legacy::cubic_curve_group&)
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
	result.linear_curves = component_select_all();
	result.cubic_curves = component_select_all();
	result.nurbs_curves = component_select_all();
	result.bilinear_patches = component_select_all();
	result.bicubic_patches = component_select_all();
	result.nurbs_patches = component_select_all();

	return result;
}

const mesh_selection mesh_selection::deselect_all()
{
	mesh_selection result;
	result.points = component_deselect_all();
	result.edges = component_deselect_all();
	result.faces = component_deselect_all();
	result.linear_curves = component_deselect_all();
	result.cubic_curves = component_deselect_all();
	result.nurbs_curves = component_deselect_all();
	result.bilinear_patches = component_deselect_all();
	result.bicubic_patches = component_deselect_all();
	result.nurbs_patches = component_deselect_all();

	return result;
}

const mesh_selection mesh_selection::select_null()
{
	return mesh_selection();
}

bool mesh_selection::empty() const
{
	return
		points.empty() &&
		edges.empty() &&
		faces.empty() &&
		linear_curves.empty() &&
		cubic_curves.empty() &&
		nurbs_curves.empty() &&
		bilinear_patches.empty() &&
		bicubic_patches.empty() &&
		nurbs_patches.empty();
}

bool mesh_selection::operator==(const mesh_selection& RHS) const
{
	return
		points == RHS.points &&
		edges == RHS.edges &&
		faces == RHS.faces &&
		linear_curves == RHS.linear_curves &&
		cubic_curves == RHS.cubic_curves &&
		nurbs_curves == RHS.nurbs_curves &&
		bilinear_patches == RHS.bilinear_patches &&
		bicubic_patches == RHS.bicubic_patches &&
		nurbs_patches == RHS.nurbs_patches;
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

/*
void mesh_selection::insert(const uint_t Begin, const record& Record, records_t& Selections)
{

	// first element, no checking needed
	if (Selections.empty())
	{
		Selections.insert(std::make_pair(Begin, Record));
		return;
	}

	// find first element equal than or greater than start of record to insert
	records_t::iterator after_start = Selections.lower_bound(Begin);
	// find the first element after the end of the record to insert
	records_t::iterator after_end = Selections.upper_bound(Record.end);
	// record right before the one we are about to insert, or Selections.end() if there is none
	records_t::iterator before;
	if (after_start == Selections.begin())
	{
		before = Selections.end();
	}
	else
	{
		before = after_start;
		--before;
	}

	// collapse records that come after the record to insert
	records_t::iterator a_record = after_start;
	record new_record = Record;
	record saved_record(0,0.0);
	int saved_start = -1;
	int count = 0;
	while (a_record != after_end && a_record->first <= new_record.end)
	{
		if (a_record->second.weight == Record.weight) // simple collapse with following record
		{
			new_record.end = a_record->second.end > new_record.end ? a_record->second.end : new_record.end; // expand range if needed
			records_t::iterator to_delete = a_record;
			++a_record;
			Selections.erase(to_delete);
			++count;
		}
		else
		{
			if (a_record->second.end <= new_record.end)
			{ // completeley overwrite old selection record
				records_t::iterator to_delete = a_record;
				++a_record;
				Selections.erase(to_delete);
			}
			else
			{ // save non-overlapped selection with other value
				saved_record = a_record->second;
				saved_start = new_record.end;
				records_t::iterator to_delete = a_record;
				++a_record;
				Selections.erase(to_delete);
			}
		}
	}

	if (saved_start > -1)
		Selections.insert(std::make_pair(saved_start, saved_record));

	// collapse with record before the one we are inserting
	if (before != Selections.end() && Begin <= before->second.end)
	{
		if (before->second.weight == Record.weight) // simple collapse
		{
			before->second.end = new_record.end;
			//show_selection_list(Selections); // FIXME extremely slow debug output
			return; // collapsed with record before the one to insert, so we're done
		}
		else // split up if selection weight differs
		{
			uint_t old_end = before->second.end;
			before->second.end = Begin;
			if (old_end > new_record.end)
			{
				Selections.insert(std::make_pair(new_record.end, record(old_end, before->second.weight)));
			}
			if (before->first == before->second.end)
				Selections.erase(before);
		}
	}

	Selections.insert(std::make_pair(Begin, new_record));

	//show_selection_list(Selections); // FIXME extremely slow debug output
}
*/

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
	Stream << "linear curves:    " << RHS.linear_curves << "\n";
	Stream << "cubic curves:     " << RHS.cubic_curves << "\n";
	Stream << "nurbs_curves:     " << RHS.nurbs_curves << "\n";
	Stream << "bilinear patches: " << RHS.bilinear_patches << "\n";
	Stream << "bicubic patches:  " << RHS.bicubic_patches << "\n";
	Stream << "nurbs_patches:    " << RHS.nurbs_patches << "\n";

	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// store_selection

void store_selection(const legacy::mesh& Mesh, mesh_selection& MeshSelection)
{
	MeshSelection.points.clear();
	MeshSelection.edges.clear();
	MeshSelection.faces.clear();
	MeshSelection.linear_curves.clear();
	MeshSelection.cubic_curves.clear();
	MeshSelection.nurbs_curves.clear();
	MeshSelection.bilinear_patches.clear();
	MeshSelection.bicubic_patches.clear();
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

	detail::store_selection store_linear_curve_selection(MeshSelection.linear_curves);
	for(legacy::mesh::linear_curve_groups_t::const_iterator group = Mesh.linear_curve_groups.begin(); group != Mesh.linear_curve_groups.end(); ++group)
		std::for_each((*group)->curves.begin(), (*group)->curves.end(), store_linear_curve_selection);

	detail::store_selection store_cubic_curve_selection(MeshSelection.cubic_curves);
	for(legacy::mesh::cubic_curve_groups_t::const_iterator group = Mesh.cubic_curve_groups.begin(); group != Mesh.cubic_curve_groups.end(); ++group)
		std::for_each((*group)->curves.begin(), (*group)->curves.end(), store_cubic_curve_selection);

	detail::store_selection store_nurbs_curve_selection(MeshSelection.nurbs_curves);
	for(legacy::mesh::nucurve_groups_t::const_iterator group = Mesh.nucurve_groups.begin(); group != Mesh.nucurve_groups.end(); ++group)
		std::for_each((*group)->curves.begin(), (*group)->curves.end(), store_nurbs_curve_selection);

	std::for_each(Mesh.bilinear_patches.begin(), Mesh.bilinear_patches.end(), detail::store_selection(MeshSelection.bilinear_patches));
	std::for_each(Mesh.bicubic_patches.begin(), Mesh.bicubic_patches.end(), detail::store_selection(MeshSelection.bicubic_patches));
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
	k3d::legacy::for_each_linear_curve(Mesh, detail::merge_selection(MeshSelection.linear_curves));
	k3d::legacy::for_each_cubic_curve(Mesh, detail::merge_selection(MeshSelection.cubic_curves));
	k3d::legacy::for_each_nucurve(Mesh, detail::merge_selection(MeshSelection.nurbs_curves));
	k3d::legacy::for_each_bilinear_patch(Mesh, detail::merge_selection(MeshSelection.bilinear_patches));
	k3d::legacy::for_each_bicubic_patch(Mesh, detail::merge_selection(MeshSelection.bicubic_patches));
	k3d::legacy::for_each_nupatch(Mesh, detail::merge_selection(MeshSelection.nurbs_patches));
}

} // namespace k3d

