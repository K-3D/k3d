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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "mesh.h"
#include "metadata_keys.h"
#include "legacy_mesh.h"
#include "log.h"
#include "mesh_selection.h"
#include "selection.h"

namespace k3d
{

namespace detail
{

class merge_generic_selection
{
public:
	merge_generic_selection(const mesh_selection::component& Component) :
		component(Component),
		selection_component(string_cast(Component.type))
	{
	}

	void operator()(const string_t& Name, pipeline_data<array>& Array)
	{
		if(Array->get_metadata_value(metadata::key::selection_component()) != selection_component)
			return;

		typed_array<double_t>* const array = dynamic_cast<typed_array<double_t>*>(&Array.writable());
		if(!array)
		{
			log() << error << "unexpected type for array [" << Name << "] with k3d:selection-component = " << selection_component << std::endl;
			return;
		}

		const uint_t range_begin = 0;
		const uint_t range_end = component.index_begin.size();
		for(uint_t range = range_begin; range != range_end; ++range)
		{
			std::fill(
				array->begin() + std::min(array->size(), component.index_begin[range]),
				array->begin() + std::min(array->size(), component.index_end[range]),
				component.weight[range]);
		}
	}

private:
	const mesh_selection::component& component;
	const string_t selection_component;
};

/** \deprecated */
void store_selection(const pipeline_data<mesh::selection_t>& Selection, mesh_selection::records_t& Records)
{
	if(!Selection)
		return;

	const mesh::selection_t& selection_weight = *Selection;

	const uint_t selection_begin = 0;
	const uint_t selection_end = selection_begin + selection_weight.size();
	for(uint_t selection = selection_begin; selection != selection_end; ++selection)
		Records.push_back(mesh_selection::record(selection, selection+1, selection_weight[selection]));
}

/** \deprecated */
class legacy_store_selection
{
public:
	legacy_store_selection(mesh_selection::records_t& Records) :
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

/** \deprecated */
void merge_selection(const mesh_selection::records_t& Records, mesh::selection_t& Selection)
{
	const uint_t selection_count = Selection.size();

	for(mesh_selection::records_t::const_iterator record = Records.begin(); record != Records.end(); ++record)
	{
		if(record->begin >= selection_count)
			break;

		mesh::selection_t::iterator begin = Selection.begin() + record->begin;
		mesh::selection_t::iterator end = Selection.begin() + std::min(selection_count, record->end);
		std::fill(begin, end, record->weight);
	}
}

/** \deprecated */
template<typename gprims_type>
void merge_selection(const mesh_selection::records_t& Records, const gprims_type& GPrims, pipeline_data<mesh::selection_t>& Selection)
{
	return_if_fail(GPrims);

	const uint_t gprim_count = GPrims->size();

	if(!Selection || Selection->size() != gprim_count)
		Selection.create(new mesh::selection_t(gprim_count));

	mesh::selection_t& selection = Selection.writable();

	detail::merge_selection(Records, selection);
}

/** \deprecated */
class legacy_merge_selection
{
public:
	legacy_merge_selection(const mesh_selection::records_t& Records) :
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

	result.components.push_back(component(0, uint_t(-1), selection::CONSTANT, 0, uint_t(-1), 1.0));
	result.components.push_back(component(0, uint_t(-1), selection::UNIFORM, 0, uint_t(-1), 1.0));
	result.components.push_back(component(0, uint_t(-1), selection::VARYING, 0, uint_t(-1), 1.0));
	result.components.push_back(component(0, uint_t(-1), selection::FACE_VARYING, 0, uint_t(-1), 1.0));
	result.components.push_back(component(0, uint_t(-1), selection::SPLIT_EDGE, 0, uint_t(-1), 1.0));
	result.components.push_back(component(0, uint_t(-1), selection::VERTEX, 0, uint_t(-1), 1.0));

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

	result.components.push_back(component(0, uint_t(-1), selection::CONSTANT, 0, uint_t(-1), 0.0));
	result.components.push_back(component(0, uint_t(-1), selection::UNIFORM, 0, uint_t(-1), 0.0));
	result.components.push_back(component(0, uint_t(-1), selection::VARYING, 0, uint_t(-1), 0.0));
	result.components.push_back(component(0, uint_t(-1), selection::FACE_VARYING, 0, uint_t(-1), 0.0));
	result.components.push_back(component(0, uint_t(-1), selection::SPLIT_EDGE, 0, uint_t(-1), 0.0));
	result.components.push_back(component(0, uint_t(-1), selection::VERTEX, 0, uint_t(-1), 0.0));

