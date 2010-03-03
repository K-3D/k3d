// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
  \author Carsten Haubold (CarstenHaubold@web.de)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "nurbs_curves.h"
#include "nurbs_patches.h"
#include "utility.h"

#include <k3dsdk/axis.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/table_copier.h>

#include <cmath>

namespace module
{

namespace nurbs
{

//////////////////////
// patch_point_data

patch_point_data::patch_point_data(const k3d::mesh& Mesh, const k3d::nurbs_patch::const_primitive& Primitive, const k3d::uint_t Patch)
{
	const k3d::uint_t points_begin = Primitive.patch_first_points[Patch];
	const k3d::uint_t points_end = Primitive.patch_u_point_counts[Patch] * Primitive.patch_v_point_counts[Patch];
	point_attributes = Mesh.point_attributes.clone_types();
	vertex_attributes = Primitive.vertex_attributes.clone(points_begin, points_end);
	k3d::table_copier point_attribute_copier(Mesh.point_attributes, point_attributes);
	for(k3d::uint_t point_idx = points_begin; point_idx != points_end; ++point_idx)
	{
		points.push_back(Mesh.points->at(Primitive.patch_points[point_idx]));
		point_attribute_copier.push_back(Primitive.patch_points[point_idx]);
		weights.push_back(Primitive.patch_point_weights[point_idx]);
	}
}

patch_point_data::patch_point_data(const k3d::uint_t Size) : points(Size), weights(Size)
{
	point_attributes.set_row_count(Size);
	vertex_attributes.set_row_count(Size);
}

const k3d::bool_t patch_point_data::validate() const
{
	return_val_if_fail(points.size() == weights.size(), false);
	return_val_if_fail(point_attributes.empty() || point_attributes.row_count() == points.size(), false);
	return_val_if_fail(vertex_attributes.empty() || vertex_attributes.row_count() == points.size(), false);
	return true;
}

////////////////////////////
// point_data_copier

point_data_copier::point_data_copier(const patch_point_data& Source, patch_point_data& Destination) : m_source(Source), m_destination(Destination)
{
	const k3d::uint_t destination_size = m_destination.points.size();
	if(m_destination.point_attributes.empty())
	{
		m_destination.point_attributes = m_source.point_attributes.clone_types();
		m_destination.point_attributes.set_row_count(destination_size);
	}
	m_point_attribute_copier.reset(new k3d::table_copier(m_source.point_attributes, m_destination.point_attributes));
	if(m_destination.vertex_attributes.empty())
	{
		m_destination.vertex_attributes = m_source.vertex_attributes.clone_types();
		m_destination.vertex_attributes.set_row_count(destination_size);
	}
	m_vertex_attribute_copier.reset(new k3d::table_copier(m_source.vertex_attributes, m_destination.vertex_attributes));
}

void point_data_copier::push_back(const k3d::uint_t Index)
{
	m_destination.points.push_back(m_source.points[Index]);
	m_destination.weights.push_back(m_source.weights[Index]);
	m_point_attribute_copier->push_back(Index);
	m_vertex_attribute_copier->push_back(Index);
}

void point_data_copier::push_back(const k3d::uint_t Count, const k3d::uint_t* Indices, const k3d::double_t* Weights)
{
	k3d::point3 p(0,0,0);
	k3d::double_t w = 0.0;
	for(k3d::uint_t i = 0; i != Count; ++i)
	{
		p += k3d::to_vector(m_source.points[Indices[i]])*Weights[i];
		w += m_source.weights[Indices[i]]*Weights[i];
	}
	m_destination.points.push_back(p);
	m_destination.weights.push_back(w);
	m_point_attribute_copier->push_back(Count, Indices, Weights);
	m_vertex_attribute_copier->push_back(Count, Indices, Weights);
}

void point_data_copier::copy(const k3d::uint_t SourceIndex, const k3d::uint_t TargetIndex)
{
	m_destination.points[TargetIndex] = m_source.points[SourceIndex];
	m_destination.weights[TargetIndex] = m_source.weights[SourceIndex];
	m_point_attribute_copier->copy(SourceIndex, TargetIndex);
	m_vertex_attribute_copier->copy(SourceIndex, TargetIndex);
}

void point_data_copier::copy(const k3d::uint_t Count, const k3d::uint_t* Indices, const k3d::double_t* Weights, const k3d::uint_t TargetIndex)
{
	k3d::point3 p(0,0,0);
	k3d::double_t w = 0.0;
	for(k3d::uint_t i = 0; i != Count; ++i)
	{
		p += k3d::to_vector(m_source.points[Indices[i]])*Weights[i];
		w += m_source.weights[Indices[i]]*Weights[i];
	}
	m_destination.points[TargetIndex] = p;
	m_destination.weights[TargetIndex] = w;
	m_point_attribute_copier->copy(Count, Indices, Weights, TargetIndex);
	m_vertex_attribute_copier->copy(Count, Indices, Weights, TargetIndex);
}

////////////////////////////
// curve_point_data_copier

curve_point_data_copier::curve_point_data_copier(const curve_arrays& Source, patch_point_data& Destination) : m_source(Source), m_destination(Destination)
{
	const k3d::uint_t destination_size = m_destination.points.size();
	if(m_destination.point_attributes.empty())
	{
		m_destination.point_attributes = m_source.point_attributes.clone_types();
		m_destination.point_attributes.set_row_count(destination_size);
	}
	m_point_attribute_copier.reset(new k3d::table_copier(m_source.point_attributes, m_destination.point_attributes));
	if(m_destination.vertex_attributes.empty())
	{
		m_destination.vertex_attributes = m_source.vertex_attributes.clone_types();
		m_destination.vertex_attributes.set_row_count(destination_size);
	}
	m_vertex_attribute_copier.reset(new k3d::table_copier(m_source.vertex_attributes, m_destination.vertex_attributes));
}

void curve_point_data_copier::push_back(const k3d::uint_t Index)
{
	m_destination.points.push_back(dehomogenize(m_source.points[Index]));
	const k3d::double_t w = 1 / m_source.points[Index][3];
	m_destination.weights.push_back(m_source.points[Index][3]);
	m_point_attribute_copier->push_back(1, &Index, &w);
	m_vertex_attribute_copier->push_back(1, &Index, &w);
}

void curve_point_data_copier::push_back(const k3d::uint_t Count, const k3d::uint_t* Indices, const k3d::double_t* Weights)
{
	k3d::point3 p(0,0,0);
	k3d::double_t w = 0.0;
	k3d::double_t* inverted_weights = new k3d::double_t[Count];
	for(k3d::uint_t i = 0; i != Count; ++i)
	{
		p += k3d::to_vector(dehomogenize(m_source.points[Indices[i]]))*Weights[i];
		w += m_source.points[Indices[i]][3]*Weights[i];
		inverted_weights[i] = Weights[i]/m_source.points[Indices[i]][3]; // Make sure we 'unweight' the curve attributes
	}

	m_destination.points.push_back(p);
	m_destination.weights.push_back(w);
	m_point_attribute_copier->push_back(Count, Indices, inverted_weights);
	m_vertex_attribute_copier->push_back(Count, Indices, inverted_weights);
}

void curve_point_data_copier::copy(const k3d::uint_t SourceIndex, const k3d::uint_t TargetIndex)
{
	m_destination.points[TargetIndex] = dehomogenize(m_source.points[SourceIndex]);
	m_destination.weights[TargetIndex] = m_source.points[SourceIndex][3];
	const k3d::double_t w = 1/m_source.points[SourceIndex][3];
	m_point_attribute_copier->copy(1, &SourceIndex, &w, TargetIndex);
	m_vertex_attribute_copier->copy(1, &SourceIndex, &w, TargetIndex);
}

void curve_point_data_copier::copy(const k3d::uint_t Count, const k3d::uint_t* Indices, const k3d::double_t* Weights, const k3d::uint_t TargetIndex)
{
	k3d::point3 p(0,0,0);
	k3d::double_t w = 0.0;
	k3d::double_t* inverted_weights = new k3d::double_t[Count];
	for(k3d::uint_t i = 0; i != Count; ++i)
	{
		p += k3d::to_vector(dehomogenize(m_source.points[Indices[i]]))*Weights[i];
		w += m_source.points[Indices[i]][3]*Weights[i];
		inverted_weights[i] = Weights[i]/m_source.points[Indices[i]][3]; // Make sure we 'unweight' the curve attributes
	}

	m_destination.points[TargetIndex] = p;
	m_destination.weights[TargetIndex] = w;
	m_point_attribute_copier->copy(Count, Indices, inverted_weights, TargetIndex);
	m_vertex_attribute_copier->copy(Count, Indices, inverted_weights, TargetIndex);
}

///////////////////////////
// patch_attribute_tables

patch_attribute_tables::patch_attribute_tables(const k3d::nurbs_patch::const_primitive& Primitive, const k3d::uint_t Patch)
{
	if(!Primitive.patch_attributes.empty())
	{
		patch_attributes = Primitive.patch_attributes.clone(Patch, Patch+1);
	}
	if(!Primitive.parameter_attributes.empty())
	{
		parameter_attributes = Primitive.parameter_attributes.clone(4*Patch, 4*Patch+4);
	}
}

///////////////////////////


void add_patch(k3d::mesh& Mesh,
		k3d::nurbs_patch::primitive& NurbsPatches,
		const patch_point_data& PointData,
		const k3d::mesh::knots_t& UKnots,
		const k3d::mesh::knots_t& VKnots,
		const k3d::uint_t UOrder,
		const k3d::uint_t VOrder,
		const k3d::table& PatchAttributes,
		const k3d::table& ParameterAttributes)
{
	// Sanity checking ...
	return_if_fail(Mesh.points);
	return_if_fail(Mesh.point_selection);

	return_if_fail(PointData.validate());

	return_if_fail(UOrder >= 2);
	return_if_fail(VOrder >= 2);

	k3d::mesh::points_t& points = Mesh.points.writable();
	k3d::mesh::selection_t& point_selection = Mesh.point_selection.writable();

	NurbsPatches.patch_first_points.push_back(NurbsPatches.patch_points.size());
	const k3d::uint_t point_count = PointData.points.size();

	if(Mesh.point_attributes.empty())
	{
		Mesh.point_attributes = PointData.point_attributes.clone_types();
		Mesh.point_attributes.set_row_count(Mesh.points->size());
	}
	k3d::table_copier point_copier(PointData.point_attributes, Mesh.point_attributes);

	if(NurbsPatches.vertex_attributes.empty())
	{
		NurbsPatches.vertex_attributes = PointData.vertex_attributes.clone_types();
		NurbsPatches.vertex_attributes.set_row_count(NurbsPatches.patch_points.size());
	}
	k3d::table_copier vertex_copier(PointData.vertex_attributes, NurbsPatches.vertex_attributes);

	for(k3d::uint_t i = 0; i != point_count; ++i)
	{
		NurbsPatches.patch_points.push_back(points.size());
		points.push_back(PointData.points[i]);
		point_copier.push_back(i);
		vertex_copier.push_back(i);
	}
	point_selection.resize(points.size(), 0.0);

	NurbsPatches.patch_selections.push_back(0.0);
	NurbsPatches.patch_u_first_knots.push_back(NurbsPatches.patch_u_knots.size());
	NurbsPatches.patch_v_first_knots.push_back(NurbsPatches.patch_v_knots.size());
	NurbsPatches.patch_u_orders.push_back(UOrder);
	NurbsPatches.patch_v_orders.push_back(VOrder);
	NurbsPatches.patch_u_point_counts.push_back(UKnots.size() - UOrder);
	NurbsPatches.patch_v_point_counts.push_back(VKnots.size() - VOrder);
	NurbsPatches.patch_trim_loop_counts.push_back(0);
	NurbsPatches.patch_first_trim_loops.push_back(0);
	NurbsPatches.patch_point_weights.insert(NurbsPatches.patch_point_weights.end(), PointData.weights.begin(), PointData.weights.end());
	NurbsPatches.patch_u_knots.insert(NurbsPatches.patch_u_knots.end(), UKnots.begin(), UKnots.end());
	NurbsPatches.patch_v_knots.insert(NurbsPatches.patch_v_knots.end(), VKnots.begin(), VKnots.end());

	if(NurbsPatches.patch_attributes.empty())
	{
		NurbsPatches.patch_attributes = PatchAttributes.clone_types();
		NurbsPatches.patch_attributes.set_row_count(NurbsPatches.patch_first_points.size()-1);
	}
	k3d::table_copier(PatchAttributes, NurbsPatches.patch_attributes).push_back(0);

	if(NurbsPatches.parameter_attributes.empty())
	{
		NurbsPatches.parameter_attributes = ParameterAttributes.clone_types();
		NurbsPatches.parameter_attributes.set_row_count(4*(NurbsPatches.patch_first_points.size()-1));
	}
	k3d::table_copier param_copier(ParameterAttributes, NurbsPatches.parameter_attributes);
	param_copier.push_back(0);
	param_copier.push_back(1);
	param_copier.push_back(2);
	param_copier.push_back(3);
}

void copy_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch)
{
	const k3d::uint_t u_order = InputPatches.patch_u_orders[Patch];
	const k3d::uint_t v_order = InputPatches.patch_v_orders[Patch];
	const k3d::uint_t u_point_count = InputPatches.patch_u_point_counts[Patch];
	const k3d::uint_t v_point_count = InputPatches.patch_v_point_counts[Patch];

	patch_point_data point_data(InputMesh, InputPatches, Patch);
	patch_attribute_tables attributes(InputPatches, Patch);

	const k3d::uint_t u_knots_begin = InputPatches.patch_u_first_knots[Patch];
	const k3d::uint_t u_knots_end = u_knots_begin + u_order + u_point_count;
	k3d::mesh::knots_t u_knots(InputPatches.patch_u_knots.begin() + u_knots_begin, InputPatches.patch_u_knots.begin() + u_knots_end);
	const k3d::uint_t v_knots_begin = InputPatches.patch_v_first_knots[Patch];
	const k3d::uint_t v_knots_end = v_knots_begin + v_order + v_point_count;
	k3d::mesh::knots_t v_knots(InputPatches.patch_v_knots.begin() + v_knots_begin, InputPatches.patch_v_knots.begin() + v_knots_end);
	add_patch(OutputMesh, OutputPatches, point_data, u_knots, v_knots, u_order, v_order, attributes.patch_attributes, attributes.parameter_attributes);
	OutputPatches.patch_materials.push_back(InputPatches.patch_materials[Patch]);
}

void add_trim_curve(k3d::nurbs_patch::primitive& OutputPatches, const k3d::uint_t Patch, const k3d::mesh::points_2d_t& Points, const k3d::mesh::weights_t& Weights, const k3d::mesh::knots_t& Knots, const k3d::uint_t Order, const k3d::double_t UOffset, const k3d::double_t VOffset, const k3d::double_t UScale, const k3d::double_t VScale)
{
	return_if_fail(Points.size() == Weights.size());
	return_if_fail(Points.size() + Order == Knots.size());
	const k3d::uint_t insert_idx = OutputPatches.patch_trim_loop_counts[Patch] == 0 ? OutputPatches.trim_loop_first_curves.size() : OutputPatches.patch_first_trim_loops[Patch] + OutputPatches.patch_trim_loop_counts[Patch];
	if(OutputPatches.patch_trim_loop_counts[Patch] == 0)
	{
		OutputPatches.patch_first_trim_loops[Patch] = OutputPatches.trim_loop_first_curves.size();
	}
	const k3d::double_t umin = OutputPatches.patch_u_knots[OutputPatches.patch_u_first_knots[Patch]];
	const k3d::double_t urange = OutputPatches.patch_u_knots[OutputPatches.patch_u_first_knots[Patch] + OutputPatches.patch_u_point_counts[Patch] + OutputPatches.patch_u_orders[Patch] - 1] - umin;
	const k3d::double_t vmin = OutputPatches.patch_v_knots[OutputPatches.patch_v_first_knots[Patch]];
	const k3d::double_t vrange = OutputPatches.patch_v_knots[OutputPatches.patch_v_first_knots[Patch] + OutputPatches.patch_v_point_counts[Patch] + OutputPatches.patch_v_orders[Patch] - 1] - vmin;
	OutputPatches.trim_loop_first_curves.insert(OutputPatches.trim_loop_first_curves.begin() + insert_idx, OutputPatches.curve_first_points.size());
	OutputPatches.trim_loop_curve_counts.insert(OutputPatches.trim_loop_curve_counts.begin() + insert_idx, 1);
	OutputPatches.trim_loop_selections.insert(OutputPatches.trim_loop_selections.begin() + insert_idx, 1.0);
	for(k3d::uint_t patch = Patch + 1; patch != OutputPatches.patch_first_trim_loops.size(); ++patch)
		++OutputPatches.patch_first_trim_loops[patch];
	OutputPatches.curve_first_points.push_back(OutputPatches.curve_points.size());
	OutputPatches.curve_first_knots.push_back(OutputPatches.curve_knots.size());
	for(k3d::uint_t i = 0; i != Points.size(); ++i)
	{
		OutputPatches.curve_points.push_back(OutputPatches.points.size());
		const k3d::double_t u = (Points[i][0] * UScale) + UOffset;
		const k3d::double_t v = (Points[i][1] * VScale) + VOffset;
		OutputPatches.points.push_back(k3d::point2(u * urange + umin, v * vrange + vmin));
		OutputPatches.curve_point_weights.push_back(Weights[i]);
		OutputPatches.point_selections.push_back(1.0);
	}
	OutputPatches.curve_point_counts.push_back(Points.size());
	for(k3d::uint_t i = 0; i != Knots.size(); ++i)
	{
		OutputPatches.curve_knots.push_back(Knots[i]);
	}
	OutputPatches.curve_selections.push_back(1.0);
	OutputPatches.curve_orders.push_back(Order);
	OutputPatches.patch_trim_loop_counts[Patch]++;
}

void traverse_curve(const k3d::mesh& SourceCurves, const k3d::uint_t SourcePrimIdx, const k3d::mesh& CurvesToTraverse, k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches)
{
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> source_curves(k3d::nurbs_curve::validate(SourceCurves, *SourceCurves.primitives[SourcePrimIdx]));
	return_if_fail(source_curves);
	OutputPatches.patch_attributes = source_curves->curve_attributes.clone_types();
	k3d::table_copier uniform_copier(source_curves->curve_attributes, OutputPatches.patch_attributes);

	for(k3d::mesh::primitives_t::const_iterator trav_primitive = CurvesToTraverse.primitives.begin(); trav_primitive != CurvesToTraverse.primitives.end(); ++trav_primitive)
	{
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> curves_to_traverse(k3d::nurbs_curve::validate(CurvesToTraverse, **trav_primitive));
		if(!curves_to_traverse)
			continue;

		const k3d::uint_t source_curves_begin = 0;
		const k3d::uint_t source_curves_end = source_curves->curve_first_knots.size();
		for(k3d::uint_t source_curve = source_curves_begin; source_curve != source_curves_end; ++source_curve)
		{
			if(!source_curves->curve_selections[source_curve])
				continue;

			curve_arrays source_curve_arrays(SourceCurves, *source_curves, source_curve);

			const k3d::uint_t curves_to_traverse_begin = 0;
			const k3d::uint_t curves_to_traverse_end = curves_to_traverse->curve_first_knots.size();
			for(k3d::uint_t curve_to_traverse = curves_to_traverse_begin; curve_to_traverse != curves_to_traverse_end; ++curve_to_traverse)
			{
				if(!curves_to_traverse->curve_selections[curve_to_traverse])
					continue;

				curve_arrays curve_to_traverse_arrays(CurvesToTraverse, *curves_to_traverse, curve_to_traverse);

				//move the 1st curve along the 2nd
				const k3d::uint_t curve_points_begin[2] = {source_curves->curve_first_points[source_curve], curves_to_traverse->curve_first_points[curve_to_traverse]};
				const k3d::uint_t curve_points_end[2] = { curve_points_begin[0] + source_curves->curve_point_counts[source_curve], curve_points_begin[1] + curves_to_traverse->curve_point_counts[curve_to_traverse] };

				patch_point_data new_points;

				k3d::mesh::knots_t u_knots(source_curve_arrays.knots.begin(), source_curve_arrays.knots.end());
				k3d::mesh::knots_t v_knots(curve_to_traverse_arrays.knots.begin(), curve_to_traverse_arrays.knots.end());

				k3d::uint_t u_order = source_curves->curve_orders[source_curve];
				k3d::uint_t v_order = curves_to_traverse->curve_orders[curve_to_traverse];

				const k3d::uint_t point_count = source_curves->curve_point_counts[source_curve];
				const k3d::mesh::points_t& source_mesh_points = *SourceCurves.points;
				const k3d::mesh::points_t& traverse_mesh_points = *CurvesToTraverse.points;

				patch_point_data tmp_points;
				tmp_points.points.resize(2);
				tmp_points.weights.resize(2);
				curve_point_data_copier u_copier(curve_to_traverse_arrays, tmp_points);
				curve_point_data_copier v_copier(source_curve_arrays, tmp_points);
				point_data_copier new_copier(tmp_points, new_points);
				k3d::double_t weights[] = {0.5, 0.5};
				if(curve_to_traverse_arrays.point_attributes.empty())
				{
					weights[0] = 0.0;
					weights[1] = 1.0;
				}
				else if(source_curve_arrays.point_attributes.empty())
				{
					weights[1] = 0.0;
					weights[0] = 1.0;
				}
				const k3d::uint_t indices[] = {0,1};
				for (int i = 0; i < curves_to_traverse->curve_point_counts[curve_to_traverse]; i++)
				{
					k3d::vector3 delta_u = dehomogenize(curve_to_traverse_arrays.points[i]) - dehomogenize(curve_to_traverse_arrays.points.front());
					const k3d::double_t w_u = curve_to_traverse_arrays.points[i][3];
					u_copier.copy(i, 0);

					for (int j = 0; j < point_count; j++)
					{
						v_copier.copy(j, 1);
						new_copier.push_back(2, indices, weights);

						new_points.points.back() = dehomogenize(source_curve_arrays.points[j]) + delta_u;
						new_points.weights.back() = source_curve_arrays.points[j][3] * w_u;
					}
				}
				k3d::table patch_attributes = curve_to_traverse_arrays.curve_attributes.empty() ? source_curve_arrays.curve_attributes.clone() : curve_to_traverse_arrays.curve_attributes.clone();
				k3d::table parameter_attributes = curve_to_traverse_arrays.parameter_attributes.clone();
				k3d::table_copier v_parameter_copier(source_curve_arrays.parameter_attributes, parameter_attributes);
				v_parameter_copier.push_back(0);
				v_parameter_copier.push_back(1);
				add_patch(OutputMesh, OutputPatches, new_points, u_knots, v_knots, source_curves->curve_orders[source_curve], curves_to_traverse->curve_orders[curve_to_traverse], patch_attributes, parameter_attributes);
				uniform_copier.push_back(source_curve);
				OutputPatches.patch_materials.push_back(source_curves->material[0]);
			}
		}
	}
}

void traverse_curve(const k3d::mesh& SourceCurves, const k3d::mesh& CurvesToTraverse, k3d::mesh& OutputMesh)
{
	for(k3d::uint_t prim_idx = 0; prim_idx != SourceCurves.primitives.size(); ++prim_idx)
	{
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> source_curves(k3d::nurbs_curve::validate(SourceCurves, *SourceCurves.primitives[prim_idx]));
		if(!source_curves)
			continue;

		boost::scoped_ptr<k3d::nurbs_patch::primitive> output_patches(k3d::nurbs_patch::create(OutputMesh));
		traverse_curve(SourceCurves, prim_idx, CurvesToTraverse, OutputMesh, *output_patches);
	}
}

void extract_patch_curve_by_number(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::uint_t Curve, const k3d::bool_t UDirection)
{
	const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
	const k3d::uint_t curve_point_count = UDirection ? InputPatches.patch_u_point_counts[Patch] : InputPatches.patch_v_point_counts[Patch];
	return_if_fail(Curve < curve_count);

	curve_arrays output_curve;
	output_curve.order = UDirection ? InputPatches.patch_u_orders[Patch] : InputPatches.patch_v_orders[Patch];

	const k3d::mesh::knots_t& input_knots = UDirection ? InputPatches.patch_u_knots : InputPatches.patch_v_knots;
	const k3d::uint_t order = UDirection ? InputPatches.patch_u_orders[Patch] : InputPatches.patch_v_orders[Patch];
	const k3d::uint_t knot_count = curve_point_count + order;
	const k3d::uint_t knots_begin = UDirection ? InputPatches.patch_u_first_knots[Patch] : InputPatches.patch_v_first_knots[Patch];
	const k3d::uint_t knots_end = knots_begin + knot_count;
	output_curve.knots.insert(output_curve.knots.end(), input_knots.begin() + knots_begin, input_knots.begin() + knots_end);

	const k3d::uint_t point_step = UDirection ? 1 : curve_count;
	const k3d::uint_t points_begin = InputPatches.patch_first_points[Patch] + Curve * (UDirection ? curve_point_count : 1);
	const k3d::uint_t points_end = points_begin + point_step * curve_point_count;
	output_curve.vertex_attributes = InputPatches.vertex_attributes.clone_types();
	k3d::table_copier vertex_copier(InputPatches.vertex_attributes, output_curve.vertex_attributes);
	output_curve.point_attributes = InputMesh.point_attributes.clone_types();
	k3d::table_copier point_copier(InputMesh.point_attributes, output_curve.point_attributes);
	for (int i = points_begin; i < points_end; i += point_step)
	{
		const k3d::double_t w = InputPatches.patch_point_weights[i];
		const k3d::point3& p = InputMesh.points->at(InputPatches.patch_points[i]);
		output_curve.points.push_back(k3d::point4(p[0]*w, p[1]*w, p[2]*w, w));
		const k3d::uint_t idx = static_cast<k3d::uint_t>(i);
		vertex_copier.push_back(1, &idx, &w);
		point_copier.push_back(1, &(InputPatches.patch_points[i]), &w);
	}

	// Calculate the value of the parameter attributes at the start and end of the curve
	const k3d::uint_t start_indices[] = {Patch*4, UDirection ? Patch*4+1 : Patch*4+2};
	const k3d::uint_t end_indices[] = {UDirection ? Patch*4+2 : Patch*4+1, Patch*4+3};
	const k3d::mesh::knots_t& other_knots = UDirection ? InputPatches.patch_v_knots : InputPatches.patch_u_knots;
	const k3d::uint_t other_order = UDirection ? InputPatches.patch_v_orders[Patch] : InputPatches.patch_u_orders[Patch];
	const k3d::uint_t other_knots_begin = UDirection ? InputPatches.patch_v_first_knots[Patch] : InputPatches.patch_u_first_knots[Patch];
	const k3d::double_t u = (other_knots[other_order-1 + Curve] - other_knots.front()) / (other_knots.back() - other_knots.front());
	const k3d::double_t weights[] = {1.0-u, u};
	output_curve.parameter_attributes = InputPatches.parameter_attributes.clone_types();
	k3d::table_copier param_copier(InputPatches.parameter_attributes, output_curve.parameter_attributes);
	param_copier.push_back(2, start_indices, weights);
	param_copier.push_back(2, end_indices, weights);

	output_curve.curve_attributes = InputPatches.patch_attributes.clone(Patch, Patch+1);

	output_curve.add_curve(OutputMesh, OutputCurve);
}

void extract_patch_curve_by_parameter(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurve, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::double_t U, const k3d::bool_t UDirection)
{
	if(U < 0.000001)
	{
		extract_patch_curve_by_number(OutputMesh, OutputCurve, InputMesh, InputPatches, Patch, 0, UDirection);
		return;
	}
	if(U > 0.999999)
	{
		const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
		extract_patch_curve_by_number(OutputMesh, OutputCurve, InputMesh, InputPatches, Patch, curve_count - 1, UDirection);
		return;
	}
	k3d::mesh::knots_t::const_iterator knots_begin = !UDirection ? InputPatches.patch_u_knots.begin() + InputPatches.patch_u_first_knots[Patch] : InputPatches.patch_v_knots.begin() + InputPatches.patch_v_first_knots[Patch];
	const k3d::uint_t order = !UDirection ? InputPatches.patch_u_orders[Patch] : InputPatches.patch_v_orders[Patch];
	const k3d::uint_t knot_count = !UDirection ? InputPatches.patch_u_point_counts[Patch] + order : InputPatches.patch_v_point_counts[Patch] + order;
	k3d::mesh::knots_t normalized_knots(knot_count);
	std::transform(knots_begin, knots_begin + knot_count, normalized_knots.begin(), knot_normalizer(*knots_begin, *(knots_begin + knot_count - 1)));
	const k3d::uint_t mul = multiplicity(normalized_knots, U, 0, knot_count);
	k3d::mesh tmp_mesh;
	tmp_mesh.points.create();
	tmp_mesh.point_selection.create();
	boost::scoped_ptr<k3d::nurbs_patch::primitive> tmp_patch(k3d::nurbs_patch::create(tmp_mesh));
	if(mul < (order-1))
	{
		insert_knot(tmp_mesh, *tmp_patch, InputMesh, InputPatches, Patch, U, order-mul-1, !UDirection);
	}
	else
	{
		copy_patch(tmp_mesh, *tmp_patch, InputMesh, InputPatches, Patch);
	}
	const k3d::mesh::knots_t& knots = !UDirection ? tmp_patch->patch_u_knots : tmp_patch->patch_v_knots;
	k3d::mesh::knots_t normalized_knots2(knots.size());
	std::transform(knots.begin(), knots.end(), normalized_knots2.begin(), knot_normalizer(knots.front(), knots.back()));
	const k3d::mesh::knots_t::const_iterator split_knot = std::find_if(normalized_knots2.begin(), normalized_knots2.end(), find_first_knot_after(U));
	const k3d::uint_t curve_idx = (split_knot - normalized_knots2.begin()) - order;
	extract_patch_curve_by_number(OutputMesh, OutputCurve, tmp_mesh, *tmp_patch, 0, curve_idx, UDirection);
}

// stores all curves of a patch in one direction in a curve primitive
void extract_curves(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::bool_t UDirection)
{
	OutputMesh.points.create();
	OutputMesh.point_selection.create();
	const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		extract_patch_curve_by_number(OutputMesh, OutputCurves, InputMesh, InputPatches, Patch, curve, UDirection);
	}
	if(OutputCurves.material.empty())
		OutputCurves.material.push_back(InputPatches.patch_materials[Patch]);
}

