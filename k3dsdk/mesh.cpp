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

#include "array_metadata.h"
#include "color.h"
#include "imaterial.h"
#include "iomanip.h"
#include "legacy_mesh.h"
#include "mesh.h"
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
		// Special-case k3d::uint_t_array so the type is labeled correctly ...
		if(const uint_t_array* const array = dynamic_cast<const uint_t_array*>(&m_array))
		{
			m_printed = true;
			m_stream << indentation << "array \"" << m_array_name << "\" [k3d::uint_t] (" << m_array.size() << "): ";
			std::copy(array->begin(), array->end(), std::ostream_iterator<uint_t>(m_stream, " "));
			m_stream << "\n";

			print_metadata();
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

			print_metadata();
		}
	}

	/// Special-case printing of 8-bit integers so they aren't printed as characters
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

			print_metadata();
		}
	}

	/// Special-case printing of 8-bit unsigned integers so they aren't printed as characters
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

			print_metadata();
		}
	}

	/// Special-case printing of strings so whitespace is handled correctly
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

			print_metadata();
		}
	}

private:
	void print_metadata()
	{
		const array::metadata_t metadata = m_array.get_metadata();
		for(array::metadata_t::const_iterator pair = metadata.begin(); pair != metadata.end(); ++pair)
			m_stream << push_indent << indentation << "metadata: " << pair->first << " = " << pair->second << "\n" << pop_indent;
	}

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
const bool_t almost_equal(const pipeline_data<typed_array<T> >& A, const pipeline_data<typed_array<T> >& B, const uint64_t Threshold)
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
const bool_t almost_equal(const pipeline_data<uint_t_array>& A, const pipeline_data<uint_t_array>& B, const uint64_t Threshold)
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
const bool_t almost_equal(const pipeline_data<T>& A, const pipeline_data<T>& B, const uint64_t Threshold)
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
			if(!(**a).almost_equal((**b), Threshold))
				return false;
		}
		// One array was NULL and the other wasn't
		else if(a->get() || b->get())
		{
			return false;
		}
	}

	return true;
}


} // namespace detail

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

		detail::almost_equal(nurbs_curve_groups, Other.nurbs_curve_groups, Threshold) &&
		detail::almost_equal(nurbs_patches, Other.nurbs_patches, Threshold) &&
		detail::almost_equal(polyhedra, Other.polyhedra, Threshold);
}