	return result;
}

const mesh_selection mesh_selection::select_null()
{
	return mesh_selection();
}

void mesh_selection::store(const mesh& Mesh, mesh_selection& Selection)
{
	Selection.clear();

	detail::store_selection(Mesh.point_selection, Selection.points);

	if(Mesh.polyhedra)
	{
		detail::store_selection(Mesh.polyhedra->edge_selection, Selection.edges);
		detail::store_selection(Mesh.polyhedra->face_selection, Selection.faces);
	}

	if(Mesh.nurbs_curve_groups)
	{
		detail::store_selection(Mesh.nurbs_curve_groups->curve_selection, Selection.nurbs_curves);
	}

	if(Mesh.nurbs_patches)
	{
		detail::store_selection(Mesh.nurbs_patches->patch_selection, Selection.nurbs_patches);
	}

	// Handle generic mesh primitives ...
	assert_not_implemented();
}

void mesh_selection::store(const legacy::mesh& Mesh, mesh_selection& Selection)
{
	Selection.clear();

	std::for_each(Mesh.points.begin(), Mesh.points.end(), detail::legacy_store_selection(Selection.points));

	detail::legacy_store_selection store_edge_selection(Selection.edges);
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

	detail::legacy_store_selection store_face_selection(Selection.faces);
	for(legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		std::for_each((*polyhedron)->faces.begin(), (*polyhedron)->faces.end(), store_face_selection);

	detail::legacy_store_selection store_nurbs_curve_selection(Selection.nurbs_curves);
	for(legacy::mesh::nucurve_groups_t::const_iterator group = Mesh.nucurve_groups.begin(); group != Mesh.nucurve_groups.end(); ++group)
		std::for_each((*group)->curves.begin(), (*group)->curves.end(), store_nurbs_curve_selection);

	std::for_each(Mesh.nupatches.begin(), Mesh.nupatches.end(), detail::legacy_store_selection(Selection.nurbs_patches));
}

void mesh_selection::merge(const mesh_selection& Selection, mesh& Mesh)
{
	if(Mesh.points && Mesh.point_selection)
		detail::merge_selection(Selection.points, Mesh.points, Mesh.point_selection);

	if(Mesh.polyhedra && Mesh.polyhedra->edge_points)
	{
		k3d::mesh::polyhedra_t& polyhedra = Mesh.polyhedra.writable();
		detail::merge_selection(Selection.edges, polyhedra.edge_points, polyhedra.edge_selection);
	}

	if(Mesh.polyhedra && Mesh.polyhedra->face_first_loops)
	{
		k3d::mesh::polyhedra_t& polyhedra = Mesh.polyhedra.writable();
		detail::merge_selection(Selection.faces, polyhedra.face_first_loops, polyhedra.face_selection);
	}

	if(Mesh.nurbs_curve_groups)
	{
		k3d::mesh::nurbs_curve_groups_t& nurbs_curve_groups = Mesh.nurbs_curve_groups.writable();
		detail::merge_selection(Selection.nurbs_curves, nurbs_curve_groups.curve_first_points, nurbs_curve_groups.curve_selection);
	}

	if(Mesh.nurbs_patches)
	{
		k3d::mesh::nurbs_patches_t& nurbs_patches = Mesh.nurbs_patches.writable();
		detail::merge_selection(Selection.nurbs_patches, nurbs_patches.patch_materials, nurbs_patches.patch_selection);
	}

	// Handle generic mesh primitives ...
	for(components_t::const_iterator component = Selection.components.begin(); component != Selection.components.end(); ++component)
	{
		const uint_t primitive_begin = std::min(Mesh.primitives.size(), component->primitive_begin);
		const uint_t primitive_end = std::min(Mesh.primitives.size(), component->primitive_end);
		for(uint_t primitive = primitive_begin; primitive != primitive_end; ++primitive)
			mesh::visit_arrays(Mesh.primitives[primitive].writable(), detail::merge_generic_selection(*component));
	}
}

void mesh_selection::merge(const mesh_selection::records_t& Records, mesh::selection_t& Selection)
{
	detail::merge_selection(Records, Selection);
}

void mesh_selection::merge(const mesh_selection& Selection, legacy::mesh& Mesh)
{
	// This is a policy decision that may belong elsewhere ... if the mesh_selection is
	// empty, don't modify the mesh at all.  This makes it possible for a plugin to have an
	// "empty" selection property, in which-case the plugin will alter its input using whatever
	// selection the input already carries.
	if(Selection.empty())
		return;

	k3d::legacy::for_each_point(Mesh, detail::legacy_merge_selection(Selection.points));

	detail::legacy_merge_selection replace_edge_selection(Selection.edges);
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

	k3d::legacy::for_each_face(Mesh, detail::legacy_merge_selection(Selection.faces));
	k3d::legacy::for_each_nucurve(Mesh, detail::legacy_merge_selection(Selection.nurbs_curves));
	k3d::legacy::for_each_nupatch(Mesh, detail::legacy_merge_selection(Selection.nurbs_patches));
}

void mesh_selection::add_component(const component& Component)
{
	components.push_back(Component);
}

void mesh_selection::clear()
{
	points.clear();
	edges.clear();
	faces.clear();
	nurbs_curves.clear();
	nurbs_patches.clear();

	components.clear();
}

bool mesh_selection::empty() const
{
	return
		points.empty() &&
		edges.empty() &&
		faces.empty() &&
		nurbs_curves.empty() &&
		nurbs_patches.empty() &&
		components.empty();
}

bool mesh_selection::operator==(const mesh_selection& RHS) const
{
	return
		points == RHS.points &&
		edges == RHS.edges &&
		faces == RHS.faces &&
		nurbs_curves == RHS.nurbs_curves &&
		nurbs_patches == RHS.nurbs_patches &&
		components == RHS.components;
}

bool mesh_selection::operator!=(const mesh_selection& RHS) const
{
	return !(operator==(RHS));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// mesh_selection::component

mesh_selection::component::component() :
	primitive_begin(0),
	primitive_end(0),
	type(selection::NONE)
{
}

mesh_selection::component::component(const uint_t PrimitiveBegin, const uint_t PrimitiveEnd, const selection::type Type) :
	primitive_begin(PrimitiveBegin),
	primitive_end(std::max(PrimitiveBegin, PrimitiveEnd)),
	type(Type)
{
}

mesh_selection::component::component(const uint_t PrimitiveBegin, const uint_t PrimitiveEnd, const selection::type Type, const uint_t IndexBegin, const uint_t IndexEnd, const double_t Weight) :
	primitive_begin(PrimitiveBegin),
	primitive_end(std::max(PrimitiveBegin, PrimitiveEnd)),
	type(Type),
	index_begin(1, IndexBegin),
	index_end(1, std::max(IndexBegin, IndexEnd)),
	weight(1, Weight)
{
}

void mesh_selection::component::add_range(const uint_t IndexBegin, const uint_t IndexEnd, const double_t Weight)
{
	index_begin.push_back(IndexBegin);
	index_end.push_back(std::max(IndexBegin, IndexEnd));
	weight.push_back(Weight);
}

bool mesh_selection::component::operator==(const component& RHS) const
{
	return type == RHS.type
		&& primitive_begin == RHS.primitive_begin
		&& primitive_end == RHS.primitive_end
		&& index_begin == RHS.index_begin
		&& index_end == RHS.index_end
		&& weight == RHS.weight
		;
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

/*
// very slow debug output of selection list. Uncomment calls below if needed
void show_selection_list(const mesh_selection::records_t& Selections)
{
	k3d::log() << debug << "------start selection list-------" << std::endl;
	for (mesh_selection::records_t::const_iterator selection = Selections.begin(); selection != Selections.end(); ++selection)
		k3d::log() << debug <<  selection->begin << "->" << selection->end << " with weight " << selection->weight << std::endl;
	k3d::log() << debug << "--------end selection list-------" << std::endl;
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
	Stream << "nurbs_curves:     " << RHS.nurbs_curves << "\n";
	Stream << "nurbs_patches:    " << RHS.nurbs_patches << "\n";

	std::copy(RHS.components.begin(), RHS.components.end(), std::ostream_iterator<mesh_selection::component>(Stream, "\n"));

	return Stream;
}

std::ostream& operator<<(std::ostream& Stream, const mesh_selection::component& RHS)
{
	Stream << "primitives: [" << RHS.primitive_begin << ", " << RHS.primitive_end << ")";
	Stream << " " << RHS.type;
	
	for(uint_t i = 0; i != RHS.index_begin.size(); ++i)
	{
		Stream << " " << "[" << RHS.index_begin[i] << ", " << RHS.index_end[i] << ")";
		Stream << " " << RHS.weight[i];
	}

	return Stream;
}

} // namespace k3d