// Convert a series of curves back to a patch
void curves_to_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::primitive& InputCurves, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::bool_t UDirection)
{
	const k3d::uint_t u_order = UDirection ? InputCurves.curve_orders.front() : InputPatches.patch_u_orders[Patch];
	const k3d::uint_t v_order = UDirection ? InputPatches.patch_v_orders[Patch] : InputCurves.curve_orders.front();
	const k3d::uint_t curve_count = InputCurves.curve_first_points.size();

	if(UDirection)
	{
		const k3d::uint_t v_knots_begin = InputPatches.patch_v_first_knots[Patch];
		const k3d::uint_t v_knots_end = v_knots_begin + curve_count + v_order;
		k3d::mesh::knots_t v_knots(InputPatches.patch_v_knots.begin() + v_knots_begin, InputPatches.patch_v_knots.begin() + v_knots_end);
		skin_curves(OutputMesh, OutputPatches, InputMesh, InputCurves, v_knots, v_order);
	}
	else
	{
		patch_point_data output_point_data;
		const k3d::uint_t point_count = InputCurves.curve_point_counts.front();
		k3d::table patch_attributes;
		k3d::table parameter_attributes;
		for(k3d::uint_t i = 0; i != point_count; ++i)
		{
			for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
			{
				curve_arrays input_curve_arrays(InputMesh, InputCurves, curve);
				curve_point_data_copier copier(input_curve_arrays, output_point_data);
				copier.push_back(i);
				if(!input_curve_arrays.curve_attributes.empty())
					patch_attributes = input_curve_arrays.curve_attributes.clone();
				if(i == 0 || i == (point_count-1))
				{
					if(curve == 0 || curve == (curve_count-1))
					{
						if(parameter_attributes.empty())
							parameter_attributes = input_curve_arrays.parameter_attributes.clone_types();
						k3d::table_copier parameter_copier(input_curve_arrays.parameter_attributes, parameter_attributes);
						parameter_copier.push_back(i == 0 ? 0 : 1);
					}
				}
			}
		}
		k3d::mesh::knots_t v_knots(InputCurves.curve_knots.begin(), InputCurves.curve_knots.begin() + InputCurves.curve_point_counts.front() + InputCurves.curve_orders.front());
		const k3d::uint_t u_knots_begin = InputPatches.patch_u_first_knots[Patch];
		const k3d::uint_t u_knots_end = u_knots_begin + curve_count + u_order;
		k3d::mesh::knots_t u_knots(InputPatches.patch_u_knots.begin() + u_knots_begin, InputPatches.patch_u_knots.begin() + u_knots_end);
		add_patch(OutputMesh, OutputPatches, output_point_data, u_knots, v_knots, u_order, v_order);
		OutputPatches.patch_materials.push_back(InputPatches.patch_materials[Patch]);
		OutputPatches.patch_selections.back() = InputPatches.patch_selections[Patch];
	}
}

