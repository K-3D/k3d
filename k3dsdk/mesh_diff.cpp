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

#include "almost_equal.h"
#include "color.h"
#include "imaterial.h"
#include "mesh_diff.h"
#include "mesh.h"
#include "type_registry.h"

#include <boost/format.hpp>

#include <sstream>

namespace k3d
{

namespace detail
{

/////////////////////////////////////////////////////////////////////
// representable_difference

class representable_difference
{
public:
	template<typename T>
	const k3d::uint64_t operator()(const T& A, const T& B)
	{
		return B - A;
	}

	const k3d::uint64_t operator()(const double_t& A, const double_t& B)
	{
		return difference(A, B);
	}

	const k3d::uint64_t operator()(const color& A, const color& B)
	{
		return std::max(difference(A.red, B.red), std::max(difference(A.green, B.green), difference(A.blue, B.blue)));
	}

	const k3d::uint64_t operator()(const normal3& A, const normal3& B)
	{
		return std::max(difference(A[0], B[0]), std::max(difference(A[1], B[1]), difference(A[2], B[2])));
	}

	const k3d::uint64_t operator()(const point3& A, const point3& B)
	{
		return std::max(difference(A[0], B[0]), std::max(difference(A[1], B[1]), difference(A[2], B[2])));
	}

	const k3d::uint64_t operator()(const vector3& A, const vector3& B)
	{
		return std::max(difference(A[0], B[0]), std::max(difference(A[1], B[1]), difference(A[2], B[2])));
	}

	const k3d::uint64_t operator()(const point2& A, const point2& B)
	{
		return std::max(difference(A[0], B[0]), difference(A[1], B[1]));
	}

private:
	inline const k3d::uint64_t difference(const k3d::double_t& A, const k3d::double_t& B)
	{
		return std::abs(to_integer(A) - to_integer(B));
	}

