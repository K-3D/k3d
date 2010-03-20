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

#include <k3dsdk/color.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/iomanip.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/table_copier.h>
#include <k3dsdk/type_registry.h>

#include <boost/scoped_ptr.hpp>

#include <iterator>
#include <map>

namespace k3d
{

////////////////////////////////////////////////////////////////////////////////////////////
// difference::test

/// Return the difference between two shared arrays (handles "fuzzy" floating-point comparisons).
namespace difference
{

template<typename T>
void test(const pipeline_data<typed_array<T> >& A, const pipeline_data<typed_array<T> >& B, accumulator& Result)
{
	if(A && B)
		A->difference(*B, Result);
	else if(!A && !B)
		Result.exact(true);
	else
		Result.exact(false);
}

/// Return the difference between two shared arrays (handles "fuzzy" floating-point comparisons).
void test(const pipeline_data<uint_t_array>& A, const pipeline_data<uint_t_array>& B, accumulator& Result)
{
	if(A && B)
		A->difference(*B, Result);
	else if(!A && !B)
		Result.exact(true);
	else
		Result.exact(false);
}

/// Return the difference between two shared objects (handles "fuzzy" floating-point comparisons).
template<typename T>
void test(const pipeline_data<T>& A, const pipeline_data<T>& B, accumulator& Result)
{
	if(A && B)
		k3d::difference::test(*A, *B, Result);
	else if(!A && !B)
		Result.exact(true);
	else
		Result.exact(false);
}

/// Return the difference between two sets of primitives.
void test(const mesh::primitives_t& A, const mesh::primitives_t& B, accumulator& Result)
{
	// If they have differing numbers of primitives, they definitely aren't equal
	Result.exact(A.size() == B.size());

	for(mesh::primitives_t::const_iterator a = A.begin(), b = B.begin(); a != A.end() && b != B.end(); ++a, ++b)
	{
		if(a->get() && b->get())
			(**a).difference((**b), Result);
		else if(!a->get() && !b->get())
			Result.exact(true);
		else
			Result.exact(false);
	}
}

} // namespace difference

////////////////////////////////////////////////////////////////////////////////////
// mesh

mesh::mesh()
{
}

void mesh::difference(const mesh& Other, difference::accumulator& Result) const
{
	k3d::difference::test(points, Other.points, Result);
	k3d::difference::test(point_selection, Other.point_selection, Result);
	k3d::difference::test(point_attributes, Other.point_attributes, Result);
	k3d::difference::test(primitives, Other.primitives, Result);
}

namespace detail
{

/// Helper function used by lookup_unused_points()
void mark_used_points(const mesh::indices_t& PrimitivePoints, mesh::bools_t& UnusedPoints)
{
	const uint_t begin = 0;
	const uint_t end = PrimitivePoints.size();
	for(uint_t i = begin; i != end; ++i)
		UnusedPoints[PrimitivePoints[i]] = false;
}

/// Helper object used by lookup_unused_points()
struct mark_used_primitive_points
{
	mark_used_primitive_points(mesh::bools_t& UnusedPoints) :
		unused_points(UnusedPoints)
	{
	}

	void operator()(const string_t&, const table&, const string_t&, const pipeline_data<array>& Array)
	{
		if(Array->get_metadata_value(metadata::key::domain()) != metadata::value::point_indices_domain())
			return;

		if(const mesh::indices_t* const array = dynamic_cast<const mesh::indices_t*>(Array.get()))
			mark_used_points(*array, unused_points);
	}