mesh& mesh::operator=(const legacy::mesh& RHS)
{
	// Convert points ...
	std::map<legacy::point*, uint_t> point_map;

	const uint_t point_size = RHS.points.size();
	points_t& points = this->points.create(new points_t(point_size));
	selection_t& point_selection = this->point_selection.create(new selection_t(point_size));

	for(uint_t i = 0; i != point_size; ++i)
	{
		points[i] = RHS.points[i]->position;
		point_selection[i] = RHS.points[i]->selection_weight;
		point_map[RHS.points[i]] = i;
	}

	// Convert primitives ...
	primitives = RHS.primitives;

	// Convert NURBS curves ...
	if(RHS.nucurve_groups.size())
	{
		nurbs_curve_groups_t& nurbs_curve_groups = this->nurbs_curve_groups.create();
		indices_t& first_curves = nurbs_curve_groups.first_curves.create();
		counts_t& curve_counts = nurbs_curve_groups.curve_counts.create();
		materials_t& materials = nurbs_curve_groups.materials.create();
		indices_t& curve_first_points = nurbs_curve_groups.curve_first_points.create();
		counts_t& curve_point_counts = nurbs_curve_groups.curve_point_counts.create();
		orders_t& curve_orders = nurbs_curve_groups.curve_orders.create();
		indices_t& curve_first_knots = nurbs_curve_groups.curve_first_knots.create();
		selection_t& curve_selection = nurbs_curve_groups.curve_selection.create();
		indices_t& curve_points = nurbs_curve_groups.curve_points.create();
		weights_t& curve_point_weights = nurbs_curve_groups.curve_point_weights.create();
		knots_t& curve_knots = nurbs_curve_groups.curve_knots.create();

		for(legacy::mesh::nucurve_groups_t::const_iterator group = RHS.nucurve_groups.begin(); group != RHS.nucurve_groups.end(); ++group)
		{
			first_curves.push_back(curve_first_points.size());
			curve_counts.push_back((*group)->curves.size());
			materials.push_back((*group)->material);

			for(legacy::nucurve_group::curves_t::const_iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			{
				curve_first_points.push_back(curve_points.size());
				curve_point_counts.push_back((*curve)->knots.size() - (*curve)->order);
				curve_orders.push_back((*curve)->order);
				curve_first_knots.push_back(curve_knots.size());
				curve_selection.push_back((*curve)->selection_weight);

				for(legacy::nucurve::control_points_t::const_iterator point = (*curve)->control_points.begin(); point != (*curve)->control_points.end(); ++point)
				{
					curve_points.push_back(point_map[point->position]);
					curve_point_weights.push_back(point->weight);
				}

				curve_knots.insert(curve_knots.end(), (*curve)->knots.begin(), (*curve)->knots.end());
			}
		}
	}

	// Convert NURBS patches ...
	if(RHS.nupatches.size())
	{
		nurbs_patches_t& nurbs_patches = this->nurbs_patches.create();
		indices_t& patch_first_points = nurbs_patches.patch_first_points.create();
		counts_t& patch_u_point_counts = nurbs_patches.patch_u_point_counts.create();
		counts_t& patch_v_point_counts = nurbs_patches.patch_v_point_counts.create();
		orders_t& patch_u_orders = nurbs_patches.patch_u_orders.create();
		orders_t& patch_v_orders = nurbs_patches.patch_v_orders.create();
		indices_t& patch_u_first_knots = nurbs_patches.patch_u_first_knots.create();
		indices_t& patch_v_first_knots = nurbs_patches.patch_v_first_knots.create();
		selection_t& patch_selection = nurbs_patches.patch_selection.create();
		materials_t& patch_materials = nurbs_patches.patch_materials.create();
		indices_t& patch_points = nurbs_patches.patch_points.create();
		weights_t& patch_point_weights = nurbs_patches.patch_point_weights.create();
		knots_t& patch_u_knots = nurbs_patches.patch_u_knots.create();
		knots_t& patch_v_knots = nurbs_patches.patch_v_knots.create();

		for(legacy::mesh::nupatches_t::const_iterator patch = RHS.nupatches.begin(); patch != RHS.nupatches.end(); ++patch)
		{
			patch_first_points.push_back(patch_points.size());
			patch_u_point_counts.push_back((*patch)->u_knots.size() - (*patch)->u_order);
			patch_v_point_counts.push_back((*patch)->v_knots.size() - (*patch)->v_order);
			patch_u_orders.push_back((*patch)->u_order);
			patch_v_orders.push_back((*patch)->v_order);
			patch_u_first_knots.push_back(patch_u_knots.size());
			patch_v_first_knots.push_back(patch_v_knots.size());
			patch_selection.push_back((*patch)->selection_weight);
			patch_materials.push_back((*patch)->material);

			for(legacy::nupatch::control_points_t::const_iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
			{
				patch_points.push_back(point_map[point->position]);
				patch_point_weights.push_back(point->weight);
			}

			patch_u_knots.insert(patch_u_knots.end(), (*patch)->u_knots.begin(), (*patch)->u_knots.end());
			patch_v_knots.insert(patch_v_knots.end(), (*patch)->v_knots.begin(), (*patch)->v_knots.end());
		}
	}

	// Convert polyhedra ...
	if(RHS.polyhedra.size())
	{
		polyhedra_t& polyhedra = this->polyhedra.create();
		indices_t& first_faces = polyhedra.first_faces.create();
		counts_t& face_counts = polyhedra.face_counts.create();
		polyhedra_t::types_t& types = polyhedra.types.create();
		indices_t& face_first_loops = polyhedra.face_first_loops.create();
		counts_t& face_loop_counts = polyhedra.face_loop_counts.create();
		selection_t& face_selection = polyhedra.face_selection.create();
		materials_t& face_materials = polyhedra.face_materials.create();
		indices_t& loop_first_edges = polyhedra.loop_first_edges.create();
		indices_t& edge_points = polyhedra.edge_points.create();
		indices_t& clockwise_edges = polyhedra.clockwise_edges.create();
		selection_t& edge_selection = polyhedra.edge_selection.create();

		for(legacy::mesh::polyhedra_t::const_iterator polyhedron = RHS.polyhedra.begin(); polyhedron != RHS.polyhedra.end(); ++polyhedron)
		{
			uint_t first_face = face_first_loops.size();
			uint_t face_count = 0;
			mesh::polyhedra_t::polyhedron_type type = (*polyhedron)->type == legacy::polyhedron::POLYGONS ? mesh::polyhedra_t::POLYGONS : mesh::polyhedra_t::CATMULL_CLARK;

			for(legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				++face_count;

				uint_t face_first_loop = loop_first_edges.size();
				uint_t face_loop_count = 1 + (*face)->holes.size();

				const uint_t first_edge = edge_points.size();

				loop_first_edges.push_back(first_edge);
				for(legacy::split_edge* edge = (*face)->first_edge; edge; edge = edge->face_clockwise)
				{
					if(edge->vertex && edge->face_clockwise)
					{
						edge_points.push_back(point_map[edge->vertex]);
						clockwise_edges.push_back(edge_points.size());
						edge_selection.push_back(edge->selection_weight);
					}

					if(edge->face_clockwise == (*face)->first_edge)
					{
						clockwise_edges.back() = first_edge;
						break;
					}
				}

				for(legacy::face::holes_t::iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
				{
					const uint_t first_edge = edge_points.size();

					loop_first_edges.push_back(first_edge);
					for(legacy::split_edge* edge = *hole; edge; edge = edge->face_clockwise)
					{
						if(edge->vertex && edge->face_clockwise && edge->face_clockwise->vertex)
						{
							edge_points.push_back(point_map[edge->vertex]);
							clockwise_edges.push_back(edge_points.size());
							edge_selection.push_back(edge->selection_weight);
						}

						if(edge->face_clockwise == (*hole))
						{
							clockwise_edges.back() = first_edge;
							break;
						}
					}
				}

				face_first_loops.push_back(face_first_loop);
				face_loop_counts.push_back(face_loop_count);
				face_selection.push_back((*face)->selection_weight);
				face_materials.push_back((*face)->material);
			}

			first_faces.push_back(first_face);
			face_counts.push_back(face_count);
			types.push_back(type);
		}
	}

	return *this;
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

	void operator()(const string_t&, const pipeline_data<array>& Array)
	{
		if(Array->get_metadata_value(k3d::mesh_point_indices()) != "true")
			return;

		if(const mesh::indices_t* const array = dynamic_cast<const mesh::indices_t*>(Array.get()))
			mark_used_points(*array, unused_points);
	}

	mesh::bools_t& unused_points;
};

} // namespace detail

void mesh::lookup_unused_points(const mesh& Mesh, mesh::bools_t& UnusedPoints)
{
	UnusedPoints.assign(Mesh.points ? Mesh.points->size() : 0, true);

	// Mark points used by legacy primitives ...
	if(Mesh.nurbs_curve_groups && Mesh.nurbs_curve_groups->curve_points)
		detail::mark_used_points(*Mesh.nurbs_curve_groups->curve_points, UnusedPoints);

	if(Mesh.nurbs_patches && Mesh.nurbs_patches->patch_points)
		detail::mark_used_points(*Mesh.nurbs_patches->patch_points, UnusedPoints);

	if(Mesh.polyhedra && Mesh.polyhedra->edge_points)
		detail::mark_used_points(*Mesh.polyhedra->edge_points, UnusedPoints);

	// Mark points used by generic mesh primtiives ...
	visit_arrays(Mesh, detail::mark_used_primitive_points(UnusedPoints));
}

namespace detail
{

/// Helper function used by delete_unused_points()
void remap_points(mesh::indices_t& PrimitivePoints, const mesh::indices_t& PointMap)
{
	const uint_t begin = 0;
	const uint_t end = PrimitivePoints.size();
	for(uint_t i = begin; i != end; ++i)
		PrimitivePoints[i] = PointMap[PrimitivePoints[i]];
}

/// Helper object used by delete_unused_points()
struct remap_primitive_points
{
	remap_primitive_points(mesh::indices_t& PointMap) :
		point_map(PointMap)
	{
	}

	void operator()(const string_t&, pipeline_data<array>& Array)
	{
		if(Array->get_metadata_value(k3d::mesh_point_indices()) != "true")
			return;

		if(mesh::indices_t* const array = dynamic_cast<mesh::indices_t*>(&Array.writable()))
			remap_points(*array, point_map);
	}

	mesh::indices_t& point_map;
};

} // namespace detail

void mesh::delete_unused_points(mesh& Mesh)
{
	// Create a bitmap marking which points are unused ...
	mesh::bools_t unused_points;
	lookup_unused_points(Mesh, unused_points);

	// Count how many points will be left when we're done ...
	const uint_t points_remaining = std::count(unused_points.begin(), unused_points.end(), false);

	// Create an array that will map from current-point-indices to new-point-indices,
	// taking into account the points that will be removed.
	mesh::indices_t point_map(unused_points.size());

	const uint_t begin = 0;
	const uint_t end = unused_points.size();
	for(uint_t current_index = begin, new_index = begin; current_index != end; ++current_index)
	{
		point_map[current_index] = new_index;
		if(!unused_points[current_index])
			++new_index;
	}

	// Move leftover points (and point selections) into their final positions ...
	mesh::points_t& points = Mesh.points.writable();
	mesh::selection_t& point_selection = Mesh.point_selection.writable();
	for(uint_t i = begin; i != end; ++i)
	{
		if(!unused_points[i])
		{
			points[point_map[i]] = points[i];
			point_selection[point_map[i]] = point_selection[i];
		}
	}

	// Update legacy mesh primitives so they use the correct indices ...
	if(Mesh.nurbs_curve_groups && Mesh.nurbs_curve_groups->curve_points)
		detail::remap_points(Mesh.nurbs_curve_groups.writable().curve_points.writable(), point_map);

	if(Mesh.nurbs_patches && Mesh.nurbs_patches->patch_points)
		detail::remap_points(Mesh.nurbs_patches.writable().patch_points.writable(), point_map);

	if(Mesh.polyhedra && Mesh.polyhedra->edge_points)
		detail::remap_points(Mesh.polyhedra.writable().edge_points.writable(), point_map);

	// Update generic mesh primitives so they use the correct indices ...
	visit_arrays(Mesh, detail::remap_primitive_points(point_map));

	// Free leftover memory ...
	points.resize(points_remaining);
	point_selection.resize(points_remaining);
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
// mesh::primitives_t

mesh::primitive& mesh::primitives_t::create(const string_t& Type)
{
	push_back(pipeline_data<mesh::primitive>());
	return back().create(new mesh::primitive(Type));
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

/*
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
*/

/** \todo Print materials */
std::ostream& operator<<(std::ostream& Stream, const mesh& RHS)
{
	Stream << detail::indentation << "mesh:\n" << push_indent;

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