void elevate_patch_degree(const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::uint_t Patch, const k3d::uint_t Elevations, const k3d::bool_t UDirection)
{
	// Extract the curves
	k3d::mesh curves_mesh;
	boost::scoped_ptr<k3d::nurbs_curve::primitive> curves_prim(k3d::nurbs_curve::create(curves_mesh));
	curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	extract_curves(curves_mesh, *curves_prim, InputMesh, InputPatches, Patch, UDirection);

	// Elevate them
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_curves_prim(k3d::nurbs_curve::validate(curves_mesh, *curves_mesh.primitives.front()));
	boost::scoped_ptr<k3d::nurbs_curve::primitive> elevated_curves_prim(k3d::nurbs_curve::create(curves_mesh));
	elevated_curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		elevate_curve_degree(curves_mesh, *elevated_curves_prim, curves_mesh, *const_curves_prim, curve, Elevations);
	}

	curves_to_patch(OutputMesh, OutputPatches, curves_mesh, *elevated_curves_prim, InputPatches, Patch, UDirection);
}

void insert_knot(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::uint_t Patch, const k3d::double_t u, const k3d::uint_t r, const k3d::bool_t UDirection)
{
	// Extract the curves
	k3d::mesh curves_mesh;
	boost::scoped_ptr<k3d::nurbs_curve::primitive> curves_prim(k3d::nurbs_curve::create(curves_mesh));
	curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	extract_curves(curves_mesh, *curves_prim, InputMesh, InputPatches, Patch, UDirection);

	// Insert the new knot into every curve
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_curves_prim(k3d::nurbs_curve::validate(curves_mesh, *curves_mesh.primitives.front()));
	boost::scoped_ptr<k3d::nurbs_curve::primitive> knot_inserted_curves_prim(k3d::nurbs_curve::create(curves_mesh));
	knot_inserted_curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		insert_knot(curves_mesh, *knot_inserted_curves_prim, curves_mesh, *const_curves_prim, curve, u, r);
	}

	curves_to_patch(OutputMesh, OutputPatches, curves_mesh, *knot_inserted_curves_prim, InputPatches, Patch, UDirection);
}