	mesh::bools_t& unused_points;
};

} // namespace detail

const bounding_box3 mesh::bounds(const mesh& Mesh)
{
	return Mesh.points ? bounds(*Mesh.points) : bounding_box3();
}

const bounding_box3 mesh::bounds(const points_t& Points)
{
	bounding_box3 results;

	const uint_t point_begin = 0;
	const uint_t point_end = point_begin + Points.size();
	for(uint_t point = point_begin; point != point_end; ++point)
		results.insert(Points[point]);

	return results;
}

void mesh::deep_copy(const mesh& From, mesh& To)
{
	To = From;
	assert_not_implemented(); // Need to ensure that all storage is unique
}

void mesh::lookup_unused_points(const mesh& Mesh, mesh::bools_t& UnusedPoints)
{
	UnusedPoints.assign(Mesh.points ? Mesh.points->size() : 0, true);
	visit_arrays(Mesh, detail::mark_used_primitive_points(UnusedPoints));
}

void mesh::create_index_removal_map(const mesh::bools_t& KeepIndices, mesh::indices_t& IndexMap)
{
	IndexMap.resize(KeepIndices.size());

	const uint_t begin = 0;
	const uint_t end = KeepIndices.size();
	for(uint_t current_index = begin, new_index = begin; current_index != end; ++current_index)
	{
		IndexMap[current_index] = new_index;
		if(!KeepIndices[current_index])
			++new_index;
	}
}

void mesh::create_index_list(const mesh::bools_t& SelectedIndices, mesh::indices_t& IndexSet)
{
	IndexSet.resize(0);

	const uint_t begin = 0;
	const uint_t end = SelectedIndices.size();
	for(uint_t index = begin; index != end; ++index)
	{
		if(SelectedIndices[index])
			IndexSet.push_back(index);
	}
}

namespace detail
{

/// Helper function used by delete_points()
void remap_points(mesh::indices_t& PrimitivePoints, const mesh::indices_t& PointMap)
{
	const uint_t begin = 0;
	const uint_t end = PrimitivePoints.size();
	for(uint_t i = begin; i != end; ++i)
		PrimitivePoints[i] = PointMap[PrimitivePoints[i]];
}

/// Helper object used by delete_points()
struct remap_primitive_points
{
	remap_primitive_points(const mesh::indices_t& PointMap) :
		point_map(PointMap)
	{
	}

	void operator()(const string_t&, const table&, const string_t&, pipeline_data<array>& Array)
	{
		if(Array->get_metadata_value(metadata::key::domain()) != metadata::value::point_indices_domain())
			return;

		if(mesh::indices_t* const array = dynamic_cast<mesh::indices_t*>(&Array.writable()))
			remap_points(*array, point_map);
	}

	const mesh::indices_t& point_map;
};

} // namespace detail

void mesh::remap_points(mesh& Mesh, const mesh::indices_t& PointMap)
{
	visit_arrays(Mesh, detail::remap_primitive_points(PointMap));
}

void mesh::delete_points(mesh& Mesh, const mesh::bools_t& Points)
{
	mesh::indices_t point_map;
	delete_points(Mesh, Points, point_map);
}

void mesh::delete_points(mesh& Mesh, const mesh::bools_t& Points, mesh::indices_t& PointMap)
{
	// Enforce preconditions ...
	return_if_fail(Mesh.points);
	return_if_fail(Mesh.point_selection);
	return_if_fail(Mesh.points->size() == Mesh.point_selection->size());
	return_if_fail(Mesh.points->size() == Mesh.point_attributes.row_count() || 0 == Mesh.point_attributes.column_count());

	// Count how many points will be left when we're done ...
	const uint_t points_remaining = std::count(Points.begin(), Points.end(), false);

	// Create a mapping from current point indices to indices after we've removed points ...
	create_index_removal_map(Points, PointMap);

	// Move leftover points, point selections, and attributes into their final positions ...
	mesh::points_t& points = Mesh.points.writable();
	mesh::selection_t& point_selection = Mesh.point_selection.writable();
	table_copier point_attributes(Mesh.point_attributes);

	const uint_t point_begin = 0;
	const uint_t point_end = point_begin + Points.size();
	for(uint_t point = point_begin; point != point_end; ++point)
	{
		if(Points[point])
			continue;
		
		points[PointMap[point]] = points[point];
		point_selection[PointMap[point]] = point_selection[point];
		point_attributes.copy(point, PointMap[point]);
	}

	// Update generic mesh primitives so they use the correct indices ...
	visit_arrays(Mesh, detail::remap_primitive_points(PointMap));

	// Free leftover memory ...
	points.resize(points_remaining);
	point_selection.resize(points_remaining);
	Mesh.point_attributes.set_row_count(points_remaining);
}

namespace detail
{

class offset_point_indices
{
public:
	offset_point_indices(const uint_t Offset) :
		offset(Offset)
	{
	}

