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
#include "imaterial.h"
#include "mesh_diff.h"
#include "mesh.h"

#include <boost/format.hpp>

#include <sstream>

namespace k3d
{

namespace diff
{

namespace detail
{

template<typename pointer_type>
void print(std::ostream& Stream, const std::string& Label, const pointer_type& A, const pointer_type& B)
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
void print(std::ostream& Stream, const std::string& Label, const pointer_type& A, const pointer_type& B, const boost::uint64_t Threshold)
{
	if(!A && !B)
		return;

	const size_t a_size = A ? A->size() : 0;
	const size_t b_size = B ? B->size() : 0;

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

	k3d::almost_equal almost_equal(Threshold);

	for(size_t i = 0; i < a_size || i < b_size; ++i)
	{
		const std::string difference_buffer = (A && i < a_size && B && i < b_size && almost_equal(A->at(i), B->at(i))) ? std::string("") : std::string("*****");

		std::ostringstream a_buffer;
		if(A && i < a_size)
			a_buffer << A->at(i);

		std::ostringstream b_buffer;
		if(B && i < b_size)
			b_buffer << B->at(i);

		Stream << format % difference_buffer % a_buffer.str() % b_buffer.str();
	}

	Stream << "\n";
}

/** \todo Implement comparisons for user arrays */
void print(std::ostream& Stream, const std::string& Label, const k3d::mesh::named_arrays& A, const k3d::mesh::named_arrays& B, const boost::uint64_t Threshold)
{
/*
    for(k3d::mesh::named_arrays::const_iterator array_iterator = Arrays.begin(); array_iterator != Arrays.end(); ++array_iterator)
    {
        Stream << Label << " " << array_iterator->first << " (" << array_iterator->second->size() << "): ";
        if(typed_array<double>* const array = dynamic_cast<typed_array<double>*>(array_iterator->second.get()))
            std::copy(array->begin(), array->end(), std::ostream_iterator<double>(Stream, " "));
        else if(typed_array<k3d::color>* const array = dynamic_cast<typed_array<k3d::color>*>(array_iterator->second.get()))
            std::copy(array->begin(), array->end(), std::ostream_iterator<k3d::color>(Stream, " "));
        else
            Stream << "<unknown type>" << std::endl;
        Stream << "\n";
    }
*/
}

} // namespace detail

void print(std::ostream& Stream, const mesh& A, const mesh& B, const boost::uint64_t Threshold)
{
	detail::print(Stream, "point groups", A.point_groups, B.point_groups);
	if(A.point_groups && B.point_groups)
	{
		detail::print(Stream, "first points", A.point_groups->first_points, B.point_groups->first_points, Threshold);
		detail::print(Stream, "point counts", A.point_groups->point_counts, B.point_groups->point_counts, Threshold);
		detail::print(Stream, "materials", A.point_groups->materials, B.point_groups->materials, Threshold);
		detail::print(Stream, "constant data", A.point_groups->constant_data, B.point_groups->constant_data, Threshold);
		detail::print(Stream, "points", A.point_groups->points, B.point_groups->points, Threshold);
		detail::print(Stream, "varying data", A.point_groups->varying_data, B.point_groups->varying_data, Threshold);
	}

	detail::print(Stream, "linear curve groups", A.linear_curve_groups, B.linear_curve_groups);
	if(A.linear_curve_groups && B.linear_curve_groups)
	{
		detail::print(Stream, "first curves", A.linear_curve_groups->first_curves, B.linear_curve_groups->first_curves, Threshold);
		detail::print(Stream, "curve counts", A.linear_curve_groups->curve_counts, B.linear_curve_groups->curve_counts, Threshold);
		detail::print(Stream, "periodic curves", A.linear_curve_groups->periodic_curves, B.linear_curve_groups->periodic_curves, Threshold);
		detail::print(Stream, "materials", A.linear_curve_groups->materials, B.linear_curve_groups->materials, Threshold);
		detail::print(Stream, "constant data", A.linear_curve_groups->constant_data, B.linear_curve_groups->constant_data, Threshold);
		detail::print(Stream, "curve first points", A.linear_curve_groups->curve_first_points, B.linear_curve_groups->curve_first_points, Threshold);
		detail::print(Stream, "curve point counts", A.linear_curve_groups->curve_point_counts, B.linear_curve_groups->curve_point_counts, Threshold);
		detail::print(Stream, "curve selection", A.linear_curve_groups->curve_selection, B.linear_curve_groups->curve_selection, Threshold);
		detail::print(Stream, "uniform data", A.linear_curve_groups->uniform_data, B.linear_curve_groups->uniform_data, Threshold);
		detail::print(Stream, "curve points", A.linear_curve_groups->curve_points, B.linear_curve_groups->curve_points, Threshold);
	}

	detail::print(Stream, "cubic curve groups", A.cubic_curve_groups, B.cubic_curve_groups);
	if(A.cubic_curve_groups && B.cubic_curve_groups)
	{
		detail::print(Stream, "first curves", A.cubic_curve_groups->first_curves, B.cubic_curve_groups->first_curves, Threshold);
		detail::print(Stream, "curve counts", A.cubic_curve_groups->curve_counts, B.cubic_curve_groups->curve_counts, Threshold);
		detail::print(Stream, "periodic curves", A.cubic_curve_groups->periodic_curves, B.cubic_curve_groups->periodic_curves, Threshold);
		detail::print(Stream, "materials", A.cubic_curve_groups->materials, B.cubic_curve_groups->materials, Threshold);
		detail::print(Stream, "constant data", A.cubic_curve_groups->constant_data, B.cubic_curve_groups->constant_data, Threshold);
		detail::print(Stream, "curve first points", A.cubic_curve_groups->curve_first_points, B.cubic_curve_groups->curve_first_points, Threshold);
		detail::print(Stream, "curve point counts", A.cubic_curve_groups->curve_point_counts, B.cubic_curve_groups->curve_point_counts, Threshold);
		detail::print(Stream, "curve selection", A.cubic_curve_groups->curve_selection, B.cubic_curve_groups->curve_selection, Threshold);
		detail::print(Stream, "uniform data", A.cubic_curve_groups->uniform_data, B.cubic_curve_groups->uniform_data, Threshold);
		detail::print(Stream, "curve points", A.cubic_curve_groups->curve_points, B.cubic_curve_groups->curve_points, Threshold);
	}

	detail::print(Stream, "nurbs curve groups", A.nurbs_curve_groups, B.nurbs_curve_groups);
	if(A.nurbs_curve_groups && B.nurbs_curve_groups)
	{
		detail::print(Stream, "first curves", A.nurbs_curve_groups->first_curves, B.nurbs_curve_groups->first_curves, Threshold);
		detail::print(Stream, "curve counts", A.nurbs_curve_groups->curve_counts, B.nurbs_curve_groups->curve_counts, Threshold);
		detail::print(Stream, "materials", A.nurbs_curve_groups->materials, B.nurbs_curve_groups->materials, Threshold);
		detail::print(Stream, "constant data", A.nurbs_curve_groups->constant_data, B.nurbs_curve_groups->constant_data, Threshold);
		detail::print(Stream, "curve first points", A.nurbs_curve_groups->curve_first_points, B.nurbs_curve_groups->curve_first_points, Threshold);
		detail::print(Stream, "curve point counts", A.nurbs_curve_groups->curve_point_counts, B.nurbs_curve_groups->curve_point_counts, Threshold);
		detail::print(Stream, "curve orders", A.nurbs_curve_groups->curve_orders, B.nurbs_curve_groups->curve_orders, Threshold);
		detail::print(Stream, "curve selection", A.nurbs_curve_groups->curve_selection, B.nurbs_curve_groups->curve_selection, Threshold);
		detail::print(Stream, "uniform data", A.nurbs_curve_groups->uniform_data, B.nurbs_curve_groups->uniform_data, Threshold);
		detail::print(Stream, "curve points", A.nurbs_curve_groups->curve_points, B.nurbs_curve_groups->curve_points, Threshold);
		detail::print(Stream, "curve point weights", A.nurbs_curve_groups->curve_point_weights, B.nurbs_curve_groups->curve_point_weights, Threshold);
		detail::print(Stream, "curve knots", A.nurbs_curve_groups->curve_knots, B.nurbs_curve_groups->curve_knots, Threshold);
	}

	detail::print(Stream, "bilinear patches", A.bilinear_patches, B.bilinear_patches);
	if(A.bilinear_patches && B.bilinear_patches)
	{
		detail::print(Stream, "patch selection", A.bilinear_patches->patch_selection, B.bilinear_patches->patch_selection, Threshold);
		detail::print(Stream, "patch materials", A.bilinear_patches->patch_materials, B.bilinear_patches->patch_materials, Threshold);
		detail::print(Stream, "constant data", A.bilinear_patches->constant_data, B.bilinear_patches->constant_data, Threshold);
		detail::print(Stream, "uniform data", A.bilinear_patches->uniform_data, B.bilinear_patches->uniform_data, Threshold);
		detail::print(Stream, "patch points", A.bilinear_patches->patch_points, B.bilinear_patches->patch_points, Threshold);
		detail::print(Stream, "varying data", A.bilinear_patches->varying_data, B.bilinear_patches->varying_data, Threshold);
	}

	detail::print(Stream, "bicubic patches", A.bicubic_patches, B.bicubic_patches);
	if(A.bicubic_patches && B.bicubic_patches)
	{
		detail::print(Stream, "patch selection", A.bicubic_patches->patch_selection, B.bicubic_patches->patch_selection, Threshold);
		detail::print(Stream, "patch materials", A.bicubic_patches->patch_materials, B.bicubic_patches->patch_materials, Threshold);
		detail::print(Stream, "constant data", A.bicubic_patches->constant_data, B.bicubic_patches->constant_data, Threshold);
		detail::print(Stream, "uniform data", A.bicubic_patches->uniform_data, B.bicubic_patches->uniform_data, Threshold);
		detail::print(Stream, "patch points", A.bicubic_patches->patch_points, B.bicubic_patches->patch_points, Threshold);
		detail::print(Stream, "varying data", A.bicubic_patches->varying_data, B.bicubic_patches->varying_data, Threshold);
	}

	detail::print(Stream, "nurbs patches", A.nurbs_patches, B.nurbs_patches);
	if(A.nurbs_patches && B.nurbs_patches)
	{
		detail::print(Stream, "patch first points", A.nurbs_patches->patch_first_points, B.nurbs_patches->patch_first_points, Threshold);
		detail::print(Stream, "patch u point counts", A.nurbs_patches->patch_u_point_counts, B.nurbs_patches->patch_u_point_counts, Threshold);
		detail::print(Stream, "patch v point counts", A.nurbs_patches->patch_v_point_counts, B.nurbs_patches->patch_v_point_counts, Threshold);
		detail::print(Stream, "patch u orders", A.nurbs_patches->patch_u_orders, B.nurbs_patches->patch_u_orders, Threshold);
		detail::print(Stream, "patch v orders", A.nurbs_patches->patch_v_orders, B.nurbs_patches->patch_v_orders, Threshold);
		detail::print(Stream, "patch selection", A.nurbs_patches->patch_selection, B.nurbs_patches->patch_selection, Threshold);
		detail::print(Stream, "patch materials", A.nurbs_patches->patch_materials, B.nurbs_patches->patch_materials, Threshold);
		detail::print(Stream, "constant data", A.nurbs_patches->constant_data, B.nurbs_patches->constant_data, Threshold);
		detail::print(Stream, "uniform data", A.nurbs_patches->uniform_data, B.nurbs_patches->uniform_data, Threshold);
		detail::print(Stream, "patch points", A.nurbs_patches->patch_points, B.nurbs_patches->patch_points, Threshold);
		detail::print(Stream, "patch point weights", A.nurbs_patches->patch_point_weights, B.nurbs_patches->patch_point_weights, Threshold);
		detail::print(Stream, "patch u knots", A.nurbs_patches->patch_u_knots, B.nurbs_patches->patch_u_knots, Threshold);
		detail::print(Stream, "patch v knots", A.nurbs_patches->patch_v_knots, B.nurbs_patches->patch_v_knots, Threshold);
		detail::print(Stream, "varying data", A.nurbs_patches->varying_data, B.nurbs_patches->varying_data, Threshold);
	}

	detail::print(Stream, "polyhedra", A.polyhedra, B.polyhedra);
	if(A.polyhedra && B.polyhedra)
	{
		detail::print(Stream, "first faces", A.polyhedra->first_faces, B.polyhedra->first_faces, Threshold);
		detail::print(Stream, "face counts", A.polyhedra->face_counts, B.polyhedra->face_counts, Threshold);
		detail::print(Stream, "types", A.polyhedra->types, B.polyhedra->types, Threshold);
		detail::print(Stream, "constant data", A.polyhedra->constant_data, B.polyhedra->constant_data, Threshold);
		detail::print(Stream, "face first loops", A.polyhedra->face_first_loops, B.polyhedra->face_first_loops, Threshold);
		detail::print(Stream, "face loop counts", A.polyhedra->face_loop_counts, B.polyhedra->face_loop_counts, Threshold);
		detail::print(Stream, "face selection", A.polyhedra->face_selection, B.polyhedra->face_selection, Threshold);
		detail::print(Stream, "face materials", A.polyhedra->face_materials, B.polyhedra->face_materials, Threshold);
		detail::print(Stream, "uniform data", A.polyhedra->uniform_data, B.polyhedra->uniform_data, Threshold);
		detail::print(Stream, "loop first edges", A.polyhedra->loop_first_edges, B.polyhedra->loop_first_edges, Threshold);
		detail::print(Stream, "edge points", A.polyhedra->edge_points, B.polyhedra->edge_points, Threshold);
		detail::print(Stream, "clockwise edges", A.polyhedra->clockwise_edges, B.polyhedra->clockwise_edges, Threshold);
		detail::print(Stream, "edge selection", A.polyhedra->edge_selection, B.polyhedra->edge_selection, Threshold);
		detail::print(Stream, "face varying data", A.polyhedra->face_varying_data, B.polyhedra->face_varying_data, Threshold);
	}

	detail::print(Stream, "blobbies", A.blobbies, B.blobbies);
	if(A.blobbies && B.blobbies)
	{
		detail::print(Stream, "first primitives", A.blobbies->first_primitives, B.blobbies->first_primitives, Threshold);
		detail::print(Stream, "primitive counts", A.blobbies->primitive_counts, B.blobbies->primitive_counts, Threshold);
		detail::print(Stream, "first operators", A.blobbies->first_operators, B.blobbies->first_operators, Threshold);
		detail::print(Stream, "operator counts", A.blobbies->operator_counts, B.blobbies->operator_counts, Threshold);
		detail::print(Stream, "materials", A.blobbies->materials, B.blobbies->materials, Threshold);
		detail::print(Stream, "constant data", A.blobbies->constant_data, B.blobbies->constant_data, Threshold);
		detail::print(Stream, "uniform data", A.blobbies->uniform_data, B.blobbies->uniform_data, Threshold);
		detail::print(Stream, "primitives", A.blobbies->primitives, B.blobbies->primitives, Threshold);
		detail::print(Stream, "primitive first floats", A.blobbies->primitive_first_floats, B.blobbies->primitive_first_floats, Threshold);
		detail::print(Stream, "primitive float counts", A.blobbies->primitive_float_counts, B.blobbies->primitive_float_counts, Threshold);
		detail::print(Stream, "varying data", A.blobbies->varying_data, B.blobbies->varying_data, Threshold);
		detail::print(Stream, "vertex data", A.blobbies->vertex_data, B.blobbies->vertex_data, Threshold);
		detail::print(Stream, "operators", A.blobbies->operators, B.blobbies->operators, Threshold);
		detail::print(Stream, "operator first operands", A.blobbies->operator_first_operands, B.blobbies->operator_first_operands, Threshold);
		detail::print(Stream, "operator operand counts", A.blobbies->operator_operand_counts, B.blobbies->operator_operand_counts, Threshold);
		detail::print(Stream, "floats", A.blobbies->floats, B.blobbies->floats, Threshold);
		detail::print(Stream, "operands", A.blobbies->operands, B.blobbies->operands, Threshold);
	}

	detail::print(Stream, "mesh", &A, &B);
	detail::print(Stream, "points", A.points, B.points, Threshold);
	detail::print(Stream, "point selection", A.point_selection, B.point_selection, Threshold);
	detail::print(Stream, "vertex data", A.vertex_data, B.vertex_data, Threshold);
}

} // namespace diff

} // namespace k3d

