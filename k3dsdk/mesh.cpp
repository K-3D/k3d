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

#include "color.h"
#include "imaterial.h"
#include "iomanip.h"
#include "legacy_mesh.h"
#include "mesh.h"
#include "shared_pointer.h"
#include "type_registry.h"

#include <boost/mpl/for_each.hpp>

#include <iterator>
#include <map>

namespace k3d
{

namespace detail
{

////////////////////////////////////////////////////////////////////////////////////////////
// indentation

/// Inserts whitespace into a stream, proportional to its indentation level
std::ostream& indentation(std::ostream& Stream)
{
	Stream << std::string(2 * current_indent(Stream), ' '); 
	return Stream;
}

////////////////////////////////////////////////////////////////////////////////////////////
// print

template<typename pointer_type>
void print(std::ostream& Stream, const string_t& Label, const pointer_type& Pointer)
{
	if(Pointer)
	{
		typedef typename pointer_type::element_type array_type;
		typedef typename array_type::value_type value_type;

		Stream << indentation << Label;
		if(type_registered<value_type>())
			Stream << " [" << type_string<value_type>() <<  "]";
		Stream << " (" << Pointer->size() <<  "): ";
		std::copy(Pointer->begin(), Pointer->end(), std::ostream_iterator<typename pointer_type::element_type::value_type>(Stream, " "));
		Stream << "\n";
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// print_array

class print_array
{
public:
	print_array(std::ostream& Stream, const string_t& ArrayName, const array& Array, bool_t& Printed) :
		m_stream(Stream),
		m_array_name(ArrayName),
		m_array(Array),
		m_printed(Printed)
	{
		if(const uint_t_array* const array = dynamic_cast<const uint_t_array*>(&m_array))
		{
			m_printed = true;
			m_stream << indentation << "array \"" << m_array_name << "\" [k3d::uint_t] (" << m_array.size() << "): ";
			std::copy(array->begin(), array->end(), std::ostream_iterator<uint_t>(m_stream, " "));
			m_stream << "\n";
		}
	}

	template<typename T>
	void operator()(T)
	{
		if(m_printed)
			return;

		if(const typed_array<T>* const array = dynamic_cast<const typed_array<T>*>(&m_array))
		{
			m_printed = true;
			m_stream << indentation << "array \"" << m_array_name << "\" [" << type_string<T>() << "] (" << m_array.size() << "): ";
			std::copy(array->begin(), array->end(), std::ostream_iterator<T>(m_stream, " "));
			m_stream << "\n";
		}
	}

	// Special-case 8-bit integers so they don't get printed as characters
	void operator()(int8_t)
	{
		typedef int8_t T;

		if(m_printed)
			return;

		if(const typed_array<T>* const array = dynamic_cast<const typed_array<T>*>(&m_array))
		{
			m_printed = true;
			m_stream << indentation << "array \"" << m_array_name << "\" [" << type_string<T>() << "] (" << m_array.size() << "): ";
			std::copy(array->begin(), array->end(), std::ostream_iterator<int16_t>(m_stream, " "));
			m_stream << "\n";
		}
	}

	// Special-case 8-bit unsigned integers so they don't get printed as characters
	void operator()(uint8_t)
	{
		typedef uint8_t T;

		if(m_printed)
			return;

		if(const typed_array<T>* const array = dynamic_cast<const typed_array<T>*>(&m_array))
		{
			m_printed = true;
			m_stream << indentation << "array \"" << m_array_name << "\" [" << type_string<T>() << "] (" << m_array.size() << "): ";
			std::copy(array->begin(), array->end(), std::ostream_iterator<uint16_t>(m_stream, " "));
			m_stream << "\n";
		}
	}

	// Special-case strings so we handle whitespace correctly
	void operator()(string_t)
	{
		typedef string_t T;

		if(m_printed)
			return;

		if(const typed_array<T>* const array = dynamic_cast<const typed_array<T>*>(&m_array))
		{
			m_printed = true;
			m_stream << indentation << "array \"" << m_array_name << "\" [" << type_string<T>() << "] (" << m_array.size() << "): ";
			for(typed_array<T>::const_iterator i = array->begin(); i != array->end(); ++i)
				m_stream << "\"" << *i << "\" ";
			m_stream << "\n";
		}
	}

private:
	std::ostream& m_stream;
	const string_t& m_array_name;
	const array& m_array;
	bool_t& m_printed;
};

////////////////////////////////////////////////////////////////////////////////////////////
// print

void print(std::ostream& Stream, const string_t& Label, const mesh::named_arrays_t& Arrays)
{
	Stream << indentation << Label << " (" << Arrays.size() << "):\n" << push_indent;
	for(mesh::attribute_arrays_t::const_iterator array_iterator = Arrays.begin(); array_iterator != Arrays.end(); ++array_iterator)
	{
		bool_t printed = false;
		boost::mpl::for_each<named_array_types>(print_array(Stream, array_iterator->first, *array_iterator->second, printed));
		if(!printed)
			log() << error << k3d_file_reference << ": array [" << array_iterator->first << "] with unknown type [" << demangle(typeid(*array_iterator->second)) << "] will not be printed" << std::endl;
	}
	Stream << pop_indent;
}

////////////////////////////////////////////////////////////////////////////////////////////
// print

void print(std::ostream& Stream, const string_t& Label, const mesh::attribute_arrays_t& Arrays)
{
	Stream << indentation << Label << " (" << Arrays.size() << "):\n" << push_indent;
	for(mesh::attribute_arrays_t::const_iterator array_iterator = Arrays.begin(); array_iterator != Arrays.end(); ++array_iterator)
	{
		bool_t printed = false;
		boost::mpl::for_each<named_array_types>(print_array(Stream, array_iterator->first, *array_iterator->second, printed));
		if(!printed)
			log() << error << k3d_file_reference << ": array [" << array_iterator->first << "] with unknown type [" << demangle(typeid(*array_iterator->second)) << "] will not be printed" << std::endl;
	}
	Stream << pop_indent;
}

////////////////////////////////////////////////////////////////////////////////////////////
// print

void print(std::ostream& Stream, const string_t& Label, const mesh::named_attribute_arrays_t& Attributes)
{
	Stream << indentation << Label << " (" << Attributes.size() << "):\n" << push_indent;
	for(mesh::named_attribute_arrays_t::const_iterator attributes = Attributes.begin(); attributes != Attributes.end(); ++attributes)
		print(Stream, "attributes \"" + attributes->first + "\"", attributes->second);
	Stream << pop_indent;
}

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

/// Return true iff two shared arrays are equivalent (handles cases where they point to the same memory, and handles "fuzzy" floating-point comparisons).
template<typename T>
const bool_t almost_equal(const boost::shared_ptr<const typed_array<T> >& A, const boost::shared_ptr<const typed_array<T> >& B, const uint64_t Threshold)
{
	if(A.get() == B.get())
		return true;

	if(A && B)
		return A->almost_equal(*B, Threshold);

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

/// Return true iff two shared arrays are equivalent (handles cases where they point to the same memory, and handles "fuzzy" floating-point comparisons).
const bool_t almost_equal(const boost::shared_ptr<const uint_t_array>& A, const boost::shared_ptr<const uint_t_array>& B, const uint64_t Threshold)
{
	if(A.get() == B.get())
		return true;

	if(A && B)
		return A->almost_equal(*B, Threshold);

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

/// Return true iff two shared objects are equivalent (handles cases where they point to the same memory, and handles "fuzzy" floating-point comparisons).
template<typename T>
const bool_t almost_equal(const boost::shared_ptr<const T>& A, const boost::shared_ptr<const T>& B, const uint64_t Threshold)
{
	if(A.get() == B.get())
		return true;

	if(A && B)
		return k3d::almost_equal<T>(Threshold)(*A, *B);

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

/// Return true iff two sets of attributes arrays are equivalent (we provide this function mainly for consistency).

const bool_t almost_equal(const mesh::attribute_arrays_t& A, const mesh::attribute_arrays_t& B, const uint64_t Threshold)
{
	return k3d::almost_equal<mesh::attribute_arrays_t>(Threshold)(A, B);
}

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

/// Return true iff two sets of primitives are equivalent.

const bool_t almost_equal(const mesh::primitives_t& A, const mesh::primitives_t& B, const uint64_t Threshold)
{
	// If we have differing numbers of primitives, they definitely aren't equal
	if(A.size() != B.size())
		return false;

	for(mesh::primitives_t::const_iterator a = A.begin(), b = B.begin(); a != A.end() && b != B.end(); ++a, ++b)
	{
		// If both primitives point to the same memory, they're equal
		if(a->get() == b->get())
			continue;

		// Perform element-wise comparisons of the two primitives ...
		if(a->get() && b->get())
		{
			if((**a).almost_equal((**b), Threshold))
				continue;
		}

		// Either the element-wise comparison failed or one array was NULL and the other wasn't
		return false;
	}

	return true;
}


} // namespace detail

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

template<>
class almost_equal<mesh::point_groups_t>
{
	typedef mesh::point_groups_t T;
public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B)
	{
		return
			detail::almost_equal(A.first_points, B.first_points, threshold) &&
			detail::almost_equal(A.point_counts, B.point_counts, threshold) &&
			detail::almost_equal(A.materials, B.materials, threshold) &&
			detail::almost_equal(A.constant_data, B.constant_data, threshold) &&
			detail::almost_equal(A.points, B.points, threshold) &&
			detail::almost_equal(A.varying_data, B.varying_data, threshold);
	}
private:
	const uint64_t threshold;
};

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

template<>
class almost_equal<mesh::linear_curve_groups_t>
{
	typedef mesh::linear_curve_groups_t T;
public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B)
	{
		return
			detail::almost_equal(A.first_curves, B.first_curves, threshold) &&
			detail::almost_equal(A.curve_counts, B.curve_counts, threshold) &&
			detail::almost_equal(A.periodic_curves, B.periodic_curves, threshold) &&
			detail::almost_equal(A.materials, B.materials, threshold) &&
			detail::almost_equal(A.constant_data, B.constant_data, threshold) &&
			detail::almost_equal(A.curve_first_points, B.curve_first_points, threshold) &&
			detail::almost_equal(A.curve_point_counts, B.curve_point_counts, threshold) &&
			detail::almost_equal(A.curve_selection, B.curve_selection, threshold) &&
			detail::almost_equal(A.uniform_data, B.uniform_data, threshold) &&
			detail::almost_equal(A.curve_points, B.curve_points, threshold);
	}
private:
	const uint64_t threshold;
};

////////////////////////////////////////////////////////////////////////////////////////////
// detail::almost_equal

template<>
class almost_equal<mesh::cubic_curve_groups_t>
{
	typedef mesh::cubic_curve_groups_t T;
public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B)
	{
		return
			detail::almost_equal(A.first_curves, B.first_curves, threshold) &&
			detail::almost_equal(A.curve_counts, B.curve_counts, threshold) &&
			detail::almost_equal(A.periodic_curves, B.periodic_curves, threshold) &&
			detail::almost_equal(A.materials, B.materials, threshold) &&
			detail::almost_equal(A.constant_data, B.constant_data, threshold) &&
			detail::almost_equal(A.curve_first_points, B.curve_first_points, threshold) &&
			detail::almost_equal(A.curve_point_counts, B.curve_point_counts, threshold) &&
			detail::almost_equal(A.curve_selection, B.curve_selection, threshold) &&
			detail::almost_equal(A.uniform_data, B.uniform_data, threshold) &&
			detail::almost_equal(A.curve_points, B.curve_points, threshold);
	}
private:
	const uint64_t threshold;
};

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

template<>
class almost_equal<mesh::nurbs_curve_groups_t>
{
	typedef mesh::nurbs_curve_groups_t T;
public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B)
	{
		return
			detail::almost_equal(A.first_curves, B.first_curves, threshold) &&
			detail::almost_equal(A.curve_counts, B.curve_counts, threshold) &&
			detail::almost_equal(A.materials, B.materials, threshold) &&
			detail::almost_equal(A.constant_data, B.constant_data, threshold) &&
			detail::almost_equal(A.curve_first_points, B.curve_first_points, threshold) &&
			detail::almost_equal(A.curve_point_counts, B.curve_point_counts, threshold) &&
			detail::almost_equal(A.curve_orders, B.curve_orders, threshold) &&
			detail::almost_equal(A.curve_first_knots, B.curve_first_knots, threshold) &&
			detail::almost_equal(A.curve_selection, B.curve_selection, threshold) &&
			detail::almost_equal(A.uniform_data, B.uniform_data, threshold) &&
			detail::almost_equal(A.curve_points, B.curve_points, threshold) &&
			detail::almost_equal(A.curve_point_weights, B.curve_point_weights, threshold) &&
			detail::almost_equal(A.curve_knots, B.curve_knots, threshold);
	}
private:
	const uint64_t threshold;
};

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

template<>
class almost_equal<mesh::bilinear_patches_t>
{
	typedef mesh::bilinear_patches_t T;
public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B)
	{
		return
			detail::almost_equal(A.patch_selection, B.patch_selection, threshold) &&
			detail::almost_equal(A.patch_materials, B.patch_materials, threshold) &&
			detail::almost_equal(A.constant_data, B.constant_data, threshold) &&
			detail::almost_equal(A.uniform_data, B.uniform_data, threshold) &&
			detail::almost_equal(A.patch_points, B.patch_points, threshold) &&
			detail::almost_equal(A.varying_data, B.varying_data, threshold);
	}
private:
	const uint64_t threshold;
};

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

template<>
class almost_equal<mesh::bicubic_patches_t>
{
	typedef mesh::bicubic_patches_t T;
public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B)
	{
		return
			detail::almost_equal(A.patch_selection, B.patch_selection, threshold) &&
			detail::almost_equal(A.patch_materials, B.patch_materials, threshold) &&
			detail::almost_equal(A.constant_data, B.constant_data, threshold) &&
			detail::almost_equal(A.uniform_data, B.uniform_data, threshold) &&
			detail::almost_equal(A.patch_points, B.patch_points, threshold) &&
			detail::almost_equal(A.varying_data, B.varying_data, threshold);
	}
private:
	const uint64_t threshold;
};

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