void split_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, k3d::uint_t Patch, const k3d::double_t u, const k3d::bool_t UDirection)
{
	// Can't split at end
	if(u > 1 - 0.000001 || u < 0.000001)
	{
		copy_patch(OutputMesh, OutputPatches, InputMesh, InputPatches, Patch);
		return;
	}

	// Extract the curves
	k3d::mesh curves_mesh;
	boost::scoped_ptr<k3d::nurbs_curve::primitive> curves_prim(k3d::nurbs_curve::create(curves_mesh));
	curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	extract_curves(curves_mesh, *curves_prim, InputMesh, InputPatches, Patch, UDirection);

	// Split all the curves
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_curves_prim(k3d::nurbs_curve::validate(curves_mesh, *curves_mesh.primitives.front()));
	boost::scoped_ptr<k3d::nurbs_curve::primitive> split_curves_prim(k3d::nurbs_curve::create(curves_mesh));
	split_curves_prim->material.push_back(InputPatches.patch_materials[Patch]);
	const k3d::uint_t curve_count = UDirection ? InputPatches.patch_v_point_counts[Patch] : InputPatches.patch_u_point_counts[Patch];
	for(k3d::uint_t curve = 0; curve != curve_count; ++curve)
	{
		split_curve(curves_mesh, *split_curves_prim, curves_mesh, *const_curves_prim, curve, u);
	}

	if(is_closed(*const_curves_prim, 0))
	{
		curves_to_patch(OutputMesh, OutputPatches, curves_mesh, *split_curves_prim, InputPatches, Patch, UDirection);
	}
	else
	{
		assert_warning(curve_count * 2 == split_curves_prim->curve_first_points.size());
		// separate each first and second split curve into different primitives and meshes
		k3d::mesh first_mesh, second_mesh;
		first_mesh.points.create();
		first_mesh.point_selection.create();
		second_mesh.points.create();
		second_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> first_prim(k3d::nurbs_curve::create(first_mesh));
		boost::scoped_ptr<k3d::nurbs_curve::primitive> second_prim(k3d::nurbs_curve::create(second_mesh));
		boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_split_prim(k3d::nurbs_curve::validate(curves_mesh, *curves_mesh.primitives.back()));
		for(k3d::uint_t curve = 0; curve != split_curves_prim->curve_first_points.size(); ++curve)
		{
			if(curve % 2 == 0)
			{
				copy_curve(first_mesh, *first_prim, curves_mesh, *const_split_prim, curve);
			}
			else
			{
				copy_curve(second_mesh, *second_prim, curves_mesh, *const_split_prim, curve);
			}
		}
		curves_to_patch(OutputMesh, OutputPatches, first_mesh, *first_prim, InputPatches, Patch, UDirection);
		curves_to_patch(OutputMesh, OutputPatches, second_mesh, *second_prim, InputPatches, Patch, UDirection);
	}
}

void revolve_curve(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Curve, const k3d::axis Axis, const k3d::double_t Angle, const k3d::uint_t Segments)
{
	//create a circle with the given angle
	k3d::mesh::knots_t u_knots;
	k3d::mesh::weights_t u_weights;
	k3d::mesh::points_t u_control_points;
	switch (Axis)
	{
	case k3d::Z :
		k3d::nurbs_curve::circular_arc(k3d::vector3(1, 0, 0), k3d::vector3(0, 1, 0), 0, Angle, Segments, u_knots, u_weights, u_control_points);
		break;
	case k3d::X :
		k3d::nurbs_curve::circular_arc(k3d::vector3(0, 0, 1), k3d::vector3(0, 1, 0), 0, Angle, Segments, u_knots, u_weights, u_control_points);
		break;
	case k3d::Y :
		k3d::nurbs_curve::circular_arc(k3d::vector3(1, 0, 0), k3d::vector3(0, 0, 1), 0, Angle, Segments, u_knots, u_weights, u_control_points);
		break;
	}

	// New patch arrays
	patch_point_data output_points;

	curve_arrays input_curve_arrays(InputMesh, InputCurves, Curve);
	output_points.point_attributes = input_curve_arrays.point_attributes.clone_types();
	output_points.vertex_attributes = input_curve_arrays.vertex_attributes.clone_types();
	k3d::table_copier point_copier(input_curve_arrays.point_attributes, output_points.point_attributes);
	k3d::table_copier vertex_copier(input_curve_arrays.vertex_attributes, output_points.vertex_attributes);
	const k3d::uint_t curve_points_begin = 0;
	const k3d::uint_t curve_points_end = input_curve_arrays.points.size();

	for (k3d::uint_t i = curve_points_begin; i != curve_points_end; ++i)
	{
		k3d::point3 p = dehomogenize(input_curve_arrays.points[i]);
		const k3d::double_t w = input_curve_arrays.points[i][3];
		const k3d::double_t inv_w = 1.0/w;
		double distance = 1.0;
		switch (Axis)
		{
		case k3d::Z :
			distance = sqrt((p[0] * p[0]) + (p[1] * p[1])); // we want the distance to the z axis
			break;
		case k3d::X :
			distance = sqrt((p[2] * p[2]) + (p[1] * p[1])); // we want the distance to the x axis
			break;
		case k3d::Y :
			distance = sqrt((p[0] * p[0]) + (p[2] * p[2])); // we want the distance to the y axis
			break;
		}

		for (int j = 0; j < u_control_points.size(); j++)
		{
			k3d::point3 p_u;
			switch (Axis)
			{
			case k3d::Z :
				p_u = k3d::point3(u_control_points[j][0] * distance, u_control_points[j][1] * distance, p[2]);
				break;
			case k3d::X :
				p_u = k3d::point3(p[0], u_control_points[j][1] * distance, u_control_points[j][2] * distance);
				break;
			case k3d::Y :
				p_u = k3d::point3(u_control_points[j][0] * distance, p[1], u_control_points[j][2] * distance);
				break;
			}

			output_points.points.push_back(p_u);
			output_points.weights.push_back(w * u_weights[j]);
			point_copier.push_back(1, &i, &inv_w);
			vertex_copier.push_back(1, &i, &inv_w);
		}
	}

	const k3d::uint_t v_order = InputCurves.curve_orders[Curve];
	const k3d::uint_t knots_begin = InputCurves.curve_first_knots[Curve];
	const k3d::uint_t knots_end = knots_begin + v_order + InputCurves.curve_point_counts[Curve];
	k3d::mesh::knots_t v_knots(InputCurves.curve_knots.begin() + knots_begin, InputCurves.curve_knots.begin() + knots_end);

	k3d::table parameter_attributes = input_curve_arrays.parameter_attributes.clone();
	k3d::table_copier parameter_copier(input_curve_arrays.parameter_attributes, parameter_attributes);
	parameter_copier.push_back(0);
	parameter_copier.push_back(1);
	parameter_copier.push_back(1);

	add_patch(OutputMesh, OutputPatches, output_points, u_knots, v_knots, 3, v_order, input_curve_arrays.curve_attributes, parameter_attributes);
	OutputPatches.patch_materials.push_back(InputCurves.material.front());
}

