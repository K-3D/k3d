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

#include "color.h"
#include "legacy_mesh.h"
#include "mesh_selection.h"
#include "mesh.h"
#include "shared_pointer.h"

#include <iterator>
#include <map>

namespace k3d
{

namespace dev
{

namespace detail
{

/// Return true if two shared arrays are equivalent (handles cases where they point to the same memory, etc)
template<typename array_type>
const bool equal(const array_type& LHS, const array_type& RHS)
{
	if(LHS.get() == RHS.get())
		return true;

	if(LHS && RHS)
		return *LHS == *RHS;

	return false;
}

/// Return true if two groups of named arrays are equivalent (handles cases where they point to the same memory, etc)
const bool equal(const mesh::named_arrays& LHS, const mesh::named_arrays& RHS)
{
	return true;
}

void store_selection(const boost::shared_ptr<const mesh::selection_t>& MeshSelection, mesh_selection::records_t& Records)
{
	if(!MeshSelection)
		return;

	const mesh::selection_t& selection_weight = *MeshSelection;

	const size_t selection_begin = 0;
	const size_t selection_end = selection_begin + selection_weight.size();
	for(size_t selection = selection_begin; selection != selection_end; ++selection)
		Records.push_back(mesh_selection::record(selection, selection+1, selection_weight[selection]));
}

template<typename gprims_type>
void replace_selection(const mesh_selection::records_t& Records, const gprims_type& GPrims, boost::shared_ptr<const mesh::selection_t>& Selection)
{
	return_if_fail(GPrims);

	const size_t gprim_count = GPrims->size();

	if(!Selection || Selection->size() != gprim_count)
		Selection.reset(new mesh::selection_t(gprim_count));

	mesh::selection_t& selection = *make_unique(Selection);

	for(mesh_selection::records_t::const_iterator record = Records.begin(); record != Records.end(); ++record)
	{
		if(record->begin >= gprim_count)
			break;

		mesh::selection_t::iterator begin = selection.begin() + record->begin;
		mesh::selection_t::iterator end = selection.begin() + std::min(gprim_count, record->end);
		std::fill(begin, end, record->weight);
	}
}

template<typename pointer_type>
void print(std::ostream& Stream, const std::string& Label, const pointer_type& Pointer)
{
	if(Pointer)
	{
		Stream << Label << " (" << Pointer->size() <<  "): ";
		std::copy(Pointer->begin(), Pointer->end(), std::ostream_iterator<typename pointer_type::element_type::value_type>(Stream, " "));
		Stream << "\n";
	}
}

void print(std::ostream& Stream, const std::string& Label, const k3d::dev::mesh::named_arrays& Arrays)
{
    for(k3d::dev::mesh::named_arrays::const_iterator array_iterator = Arrays.begin(); array_iterator != Arrays.end(); ++array_iterator)
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
}

} // namespace detail

///////////////////////////////////////////////////////////////////////////////////
// mesh::point_groups_t

const bool mesh::point_groups_t::operator==(const mesh::point_groups_t& RHS) const
{
	return
		detail::equal(first_points, RHS.first_points) &&
		detail::equal(point_counts, RHS.point_counts) &&
		detail::equal(materials, RHS.materials) &&
		detail::equal(constant_data, RHS.constant_data) &&
		detail::equal(points, RHS.points) &&
		detail::equal(varying_data, RHS.varying_data);
}

const bool mesh::point_groups_t::operator!=(const mesh::point_groups_t& RHS) const
{
	return !(*this == RHS);
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::linear_curve_groups_t

const bool mesh::linear_curve_groups_t::operator==(const mesh::linear_curve_groups_t& RHS) const
{
	return
		detail::equal(first_curves, RHS.first_curves) &&
		detail::equal(curve_counts, RHS.curve_counts) &&
		detail::equal(periodic_curves, RHS.periodic_curves) &&
		detail::equal(materials, RHS.materials) &&
		detail::equal(constant_data, RHS.constant_data) &&
		detail::equal(curve_first_points, RHS.curve_first_points) &&
		detail::equal(curve_point_counts, RHS.curve_point_counts) &&
		detail::equal(curve_selection, RHS.curve_selection) &&
		detail::equal(uniform_data, RHS.uniform_data) &&
		detail::equal(curve_points, RHS.curve_points);
}

const bool mesh::linear_curve_groups_t::operator!=(const mesh::linear_curve_groups_t& RHS) const
{
	return !(*this == RHS);
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::cubic_curve_groups_t

const bool mesh::cubic_curve_groups_t::operator==(const mesh::cubic_curve_groups_t& RHS) const
{
	return
		detail::equal(first_curves, RHS.first_curves) &&
		detail::equal(curve_counts, RHS.curve_counts) &&
		detail::equal(periodic_curves, RHS.periodic_curves) &&
		detail::equal(materials, RHS.materials) &&
		detail::equal(constant_data, RHS.constant_data) &&
		detail::equal(curve_first_points, RHS.curve_first_points) &&
		detail::equal(curve_point_counts, RHS.curve_point_counts) &&
		detail::equal(curve_selection, RHS.curve_selection) &&
		detail::equal(uniform_data, RHS.uniform_data) &&
		detail::equal(curve_points, RHS.curve_points);
}

const bool mesh::cubic_curve_groups_t::operator!=(const mesh::cubic_curve_groups_t& RHS) const
{
	return !(*this == RHS);
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::nurbs_curve_groups_t

const bool mesh::nurbs_curve_groups_t::operator==(const mesh::nurbs_curve_groups_t& RHS) const
{
	return
		detail::equal(first_curves, RHS.first_curves) &&
		detail::equal(curve_counts, RHS.curve_counts) &&
		detail::equal(materials, RHS.materials) &&
		detail::equal(constant_data, RHS.constant_data) &&
		detail::equal(curve_first_points, RHS.curve_first_points) &&
		detail::equal(curve_point_counts, RHS.curve_point_counts) &&
		detail::equal(curve_orders, RHS.curve_orders) &&
		detail::equal(curve_first_knots, RHS.curve_first_knots) &&
		detail::equal(curve_selection, RHS.curve_selection) &&
		detail::equal(uniform_data, RHS.uniform_data) &&
		detail::equal(curve_points, RHS.curve_points) &&
		detail::equal(curve_point_weights, RHS.curve_point_weights) &&
		detail::equal(curve_knots, RHS.curve_knots);
}

const bool mesh::nurbs_curve_groups_t::operator!=(const mesh::nurbs_curve_groups_t& RHS) const
{
	return !(*this == RHS);
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::bilinear_patches_t

const bool mesh::bilinear_patches_t::operator==(const mesh::bilinear_patches_t& RHS) const
{
	return
		detail::equal(patch_selection, RHS.patch_selection) &&
		detail::equal(patch_materials, RHS.patch_materials) &&
		detail::equal(constant_data, RHS.constant_data) &&
		detail::equal(uniform_data, RHS.uniform_data) &&
		detail::equal(patch_points, RHS.patch_points) &&
		detail::equal(varying_data, RHS.varying_data);
}

const bool mesh::bilinear_patches_t::operator!=(const mesh::bilinear_patches_t& RHS) const
{
	return !(*this == RHS);
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::bicubic_patches_t

const bool mesh::bicubic_patches_t::operator==(const mesh::bicubic_patches_t& RHS) const
{
	return
		detail::equal(patch_selection, RHS.patch_selection) &&
		detail::equal(patch_materials, RHS.patch_materials) &&
		detail::equal(constant_data, RHS.constant_data) &&
		detail::equal(uniform_data, RHS.uniform_data) &&
		detail::equal(patch_points, RHS.patch_points) &&
		detail::equal(varying_data, RHS.varying_data);
}

const bool mesh::bicubic_patches_t::operator!=(const mesh::bicubic_patches_t& RHS) const
{
	return !(*this == RHS);
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::nurbs_patches_t

const bool mesh::nurbs_patches_t::operator==(const mesh::nurbs_patches_t& RHS) const
{
	return
		detail::equal(patch_first_points, RHS.patch_first_points) &&
		detail::equal(patch_u_point_counts, RHS.patch_u_point_counts) &&
		detail::equal(patch_v_point_counts, RHS.patch_v_point_counts) &&
		detail::equal(patch_u_orders, RHS.patch_u_orders) &&
		detail::equal(patch_v_orders, RHS.patch_v_orders) &&
		detail::equal(patch_u_first_knots, RHS.patch_u_first_knots) &&
		detail::equal(patch_v_first_knots, RHS.patch_v_first_knots) &&
		detail::equal(patch_selection, RHS.patch_selection) &&
		detail::equal(patch_materials, RHS.patch_materials) &&
		detail::equal(constant_data, RHS.constant_data) &&
		detail::equal(uniform_data, RHS.uniform_data) &&
		detail::equal(patch_points, RHS.patch_points) &&
		detail::equal(patch_point_weights, RHS.patch_point_weights) &&
		detail::equal(patch_u_knots, RHS.patch_u_knots) &&
		detail::equal(patch_v_knots, RHS.patch_v_knots) &&
		detail::equal(varying_data, RHS.varying_data);
}

const bool mesh::nurbs_patches_t::operator!=(const mesh::nurbs_patches_t& RHS) const
{
	return !(*this == RHS);
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::polyhedra_t

const bool mesh::polyhedra_t::operator==(const mesh::polyhedra_t& RHS) const
{
	return
		detail::equal(first_faces, RHS.first_faces) &&
		detail::equal(face_counts, RHS.face_counts) &&
		detail::equal(types, RHS.types) &&
		detail::equal(constant_data, RHS.constant_data) &&
		detail::equal(face_first_loops, RHS.face_first_loops) &&
		detail::equal(face_loop_counts, RHS.face_loop_counts) &&
		detail::equal(face_selection, RHS.face_selection) &&
		detail::equal(face_materials, RHS.face_materials) &&
		detail::equal(uniform_data, RHS.uniform_data) &&
		detail::equal(loop_first_edges, RHS.loop_first_edges) &&
		detail::equal(edge_points, RHS.edge_points) &&
		detail::equal(clockwise_edges, RHS.clockwise_edges) &&
		detail::equal(edge_selection, RHS.edge_selection) &&
		detail::equal(face_varying_data, RHS.face_varying_data);
}

const bool mesh::polyhedra_t::operator!=(const mesh::polyhedra_t& RHS) const
{
	return !(*this == RHS);
}

////////////////////////////////////////////////////////////////////////////////////
// mesh::blobbies_t

const bool mesh::blobbies_t::operator==(const mesh::blobbies_t& RHS) const
{
	return
		detail::equal(first_primitives, RHS.first_primitives) &&
		detail::equal(primitive_counts, RHS.primitive_counts) &&
		detail::equal(first_operators, RHS.first_operators) &&
		detail::equal(operator_counts, RHS.operator_counts) &&
		detail::equal(materials, RHS.materials) &&
		detail::equal(constant_data, RHS.constant_data) &&
		detail::equal(uniform_data, RHS.uniform_data) &&
		detail::equal(primitives, RHS.primitives) &&
		detail::equal(primitive_first_floats, RHS.primitive_first_floats) &&
		detail::equal(primitive_float_counts, RHS.primitive_float_counts) &&
		detail::equal(varying_data, RHS.varying_data) &&
		detail::equal(vertex_data, RHS.vertex_data) &&
		detail::equal(operators, RHS.operators) &&
		detail::equal(operator_first_operands, RHS.operator_first_operands) &&
		detail::equal(operator_operand_counts, RHS.operator_operand_counts) &&
		detail::equal(floats, RHS.floats) &&
		detail::equal(operands, RHS.operands);
}

const bool mesh::blobbies_t::operator!=(const mesh::blobbies_t& RHS) const
{
	return !(*this == RHS);
}

////////////////////////////////////////////////////////////////////////////////////
// mesh

mesh::mesh()
{
}

const bool mesh::operator==(const mesh& RHS) const
{
	return
		detail::equal(points, RHS.points) &&
		detail::equal(point_selection, RHS.point_selection) &&
		detail::equal(vertex_data, RHS.vertex_data) &&
		detail::equal(point_groups, RHS.point_groups) &&
		detail::equal(linear_curve_groups, RHS.linear_curve_groups) &&
		detail::equal(cubic_curve_groups, RHS.cubic_curve_groups) &&
		detail::equal(nurbs_curve_groups, RHS.nurbs_curve_groups) &&
		detail::equal(bilinear_patches, RHS.bilinear_patches) &&
		detail::equal(bicubic_patches, RHS.bicubic_patches) &&
		detail::equal(nurbs_patches, RHS.nurbs_patches) &&
		detail::equal(polyhedra, RHS.polyhedra) &&
		detail::equal(blobbies, RHS.blobbies);
}

const bool mesh::operator!=(const mesh& RHS) const
{
	return !(*this == RHS);
}

mesh& mesh::operator=(const k3d::legacy::mesh& RHS)
{
	// Convert points ...
	std::map<k3d::legacy::point*, size_t> point_map;

	const size_t point_size = RHS.points.size();
	boost::shared_ptr<points_t> points(new points_t(point_size));
	boost::shared_ptr<selection_t> point_selection(new selection_t(point_size));

	for(size_t i = 0; i != point_size; ++i)
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

		for(k3d::legacy::mesh::point_groups_t::const_iterator group = RHS.point_groups.begin(); group != RHS.point_groups.end(); ++group)
		{
			first_points->push_back(points->size());
			point_counts->push_back((*group)->points.size());
			materials->push_back((*group)->material);

			for(k3d::legacy::point_group::points_t::const_iterator point = (*group)->points.begin(); point != (*group)->points.end(); ++point)
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

		for(k3d::legacy::mesh::linear_curve_groups_t::const_iterator group = RHS.linear_curve_groups.begin(); group != RHS.linear_curve_groups.end(); ++group)
		{
			first_curves->push_back(curve_first_points->size());
			curve_counts->push_back((*group)->curves.size());
			periodic_curves->push_back((*group)->wrap);
			materials->push_back((*group)->material);

			for(k3d::legacy::linear_curve_group::curves_t::const_iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			{
				curve_first_points->push_back(curve_points->size());
				curve_point_counts->push_back((*curve)->control_points.size());
				curve_selection->push_back((*curve)->selection_weight);

				for(k3d::legacy::linear_curve::control_points_t::const_iterator point = (*curve)->control_points.begin(); point != (*curve)->control_points.end(); ++point)
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

		for(k3d::legacy::mesh::cubic_curve_groups_t::const_iterator group = RHS.cubic_curve_groups.begin(); group != RHS.cubic_curve_groups.end(); ++group)
		{
			first_curves->push_back(curve_first_points->size());
			curve_counts->push_back((*group)->curves.size());
			periodic_curves->push_back((*group)->wrap);
			materials->push_back((*group)->material);

			for(k3d::legacy::cubic_curve_group::curves_t::const_iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			{
				curve_first_points->push_back(curve_points->size());
				curve_point_counts->push_back((*curve)->control_points.size());
				curve_selection->push_back((*curve)->selection_weight);

				for(k3d::legacy::cubic_curve::control_points_t::const_iterator point = (*curve)->control_points.begin(); point != (*curve)->control_points.end(); ++point)
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

		for(k3d::legacy::mesh::nucurve_groups_t::const_iterator group = RHS.nucurve_groups.begin(); group != RHS.nucurve_groups.end(); ++group)
		{
			first_curves->push_back(curve_first_points->size());
			curve_counts->push_back((*group)->curves.size());
			materials->push_back((*group)->material);

			for(k3d::legacy::nucurve_group::curves_t::const_iterator curve = (*group)->curves.begin(); curve != (*group)->curves.end(); ++curve)
			{
				curve_first_points->push_back(curve_points->size());
				curve_point_counts->push_back((*curve)->knots.size() - (*curve)->order);
				curve_orders->push_back((*curve)->order);
				curve_first_knots->push_back(curve_knots->size());
				curve_selection->push_back((*curve)->selection_weight);

				for(k3d::legacy::nucurve::control_points_t::const_iterator point = (*curve)->control_points.begin(); point != (*curve)->control_points.end(); ++point)
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

		for(k3d::legacy::mesh::bilinear_patches_t::const_iterator patch = RHS.bilinear_patches.begin(); patch != RHS.bilinear_patches.end(); ++patch)
		{
			patch_selection->push_back((*patch)->selection_weight);
			patch_materials->push_back((*patch)->material);

			for(k3d::legacy::bilinear_patch::control_points_t::const_iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
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

		for(k3d::legacy::mesh::bicubic_patches_t::const_iterator patch = RHS.bicubic_patches.begin(); patch != RHS.bicubic_patches.end(); ++patch)
		{
			patch_selection->push_back((*patch)->selection_weight);
			patch_materials->push_back((*patch)->material);

			for(k3d::legacy::bicubic_patch::control_points_t::const_iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
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

		for(k3d::legacy::mesh::nupatches_t::const_iterator patch = RHS.nupatches.begin(); patch != RHS.nupatches.end(); ++patch)
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

			for(k3d::legacy::nupatch::control_points_t::const_iterator point = (*patch)->control_points.begin(); point != (*patch)->control_points.end(); ++point)
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

		for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = RHS.polyhedra.begin(); polyhedron != RHS.polyhedra.end(); ++polyhedron)
		{
			size_t first_face = face_first_loops->size();
			size_t face_count = 0;
			k3d::dev::mesh::polyhedra_t::polyhedron_type type = k3d::dev::mesh::polyhedra_t::POLYGONS;

			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				++face_count;

				size_t face_first_loop = loop_first_edges->size();
				size_t face_loop_count = 1 + (*face)->holes.size();

				const size_t first_edge = edge_points->size();

				loop_first_edges->push_back(first_edge);
				for(k3d::legacy::split_edge* edge = (*face)->first_edge; edge; edge = edge->face_clockwise)
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

				for(k3d::legacy::face::holes_t::iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
				{
					const size_t first_edge = edge_points->size();

					loop_first_edges->push_back(first_edge);
					for(k3d::legacy::split_edge* edge = *hole; edge; edge = edge->face_clockwise)
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
	std::string buffer;
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
	std::string buffer;
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
	std::string buffer;
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
	Stream << "mesh:" << std::endl;

	if(RHS.point_groups)
	{
		Stream << "  point groups:" << std::endl;
		detail::print(Stream, "    first points", RHS.point_groups->first_points);
		detail::print(Stream, "    point counts", RHS.point_groups->point_counts);
		detail::print(Stream, "    constant data", RHS.point_groups->constant_data);
		detail::print(Stream, "    points", RHS.point_groups->points);
		detail::print(Stream, "    varying data", RHS.point_groups->varying_data);
	}

	if(RHS.linear_curve_groups)
	{
		Stream << "  linear curve groups:" << std::endl;
		detail::print(Stream, "    first curves", RHS.linear_curve_groups->first_curves);
		detail::print(Stream, "    curve counts", RHS.linear_curve_groups->curve_counts);
		detail::print(Stream, "    periodic curves", RHS.linear_curve_groups->periodic_curves);
		detail::print(Stream, "    constant data", RHS.linear_curve_groups->constant_data);
		detail::print(Stream, "    curve first points", RHS.linear_curve_groups->curve_first_points);
		detail::print(Stream, "    curve point counts", RHS.linear_curve_groups->curve_point_counts);
		detail::print(Stream, "    curve selection", RHS.linear_curve_groups->curve_selection);
		detail::print(Stream, "    uniform data", RHS.linear_curve_groups->uniform_data);
		detail::print(Stream, "    curve points", RHS.linear_curve_groups->curve_points);
	}

	if(RHS.cubic_curve_groups)
	{
		Stream << "  cubic curve groups:" << std::endl;
		detail::print(Stream, "    first curves", RHS.cubic_curve_groups->first_curves);
		detail::print(Stream, "    curve counts", RHS.cubic_curve_groups->curve_counts);
		detail::print(Stream, "    periodic curves", RHS.cubic_curve_groups->periodic_curves);
		detail::print(Stream, "    constant data", RHS.cubic_curve_groups->constant_data);
		detail::print(Stream, "    curve first points", RHS.cubic_curve_groups->curve_first_points);
		detail::print(Stream, "    curve point counts", RHS.cubic_curve_groups->curve_point_counts);
		detail::print(Stream, "    curve selection", RHS.cubic_curve_groups->curve_selection);
		detail::print(Stream, "    uniform data", RHS.cubic_curve_groups->uniform_data);
		detail::print(Stream, "    curve points", RHS.cubic_curve_groups->curve_points);
	}

	if(RHS.nurbs_curve_groups)
	{
		Stream << "  nurbs curve groups:" << std::endl;
		detail::print(Stream, "    first curves", RHS.nurbs_curve_groups->first_curves);
		detail::print(Stream, "    curve counts", RHS.nurbs_curve_groups->curve_counts);
		detail::print(Stream, "    constant data", RHS.nurbs_curve_groups->constant_data);
		detail::print(Stream, "    curve first points", RHS.nurbs_curve_groups->curve_first_points);
		detail::print(Stream, "    curve point counts", RHS.nurbs_curve_groups->curve_point_counts);
		detail::print(Stream, "    curve orders", RHS.nurbs_curve_groups->curve_orders);
		detail::print(Stream, "    curve selection", RHS.nurbs_curve_groups->curve_selection);
		detail::print(Stream, "    uniform data", RHS.nurbs_curve_groups->uniform_data);
		detail::print(Stream, "    curve points", RHS.nurbs_curve_groups->curve_points);
		detail::print(Stream, "    curve point weights", RHS.nurbs_curve_groups->curve_point_weights);
		detail::print(Stream, "    curve knots", RHS.nurbs_curve_groups->curve_knots);
	}

	if(RHS.bilinear_patches)
	{
		Stream << "  bilinear patches:" << std::endl;
		detail::print(Stream, "    patch selection", RHS.bilinear_patches->patch_selection);
		detail::print(Stream, "    constant data", RHS.bilinear_patches->constant_data);
		detail::print(Stream, "    uniform data", RHS.bilinear_patches->uniform_data);
		detail::print(Stream, "    patch points", RHS.bilinear_patches->patch_points);
		detail::print(Stream, "    varying data", RHS.bilinear_patches->varying_data);
	}

	if(RHS.bicubic_patches)
	{
		Stream << "  bicubic patches:" << std::endl;
		detail::print(Stream, "    patch selection", RHS.bicubic_patches->patch_selection);
		detail::print(Stream, "    constant data", RHS.bicubic_patches->constant_data);
		detail::print(Stream, "    uniform data", RHS.bicubic_patches->uniform_data);
		detail::print(Stream, "    patch points", RHS.bicubic_patches->patch_points);
		detail::print(Stream, "    varying data", RHS.bicubic_patches->varying_data);
	}

	if(RHS.nurbs_patches)
	{
		Stream << "  nurbs patches:" << std::endl;
		detail::print(Stream, "    patch first points", RHS.nurbs_patches->patch_first_points);
		detail::print(Stream, "    patch u point counts", RHS.nurbs_patches->patch_u_point_counts);
		detail::print(Stream, "    patch v point counts", RHS.nurbs_patches->patch_v_point_counts);
		detail::print(Stream, "    patch u orders", RHS.nurbs_patches->patch_u_orders);
		detail::print(Stream, "    patch v orders", RHS.nurbs_patches->patch_v_orders);
		detail::print(Stream, "    patch selection", RHS.nurbs_patches->patch_selection);
		detail::print(Stream, "    constant data", RHS.nurbs_patches->constant_data);
		detail::print(Stream, "    uniform data", RHS.nurbs_patches->uniform_data);
		detail::print(Stream, "    patch points", RHS.nurbs_patches->patch_points);
		detail::print(Stream, "    patch point weights", RHS.nurbs_patches->patch_point_weights);
		detail::print(Stream, "    patch u knots", RHS.nurbs_patches->patch_u_knots);
		detail::print(Stream, "    patch v knots", RHS.nurbs_patches->patch_v_knots);
		detail::print(Stream, "    varying data", RHS.nurbs_patches->varying_data);
	}

	if(RHS.polyhedra)
	{
		Stream << "  polyhedra:" << std::endl;
		detail::print(Stream, "    first faces", RHS.polyhedra->first_faces);
		detail::print(Stream, "    face counts", RHS.polyhedra->face_counts);
		detail::print(Stream, "    types", RHS.polyhedra->types);
		detail::print(Stream, "    constant data", RHS.polyhedra->constant_data);
		detail::print(Stream, "    face first loops", RHS.polyhedra->face_first_loops);
		detail::print(Stream, "    face loop counts", RHS.polyhedra->face_loop_counts);
		detail::print(Stream, "    face selection", RHS.polyhedra->face_selection);
		detail::print(Stream, "    uniform data", RHS.polyhedra->uniform_data);
		detail::print(Stream, "    loop first edges", RHS.polyhedra->loop_first_edges);
		detail::print(Stream, "    edge points", RHS.polyhedra->edge_points);
		detail::print(Stream, "    clockwise edges", RHS.polyhedra->clockwise_edges);
		detail::print(Stream, "    edge selection", RHS.polyhedra->edge_selection);
		detail::print(Stream, "    face varying data", RHS.polyhedra->face_varying_data);
	}

	if(RHS.blobbies)
	{
		Stream << "  blobbies:" << std::endl;
		detail::print(Stream, "    first primitives", RHS.blobbies->first_primitives);
		detail::print(Stream, "    primitive counts", RHS.blobbies->primitive_counts);
		detail::print(Stream, "    first operators", RHS.blobbies->first_operators);
		detail::print(Stream, "    operator counts", RHS.blobbies->operator_counts);
		detail::print(Stream, "    constant data", RHS.blobbies->constant_data);
		detail::print(Stream, "    uniform data", RHS.blobbies->uniform_data);
		detail::print(Stream, "    primitives", RHS.blobbies->primitives);
		detail::print(Stream, "    primitive first floats", RHS.blobbies->primitive_first_floats);
		detail::print(Stream, "    primitive float counts", RHS.blobbies->primitive_float_counts);
		detail::print(Stream, "    varying data", RHS.blobbies->varying_data);
		detail::print(Stream, "    vertex data", RHS.blobbies->vertex_data);
		detail::print(Stream, "    operators", RHS.blobbies->operators);
		detail::print(Stream, "    operator first operands", RHS.blobbies->operator_first_operands);
		detail::print(Stream, "    operator operand counts", RHS.blobbies->operator_operand_counts);
		detail::print(Stream, "    floats", RHS.blobbies->floats);
		detail::print(Stream, "    operands", RHS.blobbies->operands);
	}

	detail::print(Stream, "  points", RHS.points);
	detail::print(Stream, "  point selection", RHS.point_selection);
	detail::print(Stream, "  vertex data", RHS.vertex_data);

	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// store_selection

void store_selection(const mesh& Mesh, mesh_selection& Selection)
{
	Selection.points.clear();
	Selection.edges.clear();
	Selection.faces.clear();
	Selection.linear_curves.clear();
	Selection.cubic_curves.clear();
	Selection.nurbs_curves.clear();
	Selection.bilinear_patches.clear();
	Selection.bicubic_patches.clear();
	Selection.nurbs_patches.clear();

	detail::store_selection(Mesh.point_selection, Selection.points);

	if(Mesh.polyhedra)
	{
		detail::store_selection(Mesh.polyhedra->edge_selection, Selection.edges);
		detail::store_selection(Mesh.polyhedra->face_selection, Selection.faces);
	}

	if(Mesh.linear_curve_groups)
	{
		detail::store_selection(Mesh.linear_curve_groups->curve_selection, Selection.linear_curves);
	}

	if(Mesh.cubic_curve_groups)
	{
		detail::store_selection(Mesh.cubic_curve_groups->curve_selection, Selection.cubic_curves);
	}

	if(Mesh.nurbs_curve_groups)
	{
		detail::store_selection(Mesh.nurbs_curve_groups->curve_selection, Selection.nurbs_curves);
	}

	if(Mesh.bilinear_patches)
	{
		detail::store_selection(Mesh.bilinear_patches->patch_selection, Selection.bilinear_patches);
	}

	if(Mesh.bicubic_patches)
	{
		detail::store_selection(Mesh.bicubic_patches->patch_selection, Selection.bicubic_patches);
	}

	if(Mesh.nurbs_patches)
	{
		detail::store_selection(Mesh.nurbs_patches->patch_selection, Selection.nurbs_patches);
	}

	/** \todo Store blobby and quadric selections */
}

/////////////////////////////////////////////////////////////////////////////
// replace_selection

void replace_selection(const mesh_selection& MeshSelection, mesh& Mesh)
{
	detail::replace_selection(MeshSelection.points, Mesh.points, Mesh.point_selection);

	if(Mesh.polyhedra && Mesh.polyhedra->edge_points)
	{
		dev::mesh::polyhedra_t* const polyhedra = make_unique(Mesh.polyhedra);
		detail::replace_selection(MeshSelection.edges, polyhedra->edge_points, polyhedra->edge_selection);
	}

	if(Mesh.polyhedra && Mesh.polyhedra->face_first_loops)
	{
		dev::mesh::polyhedra_t* const polyhedra = make_unique(Mesh.polyhedra);
		detail::replace_selection(MeshSelection.faces, polyhedra->face_first_loops, polyhedra->face_selection);
	}

	if(Mesh.linear_curve_groups)
	{
		dev::mesh::linear_curve_groups_t* const linear_curve_groups = make_unique(Mesh.linear_curve_groups);
		detail::replace_selection(MeshSelection.linear_curves, linear_curve_groups->curve_first_points, linear_curve_groups->curve_selection);
	}

	if(Mesh.cubic_curve_groups)
	{
		dev::mesh::cubic_curve_groups_t* const cubic_curve_groups = make_unique(Mesh.cubic_curve_groups);
		detail::replace_selection(MeshSelection.cubic_curves, cubic_curve_groups->curve_first_points, cubic_curve_groups->curve_selection);
	}

	if(Mesh.nurbs_curve_groups)
	{
		dev::mesh::nurbs_curve_groups_t* const nurbs_curve_groups = make_unique(Mesh.nurbs_curve_groups);
		detail::replace_selection(MeshSelection.nurbs_curves, nurbs_curve_groups->curve_first_points, nurbs_curve_groups->curve_selection);
	}

	if(Mesh.bilinear_patches)
	{
		dev::mesh::bilinear_patches_t* const bilinear_patches = make_unique(Mesh.bilinear_patches);
		detail::replace_selection(MeshSelection.bilinear_patches, bilinear_patches->patch_materials, bilinear_patches->patch_selection);
	}

	if(Mesh.bicubic_patches)
	{
		dev::mesh::bicubic_patches_t* const bicubic_patches = make_unique(Mesh.bicubic_patches);
		detail::replace_selection(MeshSelection.bicubic_patches, bicubic_patches->patch_materials, bicubic_patches->patch_selection);
	}

	if(Mesh.nurbs_patches)
	{
		dev::mesh::nurbs_patches_t* const nurbs_patches = make_unique(Mesh.nurbs_patches);
		detail::replace_selection(MeshSelection.nurbs_patches, nurbs_patches->patch_materials, nurbs_patches->patch_selection);
	}
}

/////////////////////////////////////////////////////////////////////////////
// bounds

const bounding_box3 bounds(const dev::mesh& Mesh)
{
	bounding_box3 results;

	if(Mesh.points)
        return bounds(*Mesh.points);

	return results;
}

/////////////////////////////////////////////////////////////////////////////
// bounds

const bounding_box3 bounds(const dev::mesh::points_t& Points)
{
	bounding_box3 results;

    const size_t point_begin = 0;
    const size_t point_end = point_begin + Points.size();
    for(size_t point = point_begin; point != point_end; ++point)
        results.insert(Points[point]);

	return results;
}

//////////////////////////////////////////////////////////////////////////////
// center

const point3 center(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, const size_t EdgeIndex)
{
	point3 result(0, 0, 0);

	size_t count = 0;
	for(size_t edge = EdgeIndex; ; )
	{
		result += Points[EdgePoints[edge]];
		++count;

		edge = ClockwiseEdges[edge];
		if(edge == EdgeIndex)
			break;
	}

	if(count)
		result /= static_cast<double>(count);

	return result;
}

//////////////////////////////////////////////////////////////////////////////
// normal

const normal3 normal(const mesh::indices_t& EdgePoints, const mesh::indices_t& ClockwiseEdges, const mesh::points_t& Points, const size_t EdgeIndex)
{
	// Calculates the normal for an edge loop using the summation method, which is more robust than the three-point methods (handles zero-length edges)
	normal3 result(0, 0, 0);

	for(size_t edge = EdgeIndex; ; )
	{
		const point3& i = Points[EdgePoints[edge]];
		const point3& j = Points[EdgePoints[ClockwiseEdges[edge]]];

		result[0] += (i[1] + j[1]) * (j[2] - i[2]);
		result[1] += (i[2] + j[2]) * (j[0] - i[0]);
		result[2] += (i[0] + j[0]) * (j[1] - i[1]);

		edge = ClockwiseEdges[edge];
		if(edge == EdgeIndex)
			break;
	}

	return 0.5 * result;
}

////////////////////////////////////////////////////////////////////////////////
// deep_copy

void deep_copy(const mesh& From, mesh& To)
{
	To = From;
	assert_not_implemented(); // Need to ensure that all storage is unique
}


////////////////////////////////////////////////////////////////////////////////
// validate

/** \todo Handle all gprim types */
const bool validate(mesh& Mesh)
{
	bool result = true;

	if(Mesh.points && !validate_points(Mesh))
		result = false;

	if(Mesh.point_groups && !validate_point_groups(Mesh))
		result = false;

	if(Mesh.linear_curve_groups && !validate_linear_curve_groups(Mesh))
		result = false;

	if(Mesh.cubic_curve_groups && !validate_cubic_curve_groups(Mesh))
		result = false;

	if(Mesh.nurbs_curve_groups && !validate_nurbs_curve_groups(Mesh))
		result = false;

	if(Mesh.bilinear_patches && !validate_bilinear_patches(Mesh))
		result = false;

	if(Mesh.bicubic_patches && !validate_bicubic_patches(Mesh))
		result = false;

	if(Mesh.nurbs_patches && !validate_nurbs_patches(Mesh))
		result = false;

	if(Mesh.polyhedra && !validate_polyhedra(Mesh))
		result = false;

	if(Mesh.blobbies && !validate_blobbies(Mesh))
		result = false;

	return result;
}

const bool validate_points(const mesh& Mesh)
{
	if(!Mesh.points)
		return false;

	return_val_if_fail(Mesh.point_selection, false);

	return true;
}

const bool validate_point_groups(const mesh& Mesh)
{
	if(!Mesh.point_groups)
		return false;

	return_val_if_fail(Mesh.points, false);
	return_val_if_fail(Mesh.point_selection, false);
	return_val_if_fail(Mesh.point_groups->first_points, false);
	return_val_if_fail(Mesh.point_groups->point_counts, false);
	return_val_if_fail(Mesh.point_groups->points, false);
	return_val_if_fail(Mesh.point_groups->materials, false);

	return true;
}

const bool validate_linear_curve_groups(const mesh& Mesh)
{
	if(!Mesh.linear_curve_groups)
		return false;

	return_val_if_fail(Mesh.points, false);
	return_val_if_fail(Mesh.point_selection, false);
	return_val_if_fail(Mesh.linear_curve_groups->first_curves, false);
	return_val_if_fail(Mesh.linear_curve_groups->curve_counts, false);
	return_val_if_fail(Mesh.linear_curve_groups->periodic_curves, false);
	return_val_if_fail(Mesh.linear_curve_groups->curve_first_points, false);
	return_val_if_fail(Mesh.linear_curve_groups->curve_point_counts, false);
	return_val_if_fail(Mesh.linear_curve_groups->curve_points, false);
	return_val_if_fail(Mesh.linear_curve_groups->curve_selection, false);
	return_val_if_fail(Mesh.linear_curve_groups->materials, false);

	return true;
}

const bool validate_cubic_curve_groups(const mesh& Mesh)
{
	if(!Mesh.cubic_curve_groups)
		return false;

	return_val_if_fail(Mesh.points, false);
	return_val_if_fail(Mesh.point_selection, false);
	return_val_if_fail(Mesh.cubic_curve_groups->first_curves, false);
	return_val_if_fail(Mesh.cubic_curve_groups->curve_counts, false);
	return_val_if_fail(Mesh.cubic_curve_groups->periodic_curves, false);
	return_val_if_fail(Mesh.cubic_curve_groups->curve_first_points, false);
	return_val_if_fail(Mesh.cubic_curve_groups->curve_point_counts, false);
	return_val_if_fail(Mesh.cubic_curve_groups->curve_points, false);
	return_val_if_fail(Mesh.cubic_curve_groups->curve_selection, false);
	return_val_if_fail(Mesh.cubic_curve_groups->materials, false);

	return true;
}

const bool validate_nurbs_curve_groups(const mesh& Mesh)
{
	if(!Mesh.nurbs_curve_groups)
		return false;

	return_val_if_fail(Mesh.points, false);
	return_val_if_fail(Mesh.point_selection, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->first_curves, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_counts, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_first_points, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_point_counts, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_orders, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_first_knots, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_points, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_point_weights, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_knots, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->curve_selection, false);
	return_val_if_fail(Mesh.nurbs_curve_groups->materials, false);

	return true;
}

const bool validate_bilinear_patches(const mesh& Mesh)
{
	if(!Mesh.bilinear_patches)
		return false;

	return_val_if_fail(Mesh.points, false);
	return_val_if_fail(Mesh.point_selection, false);
	return_val_if_fail(Mesh.bilinear_patches->patch_points, false);
	return_val_if_fail(Mesh.bilinear_patches->patch_selection, false);
	return_val_if_fail(Mesh.bilinear_patches->patch_materials, false);

	return true;
}

const bool validate_bicubic_patches(const mesh& Mesh)
{
	if(!Mesh.bicubic_patches)
		return false;

	return_val_if_fail(Mesh.points, false);
	return_val_if_fail(Mesh.point_selection, false);
	return_val_if_fail(Mesh.bicubic_patches->patch_points, false);
	return_val_if_fail(Mesh.bicubic_patches->patch_selection, false);
	return_val_if_fail(Mesh.bicubic_patches->patch_materials, false);

	return true;
}

const bool validate_nurbs_patches(const mesh& Mesh)
{
	if(!Mesh.nurbs_patches)
		return false;

	return_val_if_fail(Mesh.points, false);
	return_val_if_fail(Mesh.point_selection, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_first_points, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_point_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_point_counts, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_orders, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_orders, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_first_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_first_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_points, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_point_weights, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_u_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_v_knots, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_selection, false);
	return_val_if_fail(Mesh.nurbs_patches->patch_materials, false);

	return true;
}

const bool validate_polyhedra(const mesh& Mesh)
{
	if(!Mesh.polyhedra)
		return false;

	return_val_if_fail(Mesh.points, false);
	return_val_if_fail(Mesh.point_selection, false);
	return_val_if_fail(Mesh.polyhedra->first_faces, false);
	return_val_if_fail(Mesh.polyhedra->face_counts, false);
	return_val_if_fail(Mesh.polyhedra->types, false);
	return_val_if_fail(Mesh.polyhedra->face_first_loops, false);
	return_val_if_fail(Mesh.polyhedra->face_loop_counts, false);
	return_val_if_fail(Mesh.polyhedra->loop_first_edges, false);
	return_val_if_fail(Mesh.polyhedra->edge_points, false);
	return_val_if_fail(Mesh.polyhedra->clockwise_edges, false);
	return_val_if_fail(Mesh.polyhedra->face_selection, false);
	return_val_if_fail(Mesh.polyhedra->edge_selection, false);
	return_val_if_fail(Mesh.polyhedra->face_materials, false);

	return true;
}

const bool validate_blobbies(const mesh& Mesh)
{
	if(!Mesh.blobbies)
		return false;

	return_val_if_fail(Mesh.blobbies->first_primitives, false);
	return_val_if_fail(Mesh.blobbies->primitive_counts, false);
	return_val_if_fail(Mesh.blobbies->first_operators, false);
	return_val_if_fail(Mesh.blobbies->operator_counts, false);
	return_val_if_fail(Mesh.blobbies->primitives, false);
	return_val_if_fail(Mesh.blobbies->primitive_first_floats, false);
	return_val_if_fail(Mesh.blobbies->primitive_float_counts, false);
	return_val_if_fail(Mesh.blobbies->operators, false);
	return_val_if_fail(Mesh.blobbies->operator_first_operands, false);
	return_val_if_fail(Mesh.blobbies->floats, false);
	return_val_if_fail(Mesh.blobbies->operands, false);
	return_val_if_fail(Mesh.blobbies->materials, false);

	return true;
}

const bool is_sds(const mesh& Mesh)
{
	return_val_if_fail(validate_polyhedra(Mesh), false);
	const mesh::polyhedra_t::types_t& types = *Mesh.polyhedra->types;
	for (size_t type = 0; type != types.size(); ++type)
	{
		if (types[type] ==  mesh::polyhedra_t::CATMULL_CLARK)
			return true;
	}
	return false;
}

} // namespace dev

} // namespace k3d