template<>
class almost_equal<mesh::nurbs_patches_t>
{
	typedef mesh::nurbs_patches_t T;
public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B)
	{
		const bool_t patches_equal =
			detail::almost_equal(A.patch_first_points, B.patch_first_points, threshold) &&
			detail::almost_equal(A.patch_u_point_counts, B.patch_u_point_counts, threshold) &&
			detail::almost_equal(A.patch_v_point_counts, B.patch_v_point_counts, threshold) &&
			detail::almost_equal(A.patch_u_orders, B.patch_u_orders, threshold) &&
			detail::almost_equal(A.patch_v_orders, B.patch_v_orders, threshold) &&
			detail::almost_equal(A.patch_u_first_knots, B.patch_u_first_knots, threshold) &&
			detail::almost_equal(A.patch_v_first_knots, B.patch_v_first_knots, threshold) &&
			detail::almost_equal(A.patch_selection, B.patch_selection, threshold) &&
			detail::almost_equal(A.patch_materials, B.patch_materials, threshold) &&
			detail::almost_equal(A.constant_data, B.constant_data, threshold) &&
			detail::almost_equal(A.uniform_data, B.uniform_data, threshold) &&
			detail::almost_equal(A.patch_points, B.patch_points, threshold) &&
			detail::almost_equal(A.patch_point_weights, B.patch_point_weights, threshold) &&
			detail::almost_equal(A.patch_u_knots, B.patch_u_knots, threshold) &&
			detail::almost_equal(A.patch_v_knots, B.patch_v_knots, threshold) &&
			detail::almost_equal(A.varying_data, B.varying_data, threshold);

		bool_t trim_curves_equal = true;
		if (A.patch_trim_curve_loop_counts && B.patch_trim_curve_loop_counts)
		{
			trim_curves_equal =
				detail::almost_equal(A.patch_trim_curve_loop_counts, B.patch_trim_curve_loop_counts, threshold) &&
				detail::almost_equal(A.patch_first_trim_curve_loops, B.patch_first_trim_curve_loops, threshold) &&
				detail::almost_equal(A.trim_points, B.trim_points, threshold) &&
				detail::almost_equal(A.trim_point_selection, B.trim_point_selection, threshold) &&
				detail::almost_equal(A.first_trim_curves, B.first_trim_curves, threshold) &&
				detail::almost_equal(A.trim_curve_counts, B.trim_curve_counts, threshold) &&
				detail::almost_equal(A.trim_curve_loop_selection, B.trim_curve_loop_selection, threshold) &&
				detail::almost_equal(A.trim_curve_first_points, B.trim_curve_first_points, threshold) &&
				detail::almost_equal(A.trim_curve_point_counts, B.trim_curve_point_counts, threshold) &&
				detail::almost_equal(A.trim_curve_orders, B.trim_curve_orders, threshold) &&
				detail::almost_equal(A.trim_curve_first_knots, B.trim_curve_first_knots, threshold) &&
				detail::almost_equal(A.trim_curve_selection, B.trim_curve_selection, threshold) &&
				detail::almost_equal(A.trim_curve_points, B.trim_curve_points, threshold) &&
				detail::almost_equal(A.trim_curve_point_weights, B.trim_curve_point_weights, threshold) &&
				detail::almost_equal(A.trim_curve_knots, B.trim_curve_knots, threshold);
		}
		
		return patches_equal && trim_curves_equal;
	}