void ruled_surface(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t Order, const k3d::uint_t Segments)
{
	return_if_fail(InputCurves.curve_first_points.size() == 2);
	return_if_fail(InputCurves.curve_orders.front() == InputCurves.curve_orders.back());
	return_if_fail(InputCurves.curve_point_counts.front() == InputCurves.curve_point_counts.back());

	curve_arrays input1_arrays(InputMesh, InputCurves, 0);
	curve_arrays input2_arrays(InputMesh, InputCurves, 1);

	const k3d::uint_t v_point_count = InputCurves.curve_point_counts.front();
	patch_point_data output_point_data;
	patch_point_data point_mixer;
	point_mixer.points.resize(2);
	point_mixer.weights.resize(2);
	curve_point_data_copier input1_copier(input1_arrays, point_mixer);
	curve_point_data_copier input2_copier(input2_arrays, point_mixer);
	point_data_copier output_copier(point_mixer, output_point_data);
	const k3d::uint_t indices[] = {0,1};
	for(k3d::uint_t i = 0; i != v_point_count; ++i)
	{
		input1_copier.copy(i, 0);
		input2_copier.copy(i, 1);
		const k3d::point3& p1 = InputMesh.points->at(InputCurves.curve_points[i]);
		const k3d::double_t w1 = InputCurves.curve_point_weights[i];
		const k3d::point3& p2 = InputMesh.points->at(InputCurves.curve_points[i + v_point_count]);
		const k3d::double_t w2 = InputCurves.curve_point_weights[i + v_point_count];

		const k3d::vector3 delta = (p2 - p1) / Segments;
		for (k3d::uint_t segment = 0; segment <= Segments; ++segment)
		{
			const k3d::double_t w = static_cast<double>(segment)/static_cast<double>(Segments);
			const k3d::double_t weights[] = {1.0-w, w};
			output_copier.push_back(2, indices, weights);
		}
	}

	k3d::mesh::knots_t u_knots;
	k3d::nurbs_curve::add_open_uniform_knots(Order, Segments + 1, u_knots);
	const k3d::uint_t v_order = InputCurves.curve_orders.front();
	k3d::mesh::knots_t v_knots(InputCurves.curve_knots.begin(), InputCurves.curve_knots.begin() + v_point_count + v_order);

	k3d::table parameter_attributes = input1_arrays.parameter_attributes.clone();
	k3d::table_copier parameter_copier(input2_arrays.parameter_attributes, parameter_attributes);
	parameter_copier.push_back(0);
	parameter_copier.push_back(1);
	add_patch(OutputMesh, OutputPatches, output_point_data, u_knots, v_knots, Order, v_order, input1_arrays.curve_attributes, parameter_attributes);
	OutputPatches.patch_materials.push_back(InputCurves.material.back());
}

void apply_knot_vectors(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::mesh::knots_t& UKnots, const k3d::mesh::knots_t& VKnots, const k3d::uint_t UOrder, const k3d::uint_t VOrder)
{
	// Make sure the patch is elevated to the correct degree
	k3d::mesh elevated_mesh;
	elevated_mesh.points.create();
	elevated_mesh.point_selection.create();
	boost::scoped_ptr<k3d::nurbs_patch::primitive> u_elevated(k3d::nurbs_patch::create(elevated_mesh));
	elevate_patch_degree(InputMesh, InputPatches, elevated_mesh, *u_elevated, Patch, UOrder - InputPatches.patch_u_orders[Patch], true);
	boost::scoped_ptr<k3d::nurbs_patch::const_primitive> const_u_elevated(k3d::nurbs_patch::validate(elevated_mesh, *elevated_mesh.primitives.back()));
	boost::scoped_ptr<k3d::nurbs_patch::primitive> v_elevated(k3d::nurbs_patch::create(elevated_mesh));
	elevate_patch_degree(elevated_mesh, *const_u_elevated, elevated_mesh, *v_elevated, 0, VOrder - InputPatches.patch_v_orders[Patch], false);
	boost::scoped_ptr<k3d::nurbs_patch::const_primitive> const_v_elevated(k3d::nurbs_patch::validate(elevated_mesh, *elevated_mesh.primitives.back()));
	// Adapt the U direction
	k3d::mesh u_mesh;
	u_mesh.points.create();
	u_mesh.point_selection.create();
	boost::scoped_ptr<k3d::nurbs_curve::primitive> u_curves(k3d::nurbs_curve::create(u_mesh));
	extract_curves(u_mesh, *u_curves, elevated_mesh, *const_v_elevated, 0, true);
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_u_curves(k3d::nurbs_curve::validate(u_mesh, *u_mesh.primitives.back()));
	boost::scoped_ptr<k3d::nurbs_curve::primitive> u_curves_merged(k3d::nurbs_curve::create(u_mesh));
	knot_vector_merger u_merger(UKnots, UOrder);
	for(k3d::uint_t curve = 0; curve != u_curves->curve_first_points.size(); ++curve)
		u_merger(u_mesh, *u_curves_merged, u_mesh, *const_u_curves, curve);
	u_curves_merged->material = const_u_curves->material;
	boost::scoped_ptr<k3d::nurbs_patch::primitive> u_patch_merged(k3d::nurbs_patch::create(u_mesh));
	curves_to_patch(u_mesh, *u_patch_merged, u_mesh, *u_curves_merged, *const_v_elevated, 0, true);

	// V direction
	k3d::mesh v_mesh;
	v_mesh.points.create();
	v_mesh.point_selection.create();
	boost::scoped_ptr<k3d::nurbs_curve::primitive> v_curves(k3d::nurbs_curve::create(v_mesh));
	boost::scoped_ptr<k3d::nurbs_patch::const_primitive> const_u_patch_merged(k3d::nurbs_patch::validate(u_mesh, *u_mesh.primitives.back()));
	extract_curves(v_mesh, *v_curves, u_mesh, *const_u_patch_merged, 0, false);
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_v_curves(k3d::nurbs_curve::validate(v_mesh, *v_mesh.primitives.back()));
	boost::scoped_ptr<k3d::nurbs_curve::primitive> v_curves_merged(k3d::nurbs_curve::create(v_mesh));
	knot_vector_merger v_merger(VKnots, VOrder);
	for(k3d::uint_t curve = 0; curve != v_curves->curve_first_points.size(); ++curve)
		v_merger(v_mesh, *v_curves_merged, v_mesh, *const_v_curves, curve);
	v_curves_merged->material = const_v_curves->material;
	curves_to_patch(OutputMesh, OutputPatches, v_mesh, *v_curves_merged, *const_u_patch_merged, 0, false);
}

void create_bilinear_patch(k3d::mesh& OutputMesh,
		k3d::nurbs_patch::primitive& OutputPatches,
		const patch_point_data& PointData,
		const k3d::table& PatchAttributes,
		const k3d::table& ParameterAttributes)
{
	k3d::mesh::knots_t bilinear_knots;
	bilinear_knots.push_back(0);
	bilinear_knots.push_back(0);
	bilinear_knots.push_back(1);
	bilinear_knots.push_back(1);
	add_patch(OutputMesh, OutputPatches, PointData, bilinear_knots, bilinear_knots, 2, 2, PatchAttributes, ParameterAttributes);
}