	/// Convert a k3d::double_t to a lexicographically-ordered twos-complement integer
	inline const k3d::int64_t to_integer(const k3d::double_t Value)
	{
		const k3d::int64_t value = *(k3d::int64_t*)&Value;
		return value < 0 ? 0x8000000000000000LL - value : value;
	}
};

/////////////////////////////////////////////////////////////////////
// equal

/// Return true iff two arrays are equivalent (handles "fuzzy" floating-point comparisons).
template<typename T>
const bool equal(const typed_array<T>& A, const typed_array<T>& B, const boost::uint64_t Threshold)
{
	return A.almost_equal(B, Threshold);
}

/// Return true iff two arrays are equivalent (handles "fuzzy" floating-point comparisons).
const bool equal(const uint_t_array& A, const uint_t_array& B, const boost::uint64_t Threshold)
{
	return A.almost_equal(B, Threshold);
}

/// Return true iff two shared arrays are equivalent (handles cases where they point to the same memory, and handles "fuzzy" floating-point comparisons).
/// \note: This overload is used to compare arbitrary objects - not just arrays.
template<typename array_type>
const bool equal(const boost::shared_ptr<array_type>& A, const boost::shared_ptr<array_type>& B, const boost::uint64_t Threshold)
{
	if(A.get() == B.get())
		return true;

	if(A && B)
		return equal(*A, *B, Threshold);

	return false;
}

/// Return true iff two groups of named arrays are equivalent (handles cases where they point to the same memory, and handles "fuzzy" floating-point comparisons)
const bool equal(const attribute_arrays& A, const attribute_arrays& B, const boost::uint64_t Threshold)
{
	// If we have differing number of arrays, we aren't equal
	if(A.size() != B.size())
		return false;

	for(attribute_arrays::const_iterator a = A.begin(), b = B.begin(); a != A.end() && b != B.end(); ++a, ++b)
	{
		// Each pair of arrays must have equal names
		if(a->first != b->first)
			return false;

		// If both arrays point to the same memory, they're equal
		if(a->second.get() == b->second.get())
			continue;

		// Perform element-wise comparisons of the two arrays
		if(a->second && b->second)
		{
			// The array::almost_equal method correctly handles type-mismatches between arrays
			if(a->second->almost_equal(*b->second, Threshold))
				continue;
		}

		// Either the element-wise comparison failed or one array was NULL and the other wasn't
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
// print_diff

template<typename pointer_type>
void print_diff(std::ostream& Stream, const std::string& Label, const pointer_type& A, const pointer_type& B)
{
	if(!A && !B)
		return;

	boost::format format("%1% %|6t|%2% %|40t|%3%\n");
	const std::string divider(32, '-');

	const std::string difference_buffer = (A && B) ? std::string("") : std::string("*****");

	std::ostringstream a_label_buffer;
	if(A)
		a_label_buffer << Label;

	std::ostringstream b_label_buffer;
	if(B)
		b_label_buffer << Label;


	Stream << format % difference_buffer % a_label_buffer.str() % b_label_buffer.str();
	Stream << format % "" % divider % divider;
	Stream << "\n";
}

template<typename pointer_type>
void print_diff(std::ostream& Stream, const std::string& Label, const pointer_type& A, const pointer_type& B, const boost::uint64_t Threshold)
{
	if(!A && !B)
		return;

	const uint_t a_size = A ? A->size() : 0;
	const uint_t b_size = B ? B->size() : 0;

	std::ostringstream a_label_buffer;
	if(A)
		a_label_buffer << Label << " (" << a_size << ")";

	std::ostringstream b_label_buffer;
	if(B)
		b_label_buffer << Label << " (" << b_size << ")";

	boost::format format("%1% %|10t|%2% %|40t|%3%\n");
	const std::string divider(28, '-');

	Stream << format % "" % a_label_buffer.str() % b_label_buffer.str();
	Stream << format % "" % divider % divider;

	k3d::almost_equal<typename pointer_type::value_type::value_type> almost_equal(Threshold);

	for(uint_t i = 0; i < a_size || i < b_size; ++i)
	{
		if(!(A && i < a_size && B && i < b_size && almost_equal(A->at(i), B->at(i))))
		{
			std::ostringstream a_buffer;
			if(A && i < a_size)
				a_buffer << A->at(i);

			std::ostringstream b_buffer;
			if(B && i < b_size)
				b_buffer << B->at(i);

			Stream << format % i % a_buffer.str() % b_buffer.str();
		}

		if(A && i < a_size && B && i < b_size && !almost_equal(A->at(i), B->at(i)))
			Stream << format % i % representable_difference()(A->at(i), B->at(i)) % "";
	}

	Stream << "\n";
}

template<typename array_t>
const bool print_diff(std::ostream& Stream, const std::string& Label, const array& A, const array& B, const boost::uint64_t Threshold)
{
	const array_t* const a = dynamic_cast<const array_t*>(&A);
	const array_t* const b = dynamic_cast<const array_t*>(&B);

	if(!a && !b)
		return false;

	const uint_t a_size = a ? a->size() : 0;
	const uint_t b_size = b ? b->size() : 0;

	std::ostringstream a_label_buffer;
	if(a)
		a_label_buffer << Label << " (" << a_size << ")";

	std::ostringstream b_label_buffer;
	if(b)
		b_label_buffer << Label << " (" << b_size << ")";

	boost::format format("%1% %|10t|%2% %|40t|%3%\n");
	const std::string divider(28, '-');

	Stream << format % "" % a_label_buffer.str() % b_label_buffer.str();
	Stream << format % "" % divider % divider;

	k3d::almost_equal<typename array_t::value_type> almost_equal(Threshold);

	for(uint_t i = 0; i < a_size || i < b_size; ++i)
	{
		if(!(a && i < a_size && b && i < b_size && almost_equal(a->at(i), b->at(i))))
		{
			std::ostringstream a_buffer;
			if(a && i < a_size)
				a_buffer << a->at(i);

			std::ostringstream b_buffer;
			if(b && i < b_size)
				b_buffer << b->at(i);

			Stream << format % i % a_buffer.str() % b_buffer.str();
		}

		if(a && i < a_size && b && i < b_size && !almost_equal(a->at(i), b->at(i)))
			Stream << format % i % representable_difference()(a->at(i), b->at(i)) % "";
	}

	Stream << "\n";

	return true;
}

void print_diff(std::ostream& Stream, const std::string& Label, const array& A, const array& B, const boost::uint64_t Threshold)
{
	if(print_diff<typed_array<color> >(Stream, Label, A, B, Threshold))
		return;
	if(print_diff<typed_array<normal3> >(Stream, Label, A, B, Threshold))
		return;
	if(print_diff<typed_array<point3> >(Stream, Label, A, B, Threshold))
		return;
	if(print_diff<typed_array<vector3> >(Stream, Label, A, B, Threshold))
		return;
	
	k3d::log() << error << k3d_file_reference << ": unknown array type: " << demangle(typeid(A)) << std::endl;
}

void print_diff(std::ostream& Stream, const std::string& Label, const boost::shared_ptr<array>& A, const boost::shared_ptr<array>& B, const boost::uint64_t Threshold)
{
	print_diff(Stream, Label, *A, *B, Threshold);
}

void print_diff(std::ostream& Stream, const std::string& Label, const k3d::mesh::attribute_arrays_t& A, const k3d::mesh::attribute_arrays_t& B, const boost::uint64_t Threshold)
{
	if(A.empty() && B.empty())
		return;

	attribute_arrays::const_iterator a = A.begin();
	attribute_arrays::const_iterator b = B.begin();

	for(; a != A.end() && b != B.end(); ++a, ++b)
	{
		if(a->first != b->first)
		{
			Stream << "array names differ: [" << a->first << "] versus [" << b->first << "]\n";
			continue;
		}

		print_diff(Stream, Label + " \"" + a->first + "\"", a->second, b->second, Threshold);
	}

	if(a != A.end() || b != B.end())
	{
		Stream << "dangling arrays\n";
		return;
	}
}

} // namespace detail

const bool equal(const mesh::point_groups_t& A, const mesh::point_groups_t& B, const boost::uint64_t Threshold)
{
	return
		detail::equal(A.first_points, B.first_points, Threshold) &&
		detail::equal(A.point_counts, B.point_counts, Threshold) &&
		detail::equal(A.materials, B.materials, Threshold) &&
		detail::equal(A.constant_data, B.constant_data, Threshold) &&
		detail::equal(A.points, B.points, Threshold) &&
		detail::equal(A.varying_data, B.varying_data, Threshold);
}

const bool equal(const mesh::linear_curve_groups_t& A, const mesh::linear_curve_groups_t& B, const boost::uint64_t Threshold)
{
	return
		detail::equal(A.first_curves, B.first_curves, Threshold) &&
		detail::equal(A.curve_counts, B.curve_counts, Threshold) &&
		detail::equal(A.periodic_curves, B.periodic_curves, Threshold) &&
		detail::equal(A.materials, B.materials, Threshold) &&
		detail::equal(A.constant_data, B.constant_data, Threshold) &&
		detail::equal(A.curve_first_points, B.curve_first_points, Threshold) &&
		detail::equal(A.curve_point_counts, B.curve_point_counts, Threshold) &&
		detail::equal(A.curve_selection, B.curve_selection, Threshold) &&
		detail::equal(A.uniform_data, B.uniform_data, Threshold) &&
		detail::equal(A.curve_points, B.curve_points, Threshold);
}

const bool equal(const mesh::cubic_curve_groups_t& A, const mesh::cubic_curve_groups_t& B, const boost::uint64_t Threshold)
{
	return
		detail::equal(A.first_curves, B.first_curves, Threshold) &&
		detail::equal(A.curve_counts, B.curve_counts, Threshold) &&
		detail::equal(A.periodic_curves, B.periodic_curves, Threshold) &&
		detail::equal(A.materials, B.materials, Threshold) &&
		detail::equal(A.constant_data, B.constant_data, Threshold) &&
		detail::equal(A.curve_first_points, B.curve_first_points, Threshold) &&
		detail::equal(A.curve_point_counts, B.curve_point_counts, Threshold) &&
		detail::equal(A.curve_selection, B.curve_selection, Threshold) &&
		detail::equal(A.uniform_data, B.uniform_data, Threshold) &&
		detail::equal(A.curve_points, B.curve_points, Threshold);
}

const bool equal(const mesh::nurbs_curve_groups_t& A, const mesh::nurbs_curve_groups_t& B, const boost::uint64_t Threshold)
{
	return
		detail::equal(A.first_curves, B.first_curves, Threshold) &&
		detail::equal(A.curve_counts, B.curve_counts, Threshold) &&
		detail::equal(A.materials, B.materials, Threshold) &&
		detail::equal(A.constant_data, B.constant_data, Threshold) &&
		detail::equal(A.curve_first_points, B.curve_first_points, Threshold) &&
		detail::equal(A.curve_point_counts, B.curve_point_counts, Threshold) &&
		detail::equal(A.curve_orders, B.curve_orders, Threshold) &&
		detail::equal(A.curve_first_knots, B.curve_first_knots, Threshold) &&
		detail::equal(A.curve_selection, B.curve_selection, Threshold) &&
		detail::equal(A.uniform_data, B.uniform_data, Threshold) &&
		detail::equal(A.curve_points, B.curve_points, Threshold) &&
		detail::equal(A.curve_point_weights, B.curve_point_weights, Threshold) &&
		detail::equal(A.curve_knots, B.curve_knots, Threshold);
}

const bool equal(const mesh::bilinear_patches_t& A, const mesh::bilinear_patches_t& B, const boost::uint64_t Threshold)
{
	return
		detail::equal(A.patch_selection, B.patch_selection, Threshold) &&
		detail::equal(A.patch_materials, B.patch_materials, Threshold) &&
		detail::equal(A.constant_data, B.constant_data, Threshold) &&
		detail::equal(A.uniform_data, B.uniform_data, Threshold) &&
		detail::equal(A.patch_points, B.patch_points, Threshold) &&
		detail::equal(A.varying_data, B.varying_data, Threshold);
}

const bool equal(const mesh::bicubic_patches_t& A, const mesh::bicubic_patches_t& B, const boost::uint64_t Threshold)
{
	return
		detail::equal(A.patch_selection, B.patch_selection, Threshold) &&
		detail::equal(A.patch_materials, B.patch_materials, Threshold) &&
		detail::equal(A.constant_data, B.constant_data, Threshold) &&
		detail::equal(A.uniform_data, B.uniform_data, Threshold) &&
		detail::equal(A.patch_points, B.patch_points, Threshold) &&
		detail::equal(A.varying_data, B.varying_data, Threshold);
}

const bool equal(const mesh::nurbs_patches_t& A, const mesh::nurbs_patches_t& B, const boost::uint64_t Threshold)
{
	bool patches_equal =
		detail::equal(A.patch_first_points, B.patch_first_points, Threshold) &&
		detail::equal(A.patch_u_point_counts, B.patch_u_point_counts, Threshold) &&
		detail::equal(A.patch_v_point_counts, B.patch_v_point_counts, Threshold) &&
		detail::equal(A.patch_u_orders, B.patch_u_orders, Threshold) &&
		detail::equal(A.patch_v_orders, B.patch_v_orders, Threshold) &&
		detail::equal(A.patch_u_first_knots, B.patch_u_first_knots, Threshold) &&
		detail::equal(A.patch_v_first_knots, B.patch_v_first_knots, Threshold) &&
		detail::equal(A.patch_selection, B.patch_selection, Threshold) &&
		detail::equal(A.patch_materials, B.patch_materials, Threshold) &&
		detail::equal(A.constant_data, B.constant_data, Threshold) &&
		detail::equal(A.uniform_data, B.uniform_data, Threshold) &&
		detail::equal(A.patch_points, B.patch_points, Threshold) &&
		detail::equal(A.patch_point_weights, B.patch_point_weights, Threshold) &&
		detail::equal(A.patch_u_knots, B.patch_u_knots, Threshold) &&
		detail::equal(A.patch_v_knots, B.patch_v_knots, Threshold) &&
		detail::equal(A.varying_data, B.varying_data, Threshold);
	bool trim_curves_equal = true;
		if (A.patch_trim_curve_loop_counts && B.patch_trim_curve_loop_counts)
		{
			trim_curves_equal =
				detail::equal(A.patch_trim_curve_loop_counts, B.patch_trim_curve_loop_counts, Threshold) &&
				detail::equal(A.patch_first_trim_curve_loops, B.patch_first_trim_curve_loops, Threshold) &&
				detail::equal(A.trim_points, B.trim_points, Threshold) &&
				detail::equal(A.trim_point_selection, B.trim_point_selection, Threshold) &&
				detail::equal(A.first_trim_curves, B.first_trim_curves, Threshold) &&
				detail::equal(A.trim_curve_counts, B.trim_curve_counts, Threshold) &&
				detail::equal(A.trim_curve_loop_selection, B.trim_curve_loop_selection, Threshold) &&
				detail::equal(A.trim_curve_first_points, B.trim_curve_first_points, Threshold) &&
				detail::equal(A.trim_curve_point_counts, B.trim_curve_point_counts, Threshold) &&
				detail::equal(A.trim_curve_orders, B.trim_curve_orders, Threshold) &&
				detail::equal(A.trim_curve_first_knots, B.trim_curve_first_knots, Threshold) &&
				detail::equal(A.trim_curve_selection, B.trim_curve_selection, Threshold) &&
				detail::equal(A.trim_curve_points, B.trim_curve_points, Threshold) &&
				detail::equal(A.trim_curve_point_weights, B.trim_curve_point_weights, Threshold) &&
				detail::equal(A.trim_curve_knots, B.trim_curve_knots, Threshold);
		}
	return patches_equal && trim_curves_equal;
}

const bool equal(const mesh::polyhedra_t& A, const mesh::polyhedra_t& B, const boost::uint64_t Threshold)
{
	return
		detail::equal(A.first_faces, B.first_faces, Threshold) &&
		detail::equal(A.face_counts, B.face_counts, Threshold) &&
		detail::equal(A.types, B.types, Threshold) &&
		detail::equal(A.constant_data, B.constant_data, Threshold) &&
		detail::equal(A.face_first_loops, B.face_first_loops, Threshold) &&
		detail::equal(A.face_loop_counts, B.face_loop_counts, Threshold) &&
		detail::equal(A.face_selection, B.face_selection, Threshold) &&
		detail::equal(A.face_materials, B.face_materials, Threshold) &&
		detail::equal(A.uniform_data, B.uniform_data, Threshold) &&
		detail::equal(A.loop_first_edges, B.loop_first_edges, Threshold) &&
		detail::equal(A.edge_points, B.edge_points, Threshold) &&
		detail::equal(A.clockwise_edges, B.clockwise_edges, Threshold) &&
		detail::equal(A.edge_selection, B.edge_selection, Threshold) &&
		detail::equal(A.face_varying_data, B.face_varying_data, Threshold);
}

const bool equal(const mesh::blobbies_t& A, const mesh::blobbies_t& B, const boost::uint64_t Threshold)
{
	return
		detail::equal(A.first_primitives, B.first_primitives, Threshold) &&
		detail::equal(A.primitive_counts, B.primitive_counts, Threshold) &&
		detail::equal(A.first_operators, B.first_operators, Threshold) &&
		detail::equal(A.operator_counts, B.operator_counts, Threshold) &&
		detail::equal(A.materials, B.materials, Threshold) &&
		detail::equal(A.constant_data, B.constant_data, Threshold) &&
		detail::equal(A.uniform_data, B.uniform_data, Threshold) &&
		detail::equal(A.primitives, B.primitives, Threshold) &&
		detail::equal(A.primitive_first_floats, B.primitive_first_floats, Threshold) &&
		detail::equal(A.primitive_float_counts, B.primitive_float_counts, Threshold) &&
		detail::equal(A.varying_data, B.varying_data, Threshold) &&
		detail::equal(A.vertex_data, B.vertex_data, Threshold) &&
		detail::equal(A.operators, B.operators, Threshold) &&
		detail::equal(A.operator_first_operands, B.operator_first_operands, Threshold) &&
		detail::equal(A.operator_operand_counts, B.operator_operand_counts, Threshold) &&
		detail::equal(A.floats, B.floats, Threshold) &&
		detail::equal(A.operands, B.operands, Threshold);
}

const bool equal(const mesh& A, const mesh& B, const boost::uint64_t Threshold)
{
	return
		detail::equal(A.points, B.points, Threshold) &&
		detail::equal(A.point_selection, B.point_selection, Threshold) &&
		detail::equal(A.vertex_data, B.vertex_data, Threshold) &&
		detail::equal(A.point_groups, B.point_groups, Threshold) &&
		detail::equal(A.linear_curve_groups, B.linear_curve_groups, Threshold) &&
		detail::equal(A.cubic_curve_groups, B.cubic_curve_groups, Threshold) &&
		detail::equal(A.nurbs_curve_groups, B.nurbs_curve_groups, Threshold) &&
		detail::equal(A.bilinear_patches, B.bilinear_patches, Threshold) &&
		detail::equal(A.bicubic_patches, B.bicubic_patches, Threshold) &&
		detail::equal(A.nurbs_patches, B.nurbs_patches, Threshold) &&
		detail::equal(A.polyhedra, B.polyhedra, Threshold) &&
		detail::equal(A.blobbies, B.blobbies, Threshold);
}

void print_diff(std::ostream& Stream, const mesh& A, const mesh& B, const boost::uint64_t Threshold)
{
	detail::print_diff(Stream, "point groups", A.point_groups, B.point_groups);
	if(A.point_groups && B.point_groups)
	{
		detail::print_diff(Stream, "first points", A.point_groups->first_points, B.point_groups->first_points, Threshold);
		detail::print_diff(Stream, "point counts", A.point_groups->point_counts, B.point_groups->point_counts, Threshold);
		detail::print_diff(Stream, "materials", A.point_groups->materials, B.point_groups->materials, Threshold);
		detail::print_diff(Stream, "constant data", A.point_groups->constant_data, B.point_groups->constant_data, Threshold);
		detail::print_diff(Stream, "points", A.point_groups->points, B.point_groups->points, Threshold);
		detail::print_diff(Stream, "varying data", A.point_groups->varying_data, B.point_groups->varying_data, Threshold);
	}

	detail::print_diff(Stream, "linear curve groups", A.linear_curve_groups, B.linear_curve_groups);
	if(A.linear_curve_groups && B.linear_curve_groups)
	{
		detail::print_diff(Stream, "first curves", A.linear_curve_groups->first_curves, B.linear_curve_groups->first_curves, Threshold);
		detail::print_diff(Stream, "curve counts", A.linear_curve_groups->curve_counts, B.linear_curve_groups->curve_counts, Threshold);
		detail::print_diff(Stream, "periodic curves", A.linear_curve_groups->periodic_curves, B.linear_curve_groups->periodic_curves, Threshold);
		detail::print_diff(Stream, "materials", A.linear_curve_groups->materials, B.linear_curve_groups->materials, Threshold);
		detail::print_diff(Stream, "constant data", A.linear_curve_groups->constant_data, B.linear_curve_groups->constant_data, Threshold);
		detail::print_diff(Stream, "curve first points", A.linear_curve_groups->curve_first_points, B.linear_curve_groups->curve_first_points, Threshold);
		detail::print_diff(Stream, "curve point counts", A.linear_curve_groups->curve_point_counts, B.linear_curve_groups->curve_point_counts, Threshold);
		detail::print_diff(Stream, "curve selection", A.linear_curve_groups->curve_selection, B.linear_curve_groups->curve_selection, Threshold);
		detail::print_diff(Stream, "uniform data", A.linear_curve_groups->uniform_data, B.linear_curve_groups->uniform_data, Threshold);
		detail::print_diff(Stream, "curve points", A.linear_curve_groups->curve_points, B.linear_curve_groups->curve_points, Threshold);
	}

	detail::print_diff(Stream, "cubic curve groups", A.cubic_curve_groups, B.cubic_curve_groups);
	if(A.cubic_curve_groups && B.cubic_curve_groups)
	{
		detail::print_diff(Stream, "first curves", A.cubic_curve_groups->first_curves, B.cubic_curve_groups->first_curves, Threshold);
		detail::print_diff(Stream, "curve counts", A.cubic_curve_groups->curve_counts, B.cubic_curve_groups->curve_counts, Threshold);
		detail::print_diff(Stream, "periodic curves", A.cubic_curve_groups->periodic_curves, B.cubic_curve_groups->periodic_curves, Threshold);
		detail::print_diff(Stream, "materials", A.cubic_curve_groups->materials, B.cubic_curve_groups->materials, Threshold);
		detail::print_diff(Stream, "constant data", A.cubic_curve_groups->constant_data, B.cubic_curve_groups->constant_data, Threshold);
		detail::print_diff(Stream, "curve first points", A.cubic_curve_groups->curve_first_points, B.cubic_curve_groups->curve_first_points, Threshold);
		detail::print_diff(Stream, "curve point counts", A.cubic_curve_groups->curve_point_counts, B.cubic_curve_groups->curve_point_counts, Threshold);
		detail::print_diff(Stream, "curve selection", A.cubic_curve_groups->curve_selection, B.cubic_curve_groups->curve_selection, Threshold);
		detail::print_diff(Stream, "uniform data", A.cubic_curve_groups->uniform_data, B.cubic_curve_groups->uniform_data, Threshold);
		detail::print_diff(Stream, "curve points", A.cubic_curve_groups->curve_points, B.cubic_curve_groups->curve_points, Threshold);
	}

	detail::print_diff(Stream, "nurbs curve groups", A.nurbs_curve_groups, B.nurbs_curve_groups);
	if(A.nurbs_curve_groups && B.nurbs_curve_groups)
	{
		detail::print_diff(Stream, "first curves", A.nurbs_curve_groups->first_curves, B.nurbs_curve_groups->first_curves, Threshold);
		detail::print_diff(Stream, "curve counts", A.nurbs_curve_groups->curve_counts, B.nurbs_curve_groups->curve_counts, Threshold);
		detail::print_diff(Stream, "materials", A.nurbs_curve_groups->materials, B.nurbs_curve_groups->materials, Threshold);
		detail::print_diff(Stream, "constant data", A.nurbs_curve_groups->constant_data, B.nurbs_curve_groups->constant_data, Threshold);
		detail::print_diff(Stream, "curve first points", A.nurbs_curve_groups->curve_first_points, B.nurbs_curve_groups->curve_first_points, Threshold);
		detail::print_diff(Stream, "curve point counts", A.nurbs_curve_groups->curve_point_counts, B.nurbs_curve_groups->curve_point_counts, Threshold);
		detail::print_diff(Stream, "curve orders", A.nurbs_curve_groups->curve_orders, B.nurbs_curve_groups->curve_orders, Threshold);
		detail::print_diff(Stream, "curve selection", A.nurbs_curve_groups->curve_selection, B.nurbs_curve_groups->curve_selection, Threshold);
		detail::print_diff(Stream, "uniform data", A.nurbs_curve_groups->uniform_data, B.nurbs_curve_groups->uniform_data, Threshold);
		detail::print_diff(Stream, "curve points", A.nurbs_curve_groups->curve_points, B.nurbs_curve_groups->curve_points, Threshold);
		detail::print_diff(Stream, "curve point weights", A.nurbs_curve_groups->curve_point_weights, B.nurbs_curve_groups->curve_point_weights, Threshold);
		detail::print_diff(Stream, "curve knots", A.nurbs_curve_groups->curve_knots, B.nurbs_curve_groups->curve_knots, Threshold);
	}

	detail::print_diff(Stream, "bilinear patches", A.bilinear_patches, B.bilinear_patches);
	if(A.bilinear_patches && B.bilinear_patches)
	{
		detail::print_diff(Stream, "patch selection", A.bilinear_patches->patch_selection, B.bilinear_patches->patch_selection, Threshold);
		detail::print_diff(Stream, "patch materials", A.bilinear_patches->patch_materials, B.bilinear_patches->patch_materials, Threshold);
		detail::print_diff(Stream, "constant data", A.bilinear_patches->constant_data, B.bilinear_patches->constant_data, Threshold);
		detail::print_diff(Stream, "uniform data", A.bilinear_patches->uniform_data, B.bilinear_patches->uniform_data, Threshold);
		detail::print_diff(Stream, "patch points", A.bilinear_patches->patch_points, B.bilinear_patches->patch_points, Threshold);
		detail::print_diff(Stream, "varying data", A.bilinear_patches->varying_data, B.bilinear_patches->varying_data, Threshold);
	}

	detail::print_diff(Stream, "bicubic patches", A.bicubic_patches, B.bicubic_patches);
	if(A.bicubic_patches && B.bicubic_patches)
	{
		detail::print_diff(Stream, "patch selection", A.bicubic_patches->patch_selection, B.bicubic_patches->patch_selection, Threshold);
		detail::print_diff(Stream, "patch materials", A.bicubic_patches->patch_materials, B.bicubic_patches->patch_materials, Threshold);
		detail::print_diff(Stream, "constant data", A.bicubic_patches->constant_data, B.bicubic_patches->constant_data, Threshold);
		detail::print_diff(Stream, "uniform data", A.bicubic_patches->uniform_data, B.bicubic_patches->uniform_data, Threshold);
		detail::print_diff(Stream, "patch points", A.bicubic_patches->patch_points, B.bicubic_patches->patch_points, Threshold);
		detail::print_diff(Stream, "varying data", A.bicubic_patches->varying_data, B.bicubic_patches->varying_data, Threshold);
	}

	detail::print_diff(Stream, "nurbs patches", A.nurbs_patches, B.nurbs_patches);
	if(A.nurbs_patches && B.nurbs_patches)
	{
		detail::print_diff(Stream, "patch first points", A.nurbs_patches->patch_first_points, B.nurbs_patches->patch_first_points, Threshold);
		detail::print_diff(Stream, "patch u point counts", A.nurbs_patches->patch_u_point_counts, B.nurbs_patches->patch_u_point_counts, Threshold);
		detail::print_diff(Stream, "patch v point counts", A.nurbs_patches->patch_v_point_counts, B.nurbs_patches->patch_v_point_counts, Threshold);
		detail::print_diff(Stream, "patch u orders", A.nurbs_patches->patch_u_orders, B.nurbs_patches->patch_u_orders, Threshold);
		detail::print_diff(Stream, "patch v orders", A.nurbs_patches->patch_v_orders, B.nurbs_patches->patch_v_orders, Threshold);
		detail::print_diff(Stream, "patch selection", A.nurbs_patches->patch_selection, B.nurbs_patches->patch_selection, Threshold);
		detail::print_diff(Stream, "patch materials", A.nurbs_patches->patch_materials, B.nurbs_patches->patch_materials, Threshold);
		detail::print_diff(Stream, "constant data", A.nurbs_patches->constant_data, B.nurbs_patches->constant_data, Threshold);
		detail::print_diff(Stream, "uniform data", A.nurbs_patches->uniform_data, B.nurbs_patches->uniform_data, Threshold);
		detail::print_diff(Stream, "patch points", A.nurbs_patches->patch_points, B.nurbs_patches->patch_points, Threshold);
		detail::print_diff(Stream, "patch point weights", A.nurbs_patches->patch_point_weights, B.nurbs_patches->patch_point_weights, Threshold);
		detail::print_diff(Stream, "patch u knots", A.nurbs_patches->patch_u_knots, B.nurbs_patches->patch_u_knots, Threshold);
		detail::print_diff(Stream, "patch v knots", A.nurbs_patches->patch_v_knots, B.nurbs_patches->patch_v_knots, Threshold);
		detail::print_diff(Stream, "varying data", A.nurbs_patches->varying_data, B.nurbs_patches->varying_data, Threshold);
		if (A.nurbs_patches->patch_trim_curve_loop_counts && B.nurbs_patches->patch_trim_curve_loop_counts)
		{
			detail::print_diff(Stream, "patch_trim_curve_loop_counts", A.nurbs_patches->patch_trim_curve_loop_counts, B.nurbs_patches->patch_trim_curve_loop_counts, Threshold);
			detail::print_diff(Stream, "patch_first_trim_curve_loops", A.nurbs_patches->patch_first_trim_curve_loops, B.nurbs_patches->patch_first_trim_curve_loops, Threshold);
			detail::print_diff(Stream, "trim_points", A.nurbs_patches->trim_points, B.nurbs_patches->trim_points, Threshold);
			detail::print_diff(Stream, "trim_point_selection", A.nurbs_patches->trim_point_selection, B.nurbs_patches->trim_point_selection, Threshold);
			detail::print_diff(Stream, "first_trim_curves", A.nurbs_patches->first_trim_curves, B.nurbs_patches->first_trim_curves, Threshold);
			detail::print_diff(Stream, "trim_curve_counts", A.nurbs_patches->trim_curve_counts, B.nurbs_patches->trim_curve_counts, Threshold);
			detail::print_diff(Stream, "trim_curve_loop_selection", A.nurbs_patches->trim_curve_loop_selection, B.nurbs_patches->trim_curve_loop_selection, Threshold);
			detail::print_diff(Stream, "trim_curve_first_points", A.nurbs_patches->trim_curve_first_points, B.nurbs_patches->trim_curve_first_points, Threshold);
			detail::print_diff(Stream, "trim_curve_point_counts", A.nurbs_patches->trim_curve_point_counts, B.nurbs_patches->trim_curve_point_counts, Threshold);
			detail::print_diff(Stream, "trim_curve_orders", A.nurbs_patches->trim_curve_orders, B.nurbs_patches->trim_curve_orders, Threshold);
			detail::print_diff(Stream, "trim_curve_first_knots", A.nurbs_patches->trim_curve_first_knots, B.nurbs_patches->trim_curve_first_knots, Threshold);
			detail::print_diff(Stream, "trim_curve_selection", A.nurbs_patches->trim_curve_selection, B.nurbs_patches->trim_curve_selection, Threshold);
			detail::print_diff(Stream, "trim_curve_points", A.nurbs_patches->trim_curve_points, B.nurbs_patches->trim_curve_points, Threshold);
			detail::print_diff(Stream, "trim_curve_point_weights", A.nurbs_patches->trim_curve_point_weights, B.nurbs_patches->trim_curve_point_weights, Threshold);
			detail::print_diff(Stream, "trim_curve_knots", A.nurbs_patches->trim_curve_knots, B.nurbs_patches->trim_curve_knots, Threshold);
		}
	}

	detail::print_diff(Stream, "polyhedra", A.polyhedra, B.polyhedra);
	if(A.polyhedra && B.polyhedra)
	{
		detail::print_diff(Stream, "first faces", A.polyhedra->first_faces, B.polyhedra->first_faces, Threshold);
		detail::print_diff(Stream, "face counts", A.polyhedra->face_counts, B.polyhedra->face_counts, Threshold);
		detail::print_diff(Stream, "types", A.polyhedra->types, B.polyhedra->types, Threshold);
		detail::print_diff(Stream, "constant data", A.polyhedra->constant_data, B.polyhedra->constant_data, Threshold);
		detail::print_diff(Stream, "face first loops", A.polyhedra->face_first_loops, B.polyhedra->face_first_loops, Threshold);
		detail::print_diff(Stream, "face loop counts", A.polyhedra->face_loop_counts, B.polyhedra->face_loop_counts, Threshold);
		detail::print_diff(Stream, "face selection", A.polyhedra->face_selection, B.polyhedra->face_selection, Threshold);
		detail::print_diff(Stream, "face materials", A.polyhedra->face_materials, B.polyhedra->face_materials, Threshold);
		detail::print_diff(Stream, "uniform data", A.polyhedra->uniform_data, B.polyhedra->uniform_data, Threshold);
		detail::print_diff(Stream, "loop first edges", A.polyhedra->loop_first_edges, B.polyhedra->loop_first_edges, Threshold);
		detail::print_diff(Stream, "edge points", A.polyhedra->edge_points, B.polyhedra->edge_points, Threshold);
		detail::print_diff(Stream, "clockwise edges", A.polyhedra->clockwise_edges, B.polyhedra->clockwise_edges, Threshold);
		detail::print_diff(Stream, "edge selection", A.polyhedra->edge_selection, B.polyhedra->edge_selection, Threshold);
		detail::print_diff(Stream, "face varying data", A.polyhedra->face_varying_data, B.polyhedra->face_varying_data, Threshold);
	}

	detail::print_diff(Stream, "blobbies", A.blobbies, B.blobbies);
	if(A.blobbies && B.blobbies)
	{
		detail::print_diff(Stream, "first primitives", A.blobbies->first_primitives, B.blobbies->first_primitives, Threshold);
		detail::print_diff(Stream, "primitive counts", A.blobbies->primitive_counts, B.blobbies->primitive_counts, Threshold);
		detail::print_diff(Stream, "first operators", A.blobbies->first_operators, B.blobbies->first_operators, Threshold);
		detail::print_diff(Stream, "operator counts", A.blobbies->operator_counts, B.blobbies->operator_counts, Threshold);
		detail::print_diff(Stream, "materials", A.blobbies->materials, B.blobbies->materials, Threshold);
		detail::print_diff(Stream, "constant data", A.blobbies->constant_data, B.blobbies->constant_data, Threshold);
		detail::print_diff(Stream, "uniform data", A.blobbies->uniform_data, B.blobbies->uniform_data, Threshold);
		detail::print_diff(Stream, "primitives", A.blobbies->primitives, B.blobbies->primitives, Threshold);
		detail::print_diff(Stream, "primitive first floats", A.blobbies->primitive_first_floats, B.blobbies->primitive_first_floats, Threshold);
		detail::print_diff(Stream, "primitive float counts", A.blobbies->primitive_float_counts, B.blobbies->primitive_float_counts, Threshold);
		detail::print_diff(Stream, "varying data", A.blobbies->varying_data, B.blobbies->varying_data, Threshold);
		detail::print_diff(Stream, "vertex data", A.blobbies->vertex_data, B.blobbies->vertex_data, Threshold);
		detail::print_diff(Stream, "operators", A.blobbies->operators, B.blobbies->operators, Threshold);
		detail::print_diff(Stream, "operator first operands", A.blobbies->operator_first_operands, B.blobbies->operator_first_operands, Threshold);
		detail::print_diff(Stream, "operator operand counts", A.blobbies->operator_operand_counts, B.blobbies->operator_operand_counts, Threshold);
		detail::print_diff(Stream, "floats", A.blobbies->floats, B.blobbies->floats, Threshold);
		detail::print_diff(Stream, "operands", A.blobbies->operands, B.blobbies->operands, Threshold);
	}

	detail::print_diff(Stream, "mesh", &A, &B);
	detail::print_diff(Stream, "points", A.points, B.points, Threshold);
	detail::print_diff(Stream, "point selection", A.point_selection, B.point_selection, Threshold);
	detail::print_diff(Stream, "vertex data", A.vertex_data, B.vertex_data, Threshold);
}

} // namespace k3d