private:
	const uint64_t threshold;
};

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

template<>
class almost_equal<mesh::polyhedra_t>
{
	typedef mesh::polyhedra_t T;
public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B)
	{
		return
			detail::almost_equal(A.first_faces, B.first_faces, threshold) &&
			detail::almost_equal(A.face_counts, B.face_counts, threshold) &&
			detail::almost_equal(A.types, B.types, threshold) &&
			detail::almost_equal(A.constant_data, B.constant_data, threshold) &&
			detail::almost_equal(A.face_first_loops, B.face_first_loops, threshold) &&
			detail::almost_equal(A.face_loop_counts, B.face_loop_counts, threshold) &&
			detail::almost_equal(A.face_selection, B.face_selection, threshold) &&
			detail::almost_equal(A.face_materials, B.face_materials, threshold) &&
			detail::almost_equal(A.uniform_data, B.uniform_data, threshold) &&
			detail::almost_equal(A.loop_first_edges, B.loop_first_edges, threshold) &&
			detail::almost_equal(A.edge_points, B.edge_points, threshold) &&
			detail::almost_equal(A.clockwise_edges, B.clockwise_edges, threshold) &&
			detail::almost_equal(A.edge_selection, B.edge_selection, threshold) &&
			detail::almost_equal(A.face_varying_data, B.face_varying_data, threshold);
	}
private:
	const uint64_t threshold;
};

////////////////////////////////////////////////////////////////////////////////////////////
// almost_equal

template<>
class almost_equal<mesh::blobbies_t>
{
	typedef mesh::blobbies_t T;
public:
	almost_equal(const uint64_t Threshold) :
		threshold(Threshold)
	{
	}

	inline const bool_t operator()(const T& A, const T& B)
	{
		return
			detail::almost_equal(A.first_primitives, B.first_primitives, threshold) &&
			detail::almost_equal(A.primitive_counts, B.primitive_counts, threshold) &&
			detail::almost_equal(A.first_operators, B.first_operators, threshold) &&
			detail::almost_equal(A.operator_counts, B.operator_counts, threshold) &&
			detail::almost_equal(A.materials, B.materials, threshold) &&
			detail::almost_equal(A.constant_data, B.constant_data, threshold) &&
			detail::almost_equal(A.uniform_data, B.uniform_data, threshold) &&
			detail::almost_equal(A.primitives, B.primitives, threshold) &&
			detail::almost_equal(A.primitive_first_floats, B.primitive_first_floats, threshold) &&
			detail::almost_equal(A.primitive_float_counts, B.primitive_float_counts, threshold) &&
			detail::almost_equal(A.varying_data, B.varying_data, threshold) &&
			detail::almost_equal(A.vertex_data, B.vertex_data, threshold) &&
			detail::almost_equal(A.operators, B.operators, threshold) &&
			detail::almost_equal(A.operator_first_operands, B.operator_first_operands, threshold) &&
			detail::almost_equal(A.operator_operand_counts, B.operator_operand_counts, threshold) &&
			detail::almost_equal(A.floats, B.floats, threshold) &&
			detail::almost_equal(A.operands, B.operands, threshold);
	}
private:
	const uint64_t threshold;
};

////////////////////////////////////////////////////////////////////////////////////
// mesh

mesh::mesh()
{
}