	void operator()(const string_t&, const table&, const string_t& ArrayName, pipeline_data<array>& Array)
	{
		if(Array->get_metadata_value(metadata::key::domain()) != metadata::value::point_indices_domain())
			return;

		uint_t_array* const array = dynamic_cast<uint_t_array*>(&Array.writable());
		if(!array)
		{
			log() << error << "array [" << ArrayName << "] must be a k3d::uint_t_array." << std::endl;
			return;
		}

		std::transform(array->begin(), array->end(), array->begin(), std::bind2nd(std::plus<uint_t>(), offset));
	}

private:
	const uint_t offset;	
};

} // namespace detail

void mesh::append(const mesh& Source, mesh& Target, uint_t* const PointBegin, uint_t* const PointEnd, uint_t* const PrimitiveBegin, uint_t* const PrimitiveEnd)
{
	const uint_t point_begin = Target.points ? Target.points->size() : 0;
	if(PointBegin)
		*PointBegin = point_begin;

	if(PrimitiveBegin)
		*PrimitiveBegin = Target.primitives.size();

	// Append source points to the target ...
	if(Source.points)
	{
		mesh::points_t& target_points = Target.points ? Target.points.writable() : Target.points.create();
		target_points.insert(target_points.end(), Source.points->begin(), Source.points->end());
	}

	if(Source.point_selection)
	{
		mesh::selection_t& target_point_selection = Target.point_selection ? Target.point_selection.writable() : Target.point_selection.create();
		target_point_selection.insert(target_point_selection.end(), Source.point_selection->begin(), Source.point_selection->end());
	}

	// Append source primitives to the target ...
	for(mesh::primitives_t::const_iterator primitive = Source.primitives.begin(); primitive != Source.primitives.end(); ++primitive)
	{
		Target.primitives.push_back(*primitive);
		mesh::primitive& new_primitive = Target.primitives.back().writable();

		if(point_begin)
			mesh::visit_arrays(new_primitive, detail::offset_point_indices(point_begin));
	}

	if(PointEnd)
		*PointEnd = Target.points ? Target.points->size() : 0;

	if(PrimitiveEnd)
		*PrimitiveEnd = Target.primitives.size();
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::primitive

mesh::primitive::primitive()
{
}

mesh::primitive::primitive(const string_t& Type) :
	type(Type)
{
}

void mesh::primitive::difference(const primitive& Other, difference::accumulator& Result) const
{
	k3d::difference::test(type, Other.type, Result);
	k3d::difference::test(structure, Other.structure, Result);
	k3d::difference::test(attributes, Other.attributes, Result);
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::primitives_t

mesh::primitive& mesh::primitives_t::create(const string_t& Type)
{
	push_back(pipeline_data<mesh::primitive>());
	return back().create(new mesh::primitive(Type));
}

////////////////////////////////////////////////////////////////////////////////////
// operator<<

std::ostream& operator<<(std::ostream& Stream, const mesh& RHS)
{
	if(RHS.points)
	{
		Stream << standard_indent << "points (" << RHS.points->size() << "):\n";
		Stream << push_indent << start_block() << *RHS.points << finish_block << pop_indent << "\n";
	}

	if(RHS.point_selection)
	{
		Stream << standard_indent << "point_selection (" << RHS.point_selection->size() << "):\n";
		Stream << push_indent << start_block() << *RHS.point_selection << finish_block << pop_indent << "\n";
	}

	Stream << standard_indent << "point_attributes (" << RHS.point_attributes.row_count() << "):\n";
	Stream << push_indent << RHS.point_attributes << pop_indent;

	Stream << standard_indent << "primitives (" << RHS.primitives.size() << "):\n" << push_indent;
	for(mesh::primitives_t::const_iterator primitive = RHS.primitives.begin(); primitive != RHS.primitives.end(); ++primitive)
		Stream << **primitive;
	Stream << pop_indent;

	return Stream;
}

////////////////////////////////////////////////////////////////////////////////////
// operator<<

std::ostream& operator<<(std::ostream& Stream, const mesh::primitive& RHS)
{
	Stream << standard_indent << "primitive \"" << RHS.type << "\"\n" << push_indent;
	Stream << standard_indent << "structure (" << RHS.structure.size() << "):\n";
	Stream << push_indent << RHS.structure << pop_indent;
	Stream << standard_indent << "attributes (" << RHS.attributes.size() << "):\n";
	Stream << push_indent << RHS.attributes << pop_indent;
	Stream << pop_indent;

	return Stream;
}

} // namespace k3d