void coons_patch(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& InputCurves, const k3d::uint_t U1, const k3d::uint_t V1, const k3d::uint_t U2, const k3d::uint_t V2)
{
	patch_point_data bilinear_point_data;
	bilinear_point_data.points.push_back(InputMesh.points->at(InputCurves.curve_points[InputCurves.curve_first_points[U1]]));
	bilinear_point_data.points.push_back(InputMesh.points->at(InputCurves.curve_points[InputCurves.curve_first_points[V1]]));
	bilinear_point_data.points.push_back(InputMesh.points->at(InputCurves.curve_points[InputCurves.curve_first_points[V2]]));
	bilinear_point_data.points.push_back(InputMesh.points->at(InputCurves.curve_points[InputCurves.curve_first_points[U2]]));

	bilinear_point_data.weights.push_back(InputCurves.curve_point_weights[InputCurves.curve_first_points[U1]]);
	bilinear_point_data.weights.push_back(InputCurves.curve_point_weights[InputCurves.curve_first_points[V1]]);
	bilinear_point_data.weights.push_back(InputCurves.curve_point_weights[InputCurves.curve_first_points[V2]]);
	bilinear_point_data.weights.push_back(InputCurves.curve_point_weights[InputCurves.curve_first_points[U2]]);

	bilinear_point_data.vertex_attributes = InputCurves.vertex_attributes.clone_types();
	bilinear_point_data.point_attributes = InputMesh.point_attributes.clone_types();
	k3d::table_copier vertex_copier(InputCurves.vertex_attributes, bilinear_point_data.vertex_attributes);
	k3d::table_copier point_copier(InputMesh.point_attributes, bilinear_point_data.point_attributes);

	vertex_copier.push_back(InputCurves.curve_first_points[U1]);
	vertex_copier.push_back(InputCurves.curve_first_points[V1]);
	vertex_copier.push_back(InputCurves.curve_first_points[U2]);
	vertex_copier.push_back(InputCurves.curve_first_points[V2]);

	point_copier.push_back(InputCurves.curve_points[InputCurves.curve_first_points[U1]]);
	point_copier.push_back(InputCurves.curve_points[InputCurves.curve_first_points[V1]]);
	point_copier.push_back(InputCurves.curve_points[InputCurves.curve_first_points[U2]]);
	point_copier.push_back(InputCurves.curve_points[InputCurves.curve_first_points[V2]]);

	k3d::table patch_attributes = InputCurves.curve_attributes.clone_types();
	k3d::table_copier patch_copier(InputCurves.curve_attributes, patch_attributes);
	k3d::uint_t curve_indices[] = {U1, V1, U2, V2};
	k3d::double_t curve_weights[] = {0.5, 0.5, 0.5, 0.5};
	patch_copier.push_back(4, curve_indices, curve_weights);

	k3d::table parameter_attributes = InputCurves.parameter_attributes.clone_types();
	if(!parameter_attributes.empty())
	{
		k3d::table_copier parameter_copier(InputCurves.parameter_attributes, parameter_attributes);
		k3d::uint_t param_idx0[] = {2*U1, 2*U2};
		k3d::double_t param_weights[] = {0.5, 0.5};
		parameter_copier.push_back(2, param_idx0, param_weights);
		k3d::uint_t param_idx1[] = {2*U1+1, 2*U2+1};
		parameter_copier.push_back(2, param_idx1, param_weights);
		k3d::uint_t param_idx2[] = {2*V1, 2*V2};
		parameter_copier.push_back(2, param_idx2, param_weights);
		k3d::uint_t param_idx3[] = {2*V1+1, 2*V2+1};
		parameter_copier.push_back(2, param_idx3, param_weights);
	}

	// Storage for temporary patches
	k3d::mesh patch_mesh;
	patch_mesh.points.create();
	patch_mesh.point_selection.create();

	boost::scoped_ptr<k3d::nurbs_patch::primitive> bilinear_patch(k3d::nurbs_patch::create(patch_mesh));
	create_bilinear_patch(patch_mesh, *bilinear_patch, bilinear_point_data, patch_attributes, parameter_attributes);
	bilinear_patch->patch_materials.push_back(InputCurves.material.back());

	// Flip curve U2 and V2
	k3d::mesh flipped_mesh;
	flipped_mesh.points.create();
	flipped_mesh.point_selection.create();
	boost::scoped_ptr<k3d::nurbs_curve::primitive> flipped_curves(k3d::nurbs_curve::create(flipped_mesh));
	copy_curve(flipped_mesh, *flipped_curves, InputMesh, InputCurves, U2);
	copy_curve(flipped_mesh, *flipped_curves, InputMesh, InputCurves, V2);
	flipped_curves->material = InputCurves.material;
	flip_curve(*flipped_curves, 0);
	flip_curve(*flipped_curves, 1);
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_flipped_curves(k3d::nurbs_curve::validate(flipped_mesh, *flipped_mesh.primitives.front()));

	// Make sure all curves are compatible
	k3d::mesh u_compatible_mesh;
	u_compatible_mesh.points.create();
	u_compatible_mesh.point_selection.create();
	boost::scoped_ptr<k3d::nurbs_curve::primitive> u_compatible_curves(k3d::nurbs_curve::create(u_compatible_mesh));
	const k3d::double_t u_order = std::max(InputCurves.curve_orders[U1], InputCurves.curve_orders[U2]);
	k3d::mesh::knots_t common_u_knots;
	knot_vector_calculator u_knot_extractor(common_u_knots);
	u_knot_extractor(InputMesh, InputCurves, U1);
	u_knot_extractor(flipped_mesh, *const_flipped_curves, 0);
	knot_vector_merger u_merger(common_u_knots, u_order);
	u_merger(u_compatible_mesh, *u_compatible_curves, InputMesh, InputCurves, U1);
	u_merger(u_compatible_mesh, *u_compatible_curves, flipped_mesh, *const_flipped_curves, 0);

	k3d::mesh v_compatible_mesh;
	v_compatible_mesh.points.create();
	v_compatible_mesh.point_selection.create();
	boost::scoped_ptr<k3d::nurbs_curve::primitive> v_compatible_curves(k3d::nurbs_curve::create(v_compatible_mesh));
	const k3d::double_t v_order = std::max(InputCurves.curve_orders[V1], InputCurves.curve_orders[U2]);
	k3d::mesh::knots_t common_v_knots;
	knot_vector_calculator v_knot_extractor(common_v_knots);
	v_knot_extractor(flipped_mesh, *const_flipped_curves, 1);
	v_knot_extractor(InputMesh, InputCurves, V1);
	knot_vector_merger v_merger(common_v_knots, v_order);
	v_merger(v_compatible_mesh, *v_compatible_curves, flipped_mesh, *const_flipped_curves, 1);
	v_merger(v_compatible_mesh, *v_compatible_curves, InputMesh, InputCurves, V1);

	u_compatible_curves->material = InputCurves.material;
	v_compatible_curves->material = InputCurves.material;

	// ruled surfaces
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_u_compatible_curves(k3d::nurbs_curve::validate(u_compatible_mesh, *u_compatible_mesh.primitives.front()));
	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> const_v_compatible_curves(k3d::nurbs_curve::validate(v_compatible_mesh, *v_compatible_mesh.primitives.front()));
	boost::scoped_ptr<k3d::nurbs_patch::primitive> u_ruled(k3d::nurbs_patch::create(patch_mesh));
	ruled_surface(patch_mesh, *u_ruled, u_compatible_mesh, *const_u_compatible_curves, 2, 1);
	boost::scoped_ptr<k3d::nurbs_patch::primitive> v_ruled(k3d::nurbs_patch::create(patch_mesh));
	ruled_surface(patch_mesh, *v_ruled, v_compatible_mesh, *const_v_compatible_curves, 2, 1);

	// Make all surfaces compatible
	boost::scoped_ptr<k3d::nurbs_patch::const_primitive> const_bilinear(k3d::nurbs_patch::validate(patch_mesh, *patch_mesh.primitives[0]));
	boost::scoped_ptr<k3d::nurbs_patch::const_primitive> const_u_ruled(k3d::nurbs_patch::validate(patch_mesh, *patch_mesh.primitives[1]));
	boost::scoped_ptr<k3d::nurbs_patch::const_primitive> const_v_ruled(k3d::nurbs_patch::validate(patch_mesh, *patch_mesh.primitives[2]));
	boost::scoped_ptr<k3d::nurbs_patch::primitive> bilinear_compatible(k3d::nurbs_patch::create(patch_mesh));
	boost::scoped_ptr<k3d::nurbs_patch::primitive> u_ruled_compatible(k3d::nurbs_patch::create(patch_mesh));
	boost::scoped_ptr<k3d::nurbs_patch::primitive> v_ruled_compatible(k3d::nurbs_patch::create(patch_mesh));
	apply_knot_vectors(patch_mesh, *bilinear_compatible, patch_mesh, *const_bilinear, 0, common_u_knots, common_v_knots, u_order, v_order);
	apply_knot_vectors(patch_mesh, *u_ruled_compatible, patch_mesh, *const_u_ruled, 0, common_v_knots, common_u_knots, u_order, v_order);
	apply_knot_vectors(patch_mesh, *v_ruled_compatible, patch_mesh, *const_v_ruled, 0, common_u_knots, common_v_knots, v_order, u_order);

	// Compose the final patch
	patch_point_data output_point_data;
	patch_point_data mixer(3); // data from a single point of the 3 patches
	mixer.point_attributes = patch_mesh.point_attributes.clone_types();
	mixer.vertex_attributes = u_ruled_compatible->vertex_attributes.clone_types();
	mixer.point_attributes.set_row_count(3);
	mixer.vertex_attributes.set_row_count(3);
	// copiers
	k3d::table_copier patch_point_copier(patch_mesh.point_attributes, mixer.point_attributes);
	k3d::table_copier u_vertex_copier(u_ruled_compatible->vertex_attributes, mixer.vertex_attributes);
	k3d::table_copier v_vertex_copier(v_ruled_compatible->vertex_attributes, mixer.vertex_attributes);
	k3d::table_copier bilinear_vertex_copier(bilinear_compatible->vertex_attributes, mixer.vertex_attributes);
	const k3d::uint_t u_count = bilinear_compatible->patch_u_point_counts.back();
	const k3d::uint_t v_count = bilinear_compatible->patch_v_point_counts.back();
	// output copying
	point_data_copier output_copier(mixer, output_point_data);
	const k3d::uint_t indices[] = {0, 1, 2};
	const k3d::double_t weights[] = {1.0, 1.0, -1.0};
	for(k3d::uint_t v = 0; v != v_count; ++v)
	{
		const k3d::uint_t u_start = u_count * v;
		for(k3d::uint_t u = 0; u != u_count; ++u)
		{
			const k3d::uint_t idx = u_start + u;
			const k3d::uint_t transposed_idx = v + u*v_count;

			patch_point_copier.copy(u_ruled_compatible->patch_points[transposed_idx], 0);
			patch_point_copier.copy(v_ruled_compatible->patch_points[idx], 1);
			patch_point_copier.copy(bilinear_compatible->patch_points[idx], 2);

			u_vertex_copier.copy(transposed_idx, 0);
			v_vertex_copier.copy(idx, 1);
			bilinear_vertex_copier.copy(idx, 2);

			mixer.points[0] = patch_mesh.points->at(u_ruled_compatible->patch_points[transposed_idx]);
			mixer.points[1] = patch_mesh.points->at(v_ruled_compatible->patch_points[idx]);
			mixer.points[2] = patch_mesh.points->at(bilinear_compatible->patch_points[idx]);

			mixer.weights[0] = u_ruled_compatible->patch_point_weights[transposed_idx];
			mixer.weights[1] = v_ruled_compatible->patch_point_weights[idx];
			mixer.weights[2] = bilinear_compatible->patch_point_weights[idx];

			output_copier.push_back(3, indices, weights);
		}
	}
	add_patch(OutputMesh, OutputPatches, output_point_data, common_u_knots, common_v_knots, u_order, v_order, patch_attributes, parameter_attributes);
	OutputPatches.patch_materials.push_back(InputCurves.material.back());
}

void extract_plane(k3d::point3& Origin, k3d::normal3& Normal, k3d::vector3& U, k3d::vector3& V, k3d::mesh::points_2d_t& ParameterPoints, const k3d::mesh::points_t& Points, const k3d::double_t UOffset)
{
	// The origin is the centroid of the point cloud
	Origin = k3d::point3(0,0,0);
	const k3d::uint_t points_count = Points.size();
	for(k3d::uint_t i = 0; i != points_count; ++i)
		Origin += k3d::to_vector(Points[i]);
	Origin /= points_count;

	// Extract the average normal (from polyhedron.cpp)
	Normal = k3d::normal3(0,0,0);
	for(k3d::uint_t p = 0; p != points_count; ++p)
	{
		const k3d::point3& i = Points[p];
		const k3d::point3& j = Points[(p+1)%points_count];

		Normal[0] += (i[1] + j[1]) * (j[2] - i[2]);
		Normal[1] += (i[2] + j[2]) * (j[0] - i[0]);
		Normal[2] += (i[0] + j[0]) * (j[1] - i[1]);
	}
	Normal = k3d::normalize(Normal);

	// As U-axis, we use the first point in the cloud, rotated by an angle depending on the UOffset parameter
	k3d::point4 u_point(Points.front()[0], Points.front()[1], Points.front()[2], 1);
	k3d::point4 u_point_rotated = k3d::rotate3(k3d::pi_times_2()*UOffset, k3d::to_vector(Normal)) * u_point;
	U = k3d::normalize(k3d::point3(u_point_rotated[0], u_point_rotated[1], u_point_rotated[2]) - Origin);

	// The V axis is the cross product of the Normal and U
	V = Normal ^ U;

	// Now we can extract the parameter coords
	for(k3d::uint_t i = 0; i != points_count; ++i)
	{
		const k3d::vector3 vec = Points[i] - Origin;
		ParameterPoints.push_back(k3d::point2(U*vec, V*vec));
	}
}