const bool_t mesh::almost_equal(const mesh& Other, const uint64_t Threshold) const
{
	return
		detail::almost_equal(points, Other.points, Threshold) &&
		detail::almost_equal(point_selection, Other.point_selection, Threshold) &&
		detail::almost_equal(vertex_data, Other.vertex_data, Threshold) &&
		detail::almost_equal(primitives, Other.primitives, Threshold) &&

		detail::almost_equal(point_groups, Other.point_groups, Threshold) &&
		detail::almost_equal(linear_curve_groups, Other.linear_curve_groups, Threshold) &&
		detail::almost_equal(cubic_curve_groups, Other.cubic_curve_groups, Threshold) &&
		detail::almost_equal(nurbs_curve_groups, Other.nurbs_curve_groups, Threshold) &&
		detail::almost_equal(bilinear_patches, Other.bilinear_patches, Threshold) &&
		detail::almost_equal(bicubic_patches, Other.bicubic_patches, Threshold) &&
		detail::almost_equal(nurbs_patches, Other.nurbs_patches, Threshold) &&
		detail::almost_equal(polyhedra, Other.polyhedra, Threshold) &&
		detail::almost_equal(blobbies, Other.blobbies, Threshold);
}

mesh& mesh::operator=(const legacy::mesh& RHS)
{
	// Convert points ...
	std::map<legacy::point*, uint_t> point_map;

	const uint_t point_size = RHS.points.size();
	boost::shared_ptr<points_t> points(new points_t(point_size));
	boost::shared_ptr<selection_t> point_selection(new selection_t(point_size));

	for(uint_t i = 0; i != point_size; ++i)
	{
		(*points)[i] = RHS.points[i]->position;
		(*point_selection)[i] = RHS.points[i]->selection_weight;
		point_map[RHS.points[i]] = i;
	}

	this->points = points;
	this->point_selection = point_selection;

	// Convert point groups ...
	if(RHS.point_groups.size())
	{
		boost::shared_ptr<point_groups_t> point_groups(new point_groups_t());
		boost::shared_ptr<indices_t> first_points(new indices_t());
		boost::shared_ptr<counts_t> point_counts(new counts_t());
		boost::shared_ptr<materials_t> materials(new materials_t());
		boost::shared_ptr<indices_t> points(new indices_t());

		for(legacy::mesh::point_groups_t::const_iterator group = RHS.point_groups.begin(); group != RHS.point_groups.end(); ++group)
		{
			first_points->push_back(points->size());
			point_counts->push_back((*group)->points.size());
			materials->push_back((*group)->material);

			for(legacy::point_group::points_t::const_iterator point = (*group)->points.begin(); point != (*group)->points.end(); ++point)
				points->push_back(point_map[*point]);
		}

		point_groups->first_points = first_points;
		point_groups->point_counts = point_counts;
		point_groups->materials = materials;
		point_groups->points = points;

		this->point_groups = point_groups;
	}

	// Convert linear curves ...
	if(RHS.linear_curve_groups.size())
	{
		boost::shared_ptr<linear_curve_groups_t> linear_curve_groups(new linear_curve_groups_t());
		boost::shared_ptr<indices_t> first_curves(new indices_t());
		boost::shared_ptr<counts_t> curve_counts(new counts_t());
		boost::shared_ptr<bools_t> periodic_curves(new bools_t());
		boost::shared_ptr<materials_t> materials(new materials_t());
		boost::shared_ptr<indices_t> curve_first_points(new indices_t());
		boost::shared_ptr<counts_t> curve_point_counts(new counts_t());
		boost::shared_ptr<selection_t> curve_selection(new selection_t());
		boost::shared_ptr<indices_t> curve_points(new indices_t());

		for(legacy::mesh::linear_curve_groups_t::const_iterator group = RHS.linear_curve_groups.begin(); group != RHS.linear_curve_groups.end(); ++group)
		{
			first_curves->push_back(curve_first_points->size());
			curve_counts->push_back((*group)->curves.size());
			periodic_curves->push_back((*group)->wrap);
			materials->push_back((*group)->material);

			for(legacy::linear_curve_group::curves_t::const_iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			{
				curve_first_points->push_back(curve_points->size());
				curve_point_counts->push_back((*curve)->control_points.size());
				curve_selection->push_back((*curve)->selection_weight);

				for(legacy::linear_curve::control_points_t::const_iterator point = (*curve)->control_points.begin(); point != (*curve)->control_points.end(); ++point)
					curve_points->push_back(point_map[*point]);
			}
		}

		linear_curve_groups->first_curves = first_curves;
		linear_curve_groups->curve_counts = curve_counts;
		linear_curve_groups->periodic_curves = periodic_curves;
		linear_curve_groups->materials = materials;
		linear_curve_groups->curve_first_points = curve_first_points;
		linear_curve_groups->curve_point_counts = curve_point_counts;
		linear_curve_groups->curve_selection = curve_selection;
		linear_curve_groups->curve_points = curve_points;

		this->linear_curve_groups = linear_curve_groups;
	}

	// Convert cubic curves ...
	if(RHS.cubic_curve_groups.size())
	{
		boost::shared_ptr<cubic_curve_groups_t> cubic_curve_groups(new cubic_curve_groups_t());
		boost::shared_ptr<indices_t> first_curves(new indices_t());
		boost::shared_ptr<counts_t> curve_counts(new counts_t());
		boost::shared_ptr<bools_t> periodic_curves(new bools_t());
		boost::shared_ptr<materials_t> materials(new materials_t());
		boost::shared_ptr<indices_t> curve_first_points(new indices_t());
		boost::shared_ptr<counts_t> curve_point_counts(new counts_t());
		boost::shared_ptr<selection_t> curve_selection(new selection_t());
		boost::shared_ptr<indices_t> curve_points(new indices_t());

		for(legacy::mesh::cubic_curve_groups_t::const_iterator group = RHS.cubic_curve_groups.begin(); group != RHS.cubic_curve_groups.end(); ++group)
		{
			first_curves->push_back(curve_first_points->size());
			curve_counts->push_back((*group)->curves.size());
			periodic_curves->push_back((*group)->wrap);
			materials->push_back((*group)->material);

			for(legacy::cubic_curve_group::curves_t::const_iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			{
				curve_first_points->push_back(curve_points->size());
				curve_point_counts->push_back((*curve)->control_points.size());
				curve_selection->push_back((*curve)->selection_weight);

				for(legacy::cubic_curve::control_points_t::const_iterator point = (*curve)->control_points.begin(); point != (*curve)->control_points.end(); ++point)
					curve_points->push_back(point_map[*point]);
			}
		}

		cubic_curve_groups->first_curves = first_curves;
		cubic_curve_groups->curve_counts = curve_counts;
		cubic_curve_groups->periodic_curves = periodic_curves;
		cubic_curve_groups->materials = materials;
		cubic_curve_groups->curve_first_points = curve_first_points;
		cubic_curve_groups->curve_point_counts = curve_point_counts;
		cubic_curve_groups->curve_selection = curve_selection;
		cubic_curve_groups->curve_points = curve_points;

		this->cubic_curve_groups = cubic_curve_groups;
	}

	// Convert NURBS curves ...
	if(RHS.nucurve_groups.size())
	{
		boost::shared_ptr<nurbs_curve_groups_t> nurbs_curve_groups(new nurbs_curve_groups_t());
		boost::shared_ptr<indices_t> first_curves(new indices_t());
		boost::shared_ptr<counts_t> curve_counts(new counts_t());
		boost::shared_ptr<materials_t> materials(new materials_t());
		boost::shared_ptr<indices_t> curve_first_points(new indices_t());
		boost::shared_ptr<counts_t> curve_point_counts(new counts_t());
		boost::shared_ptr<orders_t> curve_orders(new orders_t());
		boost::shared_ptr<indices_t> curve_first_knots(new indices_t());
		boost::shared_ptr<selection_t> curve_selection(new selection_t());
		boost::shared_ptr<indices_t> curve_points(new indices_t());
		boost::shared_ptr<weights_t> curve_point_weights(new weights_t());
		boost::shared_ptr<knots_t> curve_knots(new knots_t());

		for(legacy::mesh::nucurve_groups_t::const_iterator group = RHS.nucurve_groups.begin(); group != RHS.nucurve_groups.end(); ++group)
		{
			first_curves->push_back(curve_first_points->size());
			curve_counts->push_back((*group)->curves.size());
			materials->push_back((*group)->material);

			for(legacy::nucurve_group::curves_t::const_iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			{
				curve_first_points->push_back(curve_points->size());
				curve_point_counts->push_back((*curve)->knots.size() - (*curve)->order);
				curve_orders->push_back((*curve)->order);
				curve_first_knots->push_back(curve_knots->size());
				curve_selection->push_back((*curve)->selection_weight);

				for(legacy::nucurve::control_points_t::const_iterator point = (*curve)->control_points.begin(); point != (*curve)->control_points.end(); ++point)
				{
					curve_points->push_back(point_map[point->position]);
					curve_point_weights->push_back(point->weight);
				}

				curve_knots->insert(curve_knots->end(), (*curve)->knots.begin(), (*curve)->knots.end());
			}
		}

		nurbs_curve_groups->first_curves = first_curves;
		nurbs_curve_groups->curve_counts = curve_counts;
		nurbs_curve_groups->materials = materials;
		nurbs_curve_groups->curve_first_points = curve_first_points;
		nurbs_curve_groups->curve_point_counts = curve_point_counts;
		nurbs_curve_groups->curve_orders = curve_orders;
		nurbs_curve_groups->curve_first_knots = curve_first_knots;
		nurbs_curve_groups->curve_selection = curve_selection;
		nurbs_curve_groups->curve_points = curve_points;
		nurbs_curve_groups->curve_point_weights = curve_point_weights;
		nurbs_curve_groups->curve_knots = curve_knots;
		this->nurbs_curve_groups = nurbs_curve_groups;
	}

	// Convert bilinear patches ...
	if(RHS.bilinear_patches.size())
	{
		boost::shared_ptr<bilinear_patches_t> bilinear_patches(new bilinear_patches_t());
		boost::shared_ptr<selection_t> patch_selection(new selection_t());
		boost::shared_ptr<materials_t> patch_materials(new materials_t());
		boost::shared_ptr<indices_t> patch_points(new indices_t());

		for(legacy::mesh::bilinear_patches_t::const_iterator patch = RHS.bilinear_patches.begin(); patch != RHS.bilinear_patches.end(); ++patch)
		{
			patch_selection->push_back((*patch)->selection_weight);
			patch_materials->push_back((*patch)->material);

			for(legacy::bilinear_patch::control_points_t::const_iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
				patch_points->push_back(point_map[*point]);
		}

		bilinear_patches->patch_selection = patch_selection;
		bilinear_patches->patch_materials = patch_materials;
		bilinear_patches->patch_points = patch_points;
		this->bilinear_patches = bilinear_patches;
	}

	// Convert bicubic patches ...
	if(RHS.bicubic_patches.size())
	{
		boost::shared_ptr<bicubic_patches_t> bicubic_patches(new bicubic_patches_t());
		boost::shared_ptr<selection_t> patch_selection(new selection_t());
		boost::shared_ptr<materials_t> patch_materials(new materials_t());
		boost::shared_ptr<indices_t> patch_points(new indices_t());

		for(legacy::mesh::bicubic_patches_t::const_iterator patch = RHS.bicubic_patches.begin(); patch != RHS.bicubic_patches.end(); ++patch)
		{
			patch_selection->push_back((*patch)->selection_weight);
			patch_materials->push_back((*patch)->material);

			for(legacy::bicubic_patch::control_points_t::const_iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
				patch_points->push_back(point_map[*point]);
		}

		bicubic_patches->patch_selection = patch_selection;
		bicubic_patches->patch_materials = patch_materials;
		bicubic_patches->patch_points = patch_points;
		this->bicubic_patches = bicubic_patches;
	}

	// Convert NURBS patches ...
	if(RHS.nupatches.size())
	{
		boost::shared_ptr<nurbs_patches_t> nurbs_patches(new nurbs_patches_t());
		boost::shared_ptr<indices_t> patch_first_points(new indices_t());
		boost::shared_ptr<counts_t> patch_u_point_counts(new counts_t());
		boost::shared_ptr<counts_t> patch_v_point_counts(new counts_t());
		boost::shared_ptr<orders_t> patch_u_orders(new orders_t());
		boost::shared_ptr<orders_t> patch_v_orders(new orders_t());
		boost::shared_ptr<indices_t> patch_u_first_knots(new indices_t());
		boost::shared_ptr<indices_t> patch_v_first_knots(new indices_t());
		boost::shared_ptr<selection_t> patch_selection(new selection_t());
		boost::shared_ptr<materials_t> patch_materials(new materials_t());
		boost::shared_ptr<indices_t> patch_points(new indices_t());
		boost::shared_ptr<weights_t> patch_point_weights(new weights_t());
		boost::shared_ptr<knots_t> patch_u_knots(new knots_t());
		boost::shared_ptr<knots_t> patch_v_knots(new knots_t());

		for(legacy::mesh::nupatches_t::const_iterator patch = RHS.nupatches.begin(); patch != RHS.nupatches.end(); ++patch)
		{
			patch_first_points->push_back(patch_points->size());
			patch_u_point_counts->push_back((*patch)->u_knots.size() - (*patch)->u_order);
			patch_v_point_counts->push_back((*patch)->v_knots.size() - (*patch)->v_order);
			patch_u_orders->push_back((*patch)->u_order);
			patch_v_orders->push_back((*patch)->v_order);
			patch_u_first_knots->push_back(patch_u_knots->size());
			patch_v_first_knots->push_back(patch_v_knots->size());
			patch_selection->push_back((*patch)->selection_weight);
			patch_materials->push_back((*patch)->material);

			for(legacy::nupatch::control_points_t::const_iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
			{
				patch_points->push_back(point_map[point->position]);
				patch_point_weights->push_back(point->weight);
			}

			patch_u_knots->insert(patch_u_knots->end(), (*patch)->u_knots.begin(), (*patch)->u_knots.end());
			patch_v_knots->insert(patch_v_knots->end(), (*patch)->v_knots.begin(), (*patch)->v_knots.end());
		}

		nurbs_patches->patch_first_points = patch_first_points;
		nurbs_patches->patch_u_point_counts = patch_u_point_counts;
		nurbs_patches->patch_v_point_counts = patch_v_point_counts;
		nurbs_patches->patch_u_orders = patch_u_orders;
		nurbs_patches->patch_v_orders = patch_v_orders;
		nurbs_patches->patch_u_first_knots = patch_u_first_knots;
		nurbs_patches->patch_v_first_knots = patch_v_first_knots;
		nurbs_patches->patch_selection = patch_selection;
		nurbs_patches->patch_materials = patch_materials;
		nurbs_patches->patch_points = patch_points;
		nurbs_patches->patch_point_weights = patch_point_weights;
		nurbs_patches->patch_u_knots = patch_u_knots;
		nurbs_patches->patch_v_knots = patch_v_knots;
		this->nurbs_patches = nurbs_patches;
	}

	// Convert polyhedra ...
	if(RHS.polyhedra.size())
	{
		boost::shared_ptr<polyhedra_t> polyhedra(new polyhedra_t());
		boost::shared_ptr<indices_t> first_faces(new indices_t());
		boost::shared_ptr<counts_t> face_counts(new counts_t());
		boost::shared_ptr<polyhedra_t::types_t> types(new polyhedra_t::types_t());
		boost::shared_ptr<indices_t> face_first_loops(new indices_t());
		boost::shared_ptr<counts_t> face_loop_counts(new counts_t());
		boost::shared_ptr<selection_t> face_selection(new selection_t());
		boost::shared_ptr<materials_t> face_materials(new materials_t());
		boost::shared_ptr<indices_t> loop_first_edges(new indices_t());
		boost::shared_ptr<indices_t> edge_points(new indices_t());
		boost::shared_ptr<indices_t> clockwise_edges(new indices_t());
		boost::shared_ptr<selection_t> edge_selection(new selection_t());

		for(legacy::mesh::polyhedra_t::const_iterator polyhedron = RHS.polyhedra.begin(); polyhedron != RHS.polyhedra.end(); ++polyhedron)
		{
			uint_t first_face = face_first_loops->size();
			uint_t face_count = 0;
			mesh::polyhedra_t::polyhedron_type type = (*polyhedron)->type == legacy::polyhedron::POLYGONS ? mesh::polyhedra_t::POLYGONS : mesh::polyhedra_t::CATMULL_CLARK;

			for(legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				++face_count;

				uint_t face_first_loop = loop_first_edges->size();
				uint_t face_loop_count = 1 + (*face)->holes.size();

				const uint_t first_edge = edge_points->size();

				loop_first_edges->push_back(first_edge);
				for(legacy::split_edge* edge = (*face)->first_edge; edge; edge = edge->face_clockwise)
				{
					if(edge->vertex && edge->face_clockwise)
					{
						edge_points->push_back(point_map[edge->vertex]);
						clockwise_edges->push_back(edge_points->size());
						edge_selection->push_back(edge->selection_weight);
					}

					if(edge->face_clockwise == (*face)->first_edge)
					{
						clockwise_edges->back() = first_edge;
						break;
					}
				}

				for(legacy::face::holes_t::iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
				{
					const uint_t first_edge = edge_points->size();

					loop_first_edges->push_back(first_edge);
					for(legacy::split_edge* edge = *hole; edge; edge = edge->face_clockwise)
					{
						if(edge->vertex && edge->face_clockwise && edge->face_clockwise->vertex)
						{
							edge_points->push_back(point_map[edge->vertex]);
							clockwise_edges->push_back(edge_points->size());
							edge_selection->push_back(edge->selection_weight);
						}

						if(edge->face_clockwise == (*hole))
						{
							clockwise_edges->back() = first_edge;
							break;
						}
					}
				}

				face_first_loops->push_back(face_first_loop);
				face_loop_counts->push_back(face_loop_count);
				face_selection->push_back((*face)->selection_weight);
				face_materials->push_back((*face)->material);
			}

			first_faces->push_back(first_face);
			face_counts->push_back(face_count);
			types->push_back(type);
		}

		polyhedra->edge_selection = edge_selection;
		polyhedra->clockwise_edges = clockwise_edges;
		polyhedra->edge_points = edge_points;
		polyhedra->loop_first_edges = loop_first_edges;
		polyhedra->face_materials = face_materials;
		polyhedra->face_selection = face_selection;
		polyhedra->face_loop_counts = face_loop_counts;
		polyhedra->face_first_loops = face_first_loops;
		polyhedra->types = types;
		polyhedra->face_counts = face_counts;
		polyhedra->first_faces = first_faces;
		this->polyhedra = polyhedra;
	}

	// Convert blobbies ...
	if(RHS.blobbies.size())
	{
		assert_not_implemented();
	}

	return *this;
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::primitive

mesh::primitive::primitive(const string_t& Type) :
	type(Type)
{
}

const bool_t mesh::primitive::almost_equal(const primitive& Other, const uint64_t Threshold) const
{
	return
		k3d::almost_equal<string_t>(Threshold)(type, Other.type) &&
		k3d::almost_equal<named_arrays_t>(Threshold)(topology, Other.topology) &&
		k3d::almost_equal<named_attribute_arrays_t>(Threshold)(attributes, Other.attributes);
}

////////////////////////////////////////////////////////////////////////////////////
// serialization

std::ostream& operator<<(std::ostream& Stream, const mesh::polyhedra_t::polyhedron_type& RHS)
{
	switch(RHS)
	{
		case mesh::polyhedra_t::POLYGONS:
			Stream << "polygons";
			break;
		case mesh::polyhedra_t::CATMULL_CLARK:
			Stream << "catmull_clark";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, mesh::polyhedra_t::polyhedron_type& RHS)
{
	string_t buffer;
	Stream >> buffer;

	if(buffer == "polygons")
		RHS = mesh::polyhedra_t::POLYGONS;
	else if(buffer == "catmull_clark")
		RHS = mesh::polyhedra_t::CATMULL_CLARK;
	else
		log() << error << "Unknown polyhedron type [" << buffer << "]" << std::endl;

	return Stream;
}

std::ostream& operator<<(std::ostream& Stream, const mesh::blobbies_t::primitive_type& RHS)
{
	switch(RHS)
	{
		case mesh::blobbies_t::CONSTANT:
			Stream << "constant";
			break;
		case mesh::blobbies_t::ELLIPSOID:
			Stream << "ellipsoid";
			break;
		case mesh::blobbies_t::SEGMENT:
			Stream << "segment";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, mesh::blobbies_t::primitive_type& RHS)
{
	string_t buffer;
	Stream >> buffer;

	if(buffer == "constant")
		RHS = mesh::blobbies_t::CONSTANT;
	else if(buffer == "ellipsoid")
		RHS = mesh::blobbies_t::ELLIPSOID;
	else if(buffer == "segment")
		RHS = mesh::blobbies_t::SEGMENT;
	else
		log() << error << "Unknown primitive type [" << buffer << "]" << std::endl;

	return Stream;
}

std::ostream& operator<<(std::ostream& Stream, const mesh::blobbies_t::operator_type& RHS)
{
	switch(RHS)
	{
		case mesh::blobbies_t::ADD:
			Stream << "add";
			break;
		case mesh::blobbies_t::MULTIPLY:
			Stream << "multiply";
			break;
		case mesh::blobbies_t::MAXIMUM:
			Stream << "maximum";
			break;
		case mesh::blobbies_t::MINIMUM:
			Stream << "minimum";
			break;
		case mesh::blobbies_t::DIVIDE:
			Stream << "divide";
			break;
		case mesh::blobbies_t::SUBTRACT:
			Stream << "subtract";
			break;
		case mesh::blobbies_t::NEGATE:
			Stream << "negate";
			break;
		case mesh::blobbies_t::IDENTITY:
			Stream << "identity";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, mesh::blobbies_t::operator_type& RHS)
{
	string_t buffer;
	Stream >> buffer;

	if(buffer == "add")
		RHS = mesh::blobbies_t::ADD;
	else if(buffer == "multiply")
		RHS = mesh::blobbies_t::MULTIPLY;
	else if(buffer == "maximum")
		RHS = mesh::blobbies_t::MAXIMUM;
	else if(buffer == "minimum")
		RHS = mesh::blobbies_t::MINIMUM;
	else if(buffer == "divide")
		RHS = mesh::blobbies_t::DIVIDE;
	else if(buffer == "subtract")
		RHS = mesh::blobbies_t::SUBTRACT;
	else if(buffer == "negate")
		RHS = mesh::blobbies_t::NEGATE;
	else if(buffer == "identity")
		RHS = mesh::blobbies_t::IDENTITY;
	else
		log() << error << "Unknown operator type [" << buffer << "]" << std::endl;

	return Stream;
}

/** \todo Print materials */
std::ostream& operator<<(std::ostream& Stream, const mesh& RHS)
{
	Stream << detail::indentation << "mesh:\n" << push_indent;

	if(RHS.point_groups)
	{
		Stream << detail::indentation << "point_groups:\n" << push_indent;

		detail::print(Stream, "first_points", RHS.point_groups->first_points);
		detail::print(Stream, "point_counts", RHS.point_groups->point_counts);
		detail::print(Stream, "constant_data", RHS.point_groups->constant_data);
		detail::print(Stream, "points", RHS.point_groups->points);
		detail::print(Stream, "varying_data", RHS.point_groups->varying_data);
		
		Stream << pop_indent;
	}

	if(RHS.linear_curve_groups)
	{
		Stream << detail::indentation << "linear_curve_groups:\n" << push_indent;

		detail::print(Stream, "first_curves", RHS.linear_curve_groups->first_curves);
		detail::print(Stream, "curve_counts", RHS.linear_curve_groups->curve_counts);
		detail::print(Stream, "periodic_curves", RHS.linear_curve_groups->periodic_curves);
		detail::print(Stream, "constant_data", RHS.linear_curve_groups->constant_data);
		detail::print(Stream, "curve_first points", RHS.linear_curve_groups->curve_first_points);
		detail::print(Stream, "curve_point counts", RHS.linear_curve_groups->curve_point_counts);
		detail::print(Stream, "curve_selection", RHS.linear_curve_groups->curve_selection);
		detail::print(Stream, "uniform_data", RHS.linear_curve_groups->uniform_data);
		detail::print(Stream, "curve_points", RHS.linear_curve_groups->curve_points);
		
		Stream << pop_indent;
	}

	if(RHS.cubic_curve_groups)
	{
		Stream << detail::indentation << "cubic_curve_groups:\n" << push_indent;

		detail::print(Stream, "first_curves", RHS.cubic_curve_groups->first_curves);
		detail::print(Stream, "curve_counts", RHS.cubic_curve_groups->curve_counts);
		detail::print(Stream, "periodic_curves", RHS.cubic_curve_groups->periodic_curves);
		detail::print(Stream, "constant_data", RHS.cubic_curve_groups->constant_data);
		detail::print(Stream, "curve_first_points", RHS.cubic_curve_groups->curve_first_points);
		detail::print(Stream, "curve_point_counts", RHS.cubic_curve_groups->curve_point_counts);
		detail::print(Stream, "curve_selection", RHS.cubic_curve_groups->curve_selection);
		detail::print(Stream, "uniform_data", RHS.cubic_curve_groups->uniform_data);
		detail::print(Stream, "curve_points", RHS.cubic_curve_groups->curve_points);
		
		Stream << pop_indent;
	}

	if(RHS.nurbs_curve_groups)
	{
		Stream << detail::indentation << "nurbs_curve_groups:\n" << push_indent;

		detail::print(Stream, "first_curves", RHS.nurbs_curve_groups->first_curves);
		detail::print(Stream, "curve_counts", RHS.nurbs_curve_groups->curve_counts);
		detail::print(Stream, "constant_data", RHS.nurbs_curve_groups->constant_data);
		detail::print(Stream, "curve_first_points", RHS.nurbs_curve_groups->curve_first_points);
		detail::print(Stream, "curve_point_counts", RHS.nurbs_curve_groups->curve_point_counts);
		detail::print(Stream, "curve_orders", RHS.nurbs_curve_groups->curve_orders);
		detail::print(Stream, "curve_selection", RHS.nurbs_curve_groups->curve_selection);
		detail::print(Stream, "uniform_data", RHS.nurbs_curve_groups->uniform_data);
		detail::print(Stream, "curve_points", RHS.nurbs_curve_groups->curve_points);
		detail::print(Stream, "curve_point_weights", RHS.nurbs_curve_groups->curve_point_weights);
		detail::print(Stream, "curve_knots", RHS.nurbs_curve_groups->curve_knots);
		
		Stream << pop_indent;
	}

	if(RHS.bilinear_patches)
	{
		Stream << detail::indentation << "bilinear_patches:\n" << push_indent;

		detail::print(Stream, "patch_selection", RHS.bilinear_patches->patch_selection);
		detail::print(Stream, "constant_data", RHS.bilinear_patches->constant_data);
		detail::print(Stream, "uniform_data", RHS.bilinear_patches->uniform_data);
		detail::print(Stream, "patch_points", RHS.bilinear_patches->patch_points);
		detail::print(Stream, "varying_data", RHS.bilinear_patches->varying_data);
		
		Stream << pop_indent;
	}

	if(RHS.bicubic_patches)
	{
		Stream << detail::indentation << "bicubic_patches:\n" << push_indent;

		detail::print(Stream, "patch_selection", RHS.bicubic_patches->patch_selection);
		detail::print(Stream, "constant_data", RHS.bicubic_patches->constant_data);
		detail::print(Stream, "uniform_data", RHS.bicubic_patches->uniform_data);
		detail::print(Stream, "patch_points", RHS.bicubic_patches->patch_points);
		detail::print(Stream, "varying_data", RHS.bicubic_patches->varying_data);
		
		Stream << pop_indent;
	}

	if(RHS.nurbs_patches)
	{
		Stream << detail::indentation << "nurbs_patches:\n" << push_indent;

		detail::print(Stream, "patch_first_points", RHS.nurbs_patches->patch_first_points);
		detail::print(Stream, "patch_u_point_counts", RHS.nurbs_patches->patch_u_point_counts);
		detail::print(Stream, "patch_v_point_counts", RHS.nurbs_patches->patch_v_point_counts);
		detail::print(Stream, "patch_u_orders", RHS.nurbs_patches->patch_u_orders);
		detail::print(Stream, "patch_v_orders", RHS.nurbs_patches->patch_v_orders);
		detail::print(Stream, "patch_selection", RHS.nurbs_patches->patch_selection);
		detail::print(Stream, "constant_data", RHS.nurbs_patches->constant_data);
		detail::print(Stream, "uniform_data", RHS.nurbs_patches->uniform_data);
		detail::print(Stream, "patch_points", RHS.nurbs_patches->patch_points);
		detail::print(Stream, "patch_point_weights", RHS.nurbs_patches->patch_point_weights);
		detail::print(Stream, "patch_u_knots", RHS.nurbs_patches->patch_u_knots);
		detail::print(Stream, "patch_v_knots", RHS.nurbs_patches->patch_v_knots);
		detail::print(Stream, "varying_data", RHS.nurbs_patches->varying_data);
		detail::print(Stream, "patch_trim_curve_loop_counts", RHS.nurbs_patches->patch_trim_curve_loop_counts);
		detail::print(Stream, "patch_first_trim_curve_loops", RHS.nurbs_patches->patch_first_trim_curve_loops);
		detail::print(Stream, "trim_points", RHS.nurbs_patches->trim_points);
		detail::print(Stream, "trim_point_selection", RHS.nurbs_patches->trim_point_selection);
		detail::print(Stream, "first_trim_curves", RHS.nurbs_patches->first_trim_curves);
		detail::print(Stream, "trim_curve_counts", RHS.nurbs_patches->trim_curve_counts);
		detail::print(Stream, "trim_curve_loop_selection", RHS.nurbs_patches->trim_curve_loop_selection);
		detail::print(Stream, "trim_curve_first_points", RHS.nurbs_patches->trim_curve_first_points);
		detail::print(Stream, "trim_curve_point_counts", RHS.nurbs_patches->trim_curve_point_counts);
		detail::print(Stream, "trim_curve_orders", RHS.nurbs_patches->trim_curve_orders);
		detail::print(Stream, "trim_curve_first_knots", RHS.nurbs_patches->trim_curve_first_knots);
		detail::print(Stream, "trim_curve_selection", RHS.nurbs_patches->trim_curve_selection);
		detail::print(Stream, "trim_curve_points", RHS.nurbs_patches->trim_curve_points);
		detail::print(Stream, "trim_curve_point_weights", RHS.nurbs_patches->trim_curve_point_weights);
		detail::print(Stream, "trim_curve_knots", RHS.nurbs_patches->trim_curve_knots);
		
		Stream << pop_indent;
	}

	if(RHS.polyhedra)
	{
		Stream << detail::indentation << "polyhedra:\n" << push_indent;

		detail::print(Stream, "first_faces", RHS.polyhedra->first_faces);
		detail::print(Stream, "face_counts", RHS.polyhedra->face_counts);
		detail::print(Stream, "types", RHS.polyhedra->types);
		detail::print(Stream, "constant_data", RHS.polyhedra->constant_data);
		detail::print(Stream, "face_first_loops", RHS.polyhedra->face_first_loops);
		detail::print(Stream, "face_loop_counts", RHS.polyhedra->face_loop_counts);
		detail::print(Stream, "face_selection", RHS.polyhedra->face_selection);
		detail::print(Stream, "uniform_data", RHS.polyhedra->uniform_data);
		detail::print(Stream, "loop_first_edges", RHS.polyhedra->loop_first_edges);
		detail::print(Stream, "edge_points", RHS.polyhedra->edge_points);
		detail::print(Stream, "clockwise_edges", RHS.polyhedra->clockwise_edges);
		detail::print(Stream, "edge_selection", RHS.polyhedra->edge_selection);
		detail::print(Stream, "face_varying_data", RHS.polyhedra->face_varying_data);
		
		Stream << pop_indent;
	}

	if(RHS.blobbies)
	{
		Stream << detail::indentation << "blobbies:\n" << push_indent;

		detail::print(Stream, "first_primitives", RHS.blobbies->first_primitives);
		detail::print(Stream, "primitive_counts", RHS.blobbies->primitive_counts);
		detail::print(Stream, "first_operators", RHS.blobbies->first_operators);
		detail::print(Stream, "operator_counts", RHS.blobbies->operator_counts);
		detail::print(Stream, "constant_data", RHS.blobbies->constant_data);
		detail::print(Stream, "uniform_data", RHS.blobbies->uniform_data);
		detail::print(Stream, "primitives", RHS.blobbies->primitives);
		detail::print(Stream, "primitive_first_floats", RHS.blobbies->primitive_first_floats);
		detail::print(Stream, "primitive_float_counts", RHS.blobbies->primitive_float_counts);
		detail::print(Stream, "varying_data", RHS.blobbies->varying_data);
		detail::print(Stream, "vertex_data", RHS.blobbies->vertex_data);
		detail::print(Stream, "operators", RHS.blobbies->operators);
		detail::print(Stream, "operator_first_operands", RHS.blobbies->operator_first_operands);
		detail::print(Stream, "operator_operand_counts", RHS.blobbies->operator_operand_counts);
		detail::print(Stream, "floats", RHS.blobbies->floats);
		detail::print(Stream, "operands", RHS.blobbies->operands);
		
		Stream << pop_indent;
	}

	detail::print(Stream, "points", RHS.points);
	detail::print(Stream, "point_selection", RHS.point_selection);
	detail::print(Stream, "vertex_data", RHS.vertex_data);

	Stream << detail::indentation << "primitives (" << RHS.primitives.size() << "):\n" << push_indent;
	for(mesh::primitives_t::const_iterator primitive = RHS.primitives.begin(); primitive != RHS.primitives.end(); ++primitive)
	{
		Stream << detail::indentation << "primitive \"" << (*primitive)->type << "\"\n" << push_indent;
		detail::print(Stream, "topology", (*primitive)->topology);
		detail::print(Stream, "attributes", (*primitive)->attributes);
		Stream << pop_indent;
	}
	Stream << pop_indent;
	Stream << pop_indent;

	return Stream;
}

} // namespace k3d