void bbox2(k3d::point2& Min, k3d::point2& Max, const k3d::mesh::points_2d_t& Points)
{
	Min = k3d::point2(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	Max = k3d::point2(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
	for(k3d::uint_t i = 0; i != Points.size(); ++i)
	{
		const k3d::point2& p = Points[i];
		Min[0] = std::min(p[0], Min[0]);
		Min[1] = std::min(p[1], Min[1]);
		Max[0] = std::max(p[0], Max[0]);
		Max[1] = std::max(p[1], Max[1]);
	}
}

void skin_curves(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::primitive& InputCurves, const k3d::mesh::knots_t VKnots, const k3d::uint_t VOrder)
{
	const k3d::uint_t u_order = InputCurves.curve_orders.front();
	const k3d::uint_t curve_count = InputCurves.curve_first_points.size();

	patch_point_data output_point_data;

	// Because of the way elevate_curve_degree (and ultimately add_curve) works, the points should be ordered correctly
	output_point_data.points.insert(output_point_data.points.end(), InputMesh.points->begin() + InputCurves.curve_points[InputCurves.curve_first_points.front()], InputMesh.points->end());
	output_point_data.weights = InputCurves.curve_point_weights;
	output_point_data.vertex_attributes = InputCurves.vertex_attributes.clone();
	output_point_data.point_attributes = InputMesh.point_attributes.clone(InputCurves.curve_points[InputCurves.curve_first_points.front()], InputMesh.points->size());
	k3d::mesh::knots_t u_knots(InputCurves.curve_knots.begin(), InputCurves.curve_knots.begin() + InputCurves.curve_point_counts.front() + InputCurves.curve_orders.front());

	k3d::table patch_attributes = InputCurves.curve_attributes.clone(0, 1);
	k3d::table parameter_attributes = InputCurves.parameter_attributes.clone_types();
	k3d::table_copier parameter_copier(InputCurves.parameter_attributes, parameter_attributes);
	parameter_copier.push_back(0);
	parameter_copier.push_back(1);
	parameter_copier.push_back(2*InputCurves.curve_first_points.size()-2);
	parameter_copier.push_back(2*InputCurves.curve_first_points.size()-1);

	add_patch(OutputMesh, OutputPatches, output_point_data, u_knots, VKnots, u_order, VOrder, patch_attributes, parameter_attributes);
	OutputPatches.patch_materials.push_back(InputCurves.material.back());
	OutputPatches.patch_selections.back() = 1.0;
}

void sweep(k3d::mesh& OutputMesh, k3d::nurbs_patch::primitive& OutputPatches, const k3d::mesh& InputMesh, const k3d::nurbs_curve::const_primitive& SweptCurves, const k3d::nurbs_curve::const_primitive& Paths, const k3d::uint_t Samples, const k3d::bool_t AlignNormal)
{
	const k3d::uint_t paths_count = Paths.curve_first_points.size();
	const k3d::uint_t swept_curves_count = SweptCurves.curve_first_points.size();
	for(k3d::uint_t path = 0; path != paths_count; ++path)
	{
		curve_arrays path_arrays(InputMesh, Paths, path, true);
		const k3d::uint_t path_point_count = path_arrays.points.size();
		const k3d::uint_t order = path_arrays.order;

		// Unique knot values
		k3d::mesh::knots_t unique_knots;
		for(k3d::uint_t i = 0; i < path_arrays.knots.size();)
		{
			const k3d::double_t u = path_arrays.knots[i];
			unique_knots.push_back(u);
			i += multiplicity(path_arrays.knots, u, i, path_arrays.knots.size());
		}

		// Construct a local coordinate system around each sample point
		points4_t origins;
		k3d::mesh::vectors_t x_vecs, y_vecs, z_vecs;
		k3d::mesh::knots_t samples_u;
		{
			origins.push_back(path_arrays.points.front());
			const k3d::point3 p0 = dehomogenize(path_arrays.points[0]);
			const k3d::point3 p1 = dehomogenize(path_arrays.points[1]);
			const k3d::point3 p2 = dehomogenize(path_arrays.points[2]);
			const k3d::vector3 z = k3d::normalize(p1 - p0);
			const k3d::double_t step = (path_arrays.knots[order] - path_arrays.knots[0]) / static_cast<k3d::double_t>(Samples+1);
			k3d::vector3 next_tangent = path_arrays.tangent(0.5*step);
			if(std::abs(z*next_tangent) > 0.999999)
			{
				next_tangent = path_point_count > 2 ? k3d::normalize(p2 - p1) : z;
				if(std::abs(z*next_tangent) > 0.999999) // Choose a random vector that is not parallel to z to make a cross product with
				{
					if(z[0] != 0)
						next_tangent[1] += 1;
					else
						next_tangent[0] += 1;
					next_tangent  = k3d::normalize(next_tangent);
				}
			}
			const k3d::vector3 y = k3d::normalize(z ^ next_tangent);
			x_vecs.push_back(k3d::normalize(y ^ z));
			y_vecs.push_back(y);
			z_vecs.push_back(z);
			samples_u.push_back(0);
		}
		const k3d::uint_t unique_knots_end = unique_knots.size() - 1;
		for(k3d::uint_t i = 0; i != unique_knots_end; ++i)
		{
			const k3d::double_t start_u = unique_knots[i];
			const k3d::double_t step = (unique_knots[i+1] - start_u) / static_cast<k3d::double_t>(Samples + 1);
			for(k3d::uint_t sample = 1; sample <= (Samples+1); ++sample)
			{
				const k3d::double_t u = start_u + static_cast<k3d::double_t>(sample) * step;
				samples_u.push_back(u);
				const k3d::point4 origin = path_arrays.evaluate(u).weighted_position;
				k3d::vector3 z = path_arrays.tangent(u, 0.5*step);
				const k3d::uint_t mul = multiplicity(path_arrays.knots, u, 0, path_arrays.knots.size());
				k3d::double_t scaling = 1.0;
				if(mul == order-1)
				{
					z = k3d::normalize(0.5*(z + z_vecs.back()));
					scaling = std::abs(z_vecs.back() * z);
				}
				const k3d::vector3& last_z = z_vecs.back(); // We project the coordinate system along the last z axis
				const k3d::point3 last_y_point = dehomogenize(origins.back()) + y_vecs.back();
				const k3d::double_t d = -((last_y_point - dehomogenize(origin))*z) / (last_z * z); // Projection distance
				const k3d::vector3 y = k3d::normalize((last_y_point + d*last_z) - dehomogenize(origin));
				const k3d::vector3 x = k3d::normalize(y ^ z) / scaling;
				origins.push_back(origin);
				x_vecs.push_back(x);
				y_vecs.push_back(y);
				z_vecs.push_back(z);
			}
		}
		// Closed curve, make sure the vectors are the same
		if(origins.front() == origins.back())
		{
			const k3d::vector3 y = k3d::normalize(y_vecs.front() + y_vecs.back());
			const k3d::vector3 z = k3d::normalize(z_vecs.front() + z_vecs.back());
			const k3d::double_t scaling = std::abs(z * z_vecs.front());
			const k3d::vector3 x = k3d::normalize(x_vecs.front() + x_vecs.back()) / scaling;
			x_vecs.front() = x;
			x_vecs.back() = x;
			y_vecs.front() = y;
			y_vecs.back() = y;
			z_vecs.front() = z;
			z_vecs.back() = z;
		}

		for(k3d::uint_t swept_curve = 0; swept_curve != swept_curves_count; ++swept_curve)
		{
			// Store the control points of the curve to sweep in a format relative to the coordinat system around the path's first point
			k3d::mesh::points_t relative_points;
			k3d::vector3 x,y,z;
			k3d::point4 p = origins.front();
			k3d::double_t w = p[3];
			k3d::point3 origin(p[0]/w, p[1]/w, p[2]/w);
			x = k3d::normalize(x_vecs.front());
			y = y_vecs.front();
			z = z_vecs.front();
			k3d::mesh::points_t swept_points;
			k3d::mesh::weights_t swept_weights;
			curve_arrays swept_arrays(InputMesh, SweptCurves, swept_curve, true);
			swept_arrays.points3(swept_points);
			swept_arrays.weights(swept_weights);
			const k3d::uint_t swept_point_count = swept_arrays.points.size();
			if(AlignNormal)
			{
				k3d::point3 center(0,0,0);
				for(k3d::uint_t i = 0; i != (swept_point_count-1); ++i)
				{
					center += k3d::to_vector(swept_points[i]);
				}
				center /= (swept_point_count-1);
				k3d::normal3 normal(0,0,0); // Normal calculation from polyhedron.cpp
				for(k3d::uint_t point = 0; point != swept_point_count; ++point)
				{
					const k3d::point3& i = swept_points[point];
					const k3d::point3& j = swept_points[point == (swept_point_count-1) ? 0 : point + 1];

					normal[0] += (i[1] + j[1]) * (j[2] - i[2]);
					normal[1] += (i[2] + j[2]) * (j[0] - i[0]);
					normal[2] += (i[0] + j[0]) * (j[1] - i[1]);
				}
				normal = normal.length2() ? k3d::normalize(normal) : k3d::normal3(0,0,1);
				const k3d::vector3& path_tangent = z_vecs.front();
				const k3d::vector3& axis = normal ^ path_tangent;
				const k3d::double_t angle = acos(path_tangent*normal);
				k3d::matrix4 rotation = std::isnan(angle) ? k3d::identity3() : k3d::rotate3(angle, axis);
				for(k3d::uint_t i = 0; i != swept_point_count; ++i)
				{
					const k3d::vector3 vec = rotation * (swept_points[i] - center);
					relative_points.push_back(k3d::point3(vec*x, vec*y, vec*z));
				}
			}
			else
			{
				for(k3d::uint_t i = 0; i != swept_point_count; ++i)
				{
					const k3d::vector3 vec = swept_points[i] - origin;
					relative_points.push_back(k3d::point3(vec*x, vec*y, vec*z));
				}
			}
			k3d::mesh curves_mesh;
			curves_mesh.points.create();
			curves_mesh.point_selection.create();
			boost::scoped_ptr<k3d::nurbs_curve::primitive> curves_prim(k3d::nurbs_curve::create(curves_mesh));
			curves_prim->material = SweptCurves.material;
			for(k3d::uint_t j = 0; j != relative_points.size(); ++j)
			{
				const k3d::point3& rel_point = relative_points[j];
				points4_t samples;
				for(k3d::uint_t i = 0; i != origins.size(); ++i)
				{
					p = origins[i];
					w = p[3];
					origin = k3d::point3(p[0]/w, p[1]/w, p[2]/w);
					x = x_vecs[i];
					y = y_vecs[i];
					z = z_vecs[i];
					const k3d::vector3 vec = x*rel_point[0] + y*rel_point[1] + z*rel_point[2];
					const k3d::point3 out = origin + vec;
					samples.push_back(k3d::point4(out[0]*w, out[1]*w, out[2]*w, w));
				}
				k3d::mesh::points_t points_out;
				k3d::mesh::weights_t weights_out;
				approximate(points_out, weights_out, samples_u, samples, order, path_arrays.knots);
				const k3d::double_t swept_w = swept_weights[j];
				for(k3d::uint_t i = 0; i != weights_out.size(); ++i)
					weights_out[i] *= swept_w;
				return_if_fail(points_out.size());
				k3d::nurbs_curve::add_curve(curves_mesh, *curves_prim, order, points_out, weights_out, path_arrays.knots);
			}
			skin_curves(OutputMesh, OutputPatches, curves_mesh, *curves_prim, swept_arrays.knots, SweptCurves.curve_orders[swept_curve]);
		}
	}
}

void polygonize(k3d::mesh::points_t& Vertices, k3d::mesh::counts_t& VertexCounts, k3d::mesh::indices_t& VertexIndices, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::uint_t USamples, const k3d::uint_t VSamples)
{
	const k3d::uint_t u_order = InputPatches.patch_u_orders[Patch];
	const k3d::uint_t u_knots_begin = InputPatches.patch_u_first_knots[Patch];
	const k3d::uint_t u_knots_end = u_knots_begin + InputPatches.patch_u_point_counts[Patch] + u_order;
	const k3d::mesh::knots_t::const_iterator u_it = InputPatches.patch_u_knots.begin();
	k3d::mesh::knots_t u_knots(u_it + u_knots_begin, u_it + u_knots_end);
	k3d::mesh::knots_t u_samples;
	sample(u_samples, u_knots, USamples);

	const k3d::uint_t v_order = InputPatches.patch_v_orders[Patch];
	const k3d::uint_t v_knots_begin = InputPatches.patch_v_first_knots[Patch];
	const k3d::uint_t v_knots_end = v_knots_begin + InputPatches.patch_v_point_counts[Patch] + v_order;
	const k3d::mesh::knots_t::const_iterator v_it = InputPatches.patch_v_knots.begin();
	k3d::mesh::knots_t v_knots(v_it + v_knots_begin, v_it + v_knots_end);
	std::transform(v_knots.begin(), v_knots.end(), v_knots.begin(), knot_normalizer(v_knots.front(), v_knots.back()));
	k3d::mesh::knots_t v_samples;
	sample(v_samples, v_knots, VSamples);

	const k3d::uint_t u_sample_count = u_samples.size();
	const k3d::uint_t v_sample_count = v_samples.size();

	for(k3d::uint_t j = 0; j != v_sample_count; ++j)
	{
		const k3d::double_t v = v_samples[j];
		k3d::mesh curve_mesh;
		curve_mesh.points.create();
		curve_mesh.point_selection.create();
		boost::scoped_ptr<k3d::nurbs_curve::primitive> curve(k3d::nurbs_curve::create(curve_mesh));
		extract_patch_curve_by_parameter(curve_mesh, *curve, InputMesh, InputPatches, Patch, v, true);
		curve_arrays curve_arr(curve_mesh, *curve, 0);
		for(k3d::uint_t i = 0; i != u_sample_count; ++i)
		{
			const k3d::double_t u = u_samples[i];
			Vertices.push_back(curve_arr.evaluate(u).position());
		}
	}
	for(k3d::uint_t j = 0; j != v_sample_count-1; ++j)
	{
		for(k3d::uint_t i = 0; i != u_sample_count-1; ++i)
		{
			VertexIndices.push_back(j*u_sample_count+i);
			VertexIndices.push_back(j*u_sample_count+i+1);
			VertexIndices.push_back((j+1)*u_sample_count+i+1);
			VertexIndices.push_back((j+1)*u_sample_count+i);
			VertexCounts.push_back(4);
		}
	}
}

void extract_trim_curve(k3d::mesh::points_t& Points, k3d::mesh::weights_t& Weights, k3d::mesh::knots_t& Knots, const k3d::nurbs_patch::const_primitive& NurbsPatches, const k3d::uint_t Patch, const k3d::uint_t Curve)
{
	const k3d::uint_t points_begin = NurbsPatches.curve_first_points[Curve];
	const k3d::uint_t points_count = NurbsPatches.curve_point_counts[Curve];
	const k3d::uint_t points_end = points_begin + points_count;
	for(k3d::uint_t point = points_begin; point != points_end; ++point)
	{
		const k3d::point2& p = NurbsPatches.points[NurbsPatches.curve_points[point]];
		Points.push_back(k3d::point3(p[0], p[1], 0));
		Weights.push_back(NurbsPatches.curve_point_weights[point]);
	}
	const k3d::uint_t knots_begin = NurbsPatches.curve_first_knots[Curve];
	const k3d::uint_t knots_end = knots_begin + points_count + NurbsPatches.curve_orders[Curve];
	for(k3d::uint_t k = knots_begin; k != knots_end; ++k)
	{
		Knots.push_back(NurbsPatches.curve_knots[k]);
	}
}

const k3d::point4 evaluate_position(const k3d::mesh& Mesh, const k3d::nurbs_patch::const_primitive& Patches, const k3d::uint_t Patch, const k3d::double_t U, const k3d::double_t V)
{
	k3d::mesh curve_mesh;
	curve_mesh.points.create();
	curve_mesh.point_selection.create();
	boost::scoped_ptr<k3d::nurbs_curve::primitive> curve_prim(k3d::nurbs_curve::create(curve_mesh));
	extract_patch_curve_by_parameter(curve_mesh, *curve_prim, Mesh, Patches, Patch, U, false);
	k3d::mesh::points_t curve_points;
	k3d::mesh::weights_t curve_weights;
	k3d::mesh::knots_t curve_knots;
	k3d::table curve_point_attributes;
	curve_arrays curve(curve_mesh, *curve_prim, 0, true);
	return curve.evaluate(V).weighted_position;
}

void trim_to_nurbs(k3d::mesh& OutputMesh, k3d::nurbs_curve::primitive& OutputCurves, const k3d::mesh& InputMesh, const k3d::nurbs_patch::const_primitive& InputPatches, const k3d::uint_t Patch, const k3d::uint_t Samples)
{
	const k3d::uint_t loops_begin = InputPatches.patch_first_trim_loops[Patch];
	const k3d::uint_t loops_end = InputPatches.patch_trim_loop_counts[Patch] + loops_begin;
	const k3d::uint_t u_order = InputPatches.patch_u_orders[Patch];
	const k3d::uint_t v_order = InputPatches.patch_v_orders[Patch];
	const k3d::double_t umin = InputPatches.patch_u_knots[InputPatches.patch_u_first_knots[Patch]];
	const k3d::double_t urange = InputPatches.patch_u_knots[InputPatches.patch_u_first_knots[Patch] + InputPatches.patch_u_point_counts[Patch] + u_order - 1] - umin;
	const k3d::double_t vmin = InputPatches.patch_v_knots[InputPatches.patch_v_first_knots[Patch]];
	const k3d::double_t vrange = InputPatches.patch_v_knots[InputPatches.patch_v_first_knots[Patch] + InputPatches.patch_v_point_counts[Patch] + v_order - 1] - vmin;
	for(k3d::uint_t loop = loops_begin; loop != loops_end; ++loop)
	{
		const k3d::uint_t curves_begin = InputPatches.trim_loop_first_curves[loop];
		const k3d::uint_t curves_end = curves_begin + InputPatches.trim_loop_curve_counts[loop];
		for(k3d::uint_t curve = curves_begin; curve != curves_end; ++curve)
		{
			k3d::mesh::points_t trim_points;
			k3d::mesh::weights_t trim_weights;
			k3d::mesh::knots_t trim_knots;
			const k3d::uint_t order = InputPatches.curve_orders[curve];
			extract_trim_curve(trim_points, trim_weights, trim_knots, InputPatches, Patch, curve);
			k3d::mesh tmp_mesh;
			tmp_mesh.points.create();
			tmp_mesh.point_selection.create();
			boost::scoped_ptr<k3d::nurbs_curve::primitive> tmp_curve(k3d::nurbs_curve::create(tmp_mesh));
			boost::scoped_ptr<k3d::nurbs_curve::primitive> elevated_curves(k3d::nurbs_curve::create(tmp_mesh));
			tmp_curve->material.push_back(0);
			elevated_curves->material.push_back(0);
			k3d::nurbs_curve::add_curve(tmp_mesh, *tmp_curve, order, trim_points, trim_weights, trim_knots);
			const k3d::uint_t max_order = std::max(u_order,v_order);
			const k3d::uint_t elevations = order < max_order ? max_order - order : 0;
			elevate_curve_degree(tmp_mesh, *elevated_curves, tmp_mesh, *tmp_curve, 0, elevations);
			curve_arrays el_curve(tmp_mesh, *elevated_curves, 0, true);
			k3d::mesh::knots_t u_samples;
			sample(u_samples, el_curve.knots, Samples);
			const k3d::uint_t samples_end = u_samples.size();
			points4_t sample_points;
			for(k3d::uint_t sample = 0; sample != samples_end; ++sample)
			{
				curve_arrays::curve_value val = el_curve.evaluate(u_samples[sample]);
				const k3d::point3 p = val.position();
				sample_points.push_back(val.weight() * evaluate_position(InputMesh, InputPatches, Patch, (p[0]-umin)/urange, (p[1]-vmin)/vrange));
			}
			k3d::mesh::points_t curve_points;
			k3d::mesh::weights_t curve_weights;
			approximate(curve_points, curve_weights, u_samples, sample_points, max_order, el_curve.knots);
			return_if_fail(curve_points.size());
			k3d::nurbs_curve::add_curve(OutputMesh, OutputCurves, max_order, curve_points, curve_weights, el_curve.knots);
			if(OutputCurves.material.empty())
				OutputCurves.material.push_back(InputPatches.patch_materials[Patch]);
		}
	}
}

} //namespace nurbs

} //namespace module
