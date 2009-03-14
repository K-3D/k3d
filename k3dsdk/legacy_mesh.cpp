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

#include "gl.h"
#include "legacy_mesh.h"
#include "mesh.h"
#include "nurbs_curve.h"
#include "nurbs_patch.h"
#include "polyhedron.h"
#include "result.h"
#include "utility.h"

#include <boost/scoped_ptr.hpp>

#include <algorithm>

namespace k3d
{

namespace legacy
{
	
/////////////////////////////////////////////////////////////////////////////
// point


point::point(const point3& Position) :
	position(Position)
{
}

point::point(const double X, const double Y, const double Z) :
	position(X, Y, Z)
{
}

/////////////////////////////////////////////////////////////////////////////
// split_edge

split_edge::~split_edge()
{
	// Acknowledge companion he's now alone
	if(companion)
		companion->companion = 0;
}

/////////////////////////////////////////////////////////////////////////////
// face

face::face(split_edge* FirstEdge, imaterial* Material) :
	first_edge(FirstEdge),
	material(Material)
{
}

face::~face()
{
	for(k3d::legacy::split_edge* edge = first_edge; edge; )
	{
		k3d::legacy::split_edge* next_edge = edge->face_clockwise;

		delete edge;
		if(next_edge == first_edge)
			break;

		edge = next_edge;
	}

	for(holes_t::iterator hole = holes.begin(); hole != holes.end(); ++hole)
	{
		for(k3d::legacy::split_edge* edge = *hole; edge; )
		{
			k3d::legacy::split_edge* next_edge = edge->face_clockwise;

			delete edge;
			if(next_edge == *hole)
				break;

			edge = next_edge;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// normal

normal3 normal(const split_edge* const Loop)
{
	/// Calculates the normal for an edge loop using the summation method, which is more robust than the three-point methods (handles zero-length edges)
	normal3 result(0, 0, 0);

	for(const split_edge* edge = Loop; edge && edge->face_clockwise; edge = edge->face_clockwise)
	{
		const point3& i = edge->vertex->position;
		const point3& j = edge->face_clockwise->vertex->position;

		result[0] += (i[1] + j[1]) * (j[2] - i[2]);
		result[1] += (i[2] + j[2]) * (j[0] - i[0]);
		result[2] += (i[0] + j[0]) * (j[1] - i[1]);

		if(Loop == edge->face_clockwise)
			break;
	}

	return 0.5 * result;
}

normal3 normal(const face& Face)
{
	return normal(Face.first_edge);
}

/////////////////////////////////////////////////////////////////////////////
// polyhedron

polyhedron::polyhedron() :
	type(POLYGONS)
{
}

polyhedron::~polyhedron()
{
	std::for_each(faces.begin(), faces.end(), delete_object());
}

std::ostream& operator<<(std::ostream& Stream, const polyhedron::type_t& RHS)
{
	switch(RHS)
	{
		case polyhedron::POLYGONS:
			Stream << "polygons";
			break;
		case polyhedron::CATMULL_CLARK_SUBDIVISION_MESH:
			Stream << "catmull_clark";
			break;
	}

	return Stream;
}

std::istream& operator>>(std::istream& Stream, polyhedron::type_t& RHS)
{
	std::string buffer;
	Stream >> buffer;

	if(buffer == "polygons")
		RHS = polyhedron::POLYGONS;
	else if(buffer == "catmull_clark")
		RHS = polyhedron::CATMULL_CLARK_SUBDIVISION_MESH;
	else
		log() << error << "Unknown polyhedron type [" << buffer << "]" << std::endl;

	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// set_companions

namespace detail
{

/// std::pair equivalent for edges that maintains the order of its members
template<typename T1, typename T2>
class ordered_edge;

template<typename T1, typename T2>
bool operator<(const ordered_edge<T1,T2>& lhs, const ordered_edge<T1,T2>& rhs);

template<typename T1, typename T2>
class ordered_edge :
	public std::pair<T1, T2>
{
public:
	typedef T1 first_type;
	typedef T2 second_type;

	bool ordered;
	T1 first;
	T2 second;

	explicit ordered_edge()
	{
	}

	explicit ordered_edge(const k3d::legacy::split_edge* Edge) :
		ordered(Edge->vertex < Edge->face_clockwise->vertex ? true : false),
		first(ordered ? Edge->vertex : Edge->face_clockwise->vertex),
		second(ordered ? Edge->face_clockwise->vertex : Edge->vertex)
	{
	}

	friend bool operator< <>(const ordered_edge& lhs, const ordered_edge& rhs);
};

template<typename T1, typename T2>
bool operator<(const ordered_edge<T1,T2>& lhs, const ordered_edge<T1,T2>& rhs)
{
	if(lhs.first != rhs.first)
		return lhs.first < rhs.first;

	return lhs.second < rhs.second;
}

typedef ordered_edge<k3d::legacy::point*, k3d::legacy::point*> ordered_edge_t;

} // namespace detail

void set_companions(k3d::legacy::polyhedron& Polyhedron)
{
	typedef std::map<detail::ordered_edge_t, k3d::legacy::split_edge*> edge_map_t;
	edge_map_t edge_map;

	k3d::legacy::polyhedron::faces_t::iterator face;
	for(face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); face++)
	{
		k3d::legacy::split_edge* const first = (*face)->first_edge;
		k3d::legacy::split_edge* current_edge = first;
		do
		{
			detail::ordered_edge_t ordered_edge(current_edge);
			edge_map_t::iterator edge = edge_map.find(ordered_edge);
			if(edge != edge_map.end())
			{
				k3d::legacy::split_edge* other_edge = edge->second;
				if(!other_edge)
					log() << debug << "An edge is shared by more than two faces" << std::endl;
				else
				{
					k3d::legacy::join_edges(*current_edge, *(edge->second));
					edge->second = 0;
				}
			}
			else
			{
				edge_map.insert(std::pair<detail::ordered_edge_t, k3d::legacy::split_edge*>(ordered_edge, current_edge));
			}

			current_edge = current_edge->face_clockwise;
		}
		while(current_edge != first);
	}
}

/////////////////////////////////////////////////////////////////////////////
// linear_curve


/////////////////////////////////////////////////////////////////////////////
// linear_curve_group

linear_curve_group::linear_curve_group() :
	wrap(false),
	material(0)
{
}

linear_curve_group::~linear_curve_group()
{
	std::for_each(curves.begin(), curves.end(), delete_object());
}

/////////////////////////////////////////////////////////////////////////////
// cubic_curve

/////////////////////////////////////////////////////////////////////////////
// cubic_curve_group

cubic_curve_group::cubic_curve_group() :
	wrap(false),
	material(0)
{
}

cubic_curve_group::~cubic_curve_group()
{
	std::for_each(curves.begin(), curves.end(), delete_object());
}

/////////////////////////////////////////////////////////////////////////////
// nucurve

nucurve::nucurve() :
	order(2)
{
}

/////////////////////////////////////////////////////////////////////////////
// nucurve_group

nucurve_group::nucurve_group() :
	material(0)
{
}

nucurve_group::~nucurve_group()
{
	std::for_each(curves.begin(), curves.end(), delete_object());
}

/////////////////////////////////////////////////////////////////////////////
// bilinear_patch

bilinear_patch::bilinear_patch() :
	material(0)
{
}

/////////////////////////////////////////////////////////////////////////////
// bicubic_patch

bicubic_patch::bicubic_patch() :
	material(0)
{
}

/////////////////////////////////////////////////////////////////////////////
// nupatch

nupatch::nupatch() :
	u_order(2),
	v_order(2),
	material(0)
{
}

/////////////////////////////////////////////////////////////////////////////
// blobby

blobby::blobby(opcode* Opcode) :
	root(Opcode),
	material(0)
{
}

blobby::~blobby()
{
	delete root;
}

void blobby::accept(visitor& Visitor)
{
	if(root)
		root->accept(Visitor);
}

/////////////////////////////////////////////////////////////////////////////
// blobby::constant

blobby::constant::constant(double Value) :
	value(Value)
{
}

blobby::opcode* blobby::constant::clone()
{
	return new constant(*this);
}

void blobby::constant::accept(visitor& Visitor)
{
	Visitor.visit_constant(*this);
}

/////////////////////////////////////////////////////////////////////////////
// blobby::ellipsoid

blobby::ellipsoid::ellipsoid(point* Origin, const matrix4& Transformation) :
	origin(Origin),
	transformation(Transformation)
{
}

blobby::opcode* blobby::ellipsoid::clone()
{
	return new ellipsoid(*this);
}

void blobby::ellipsoid::accept(visitor& Visitor)
{
	Visitor.visit_ellipsoid(*this);
}

/////////////////////////////////////////////////////////////////////////////
// blobby::segment

blobby::segment::segment(point* Start, point* End, double Radius, const matrix4& Transformation) :
	start(Start),
	end(End),
	radius(Radius),
	transformation(Transformation)
{
}

blobby::opcode* blobby::segment::clone()
{
	return new segment(*this);
}

void blobby::segment::accept(visitor& Visitor)
{
	Visitor.visit_segment(*this);
}

/////////////////////////////////////////////////////////////////////////////
// blobby::subtract

blobby::subtract::subtract(opcode* Subtrahend, opcode* Minuend) :
	subtrahend(Subtrahend),
	minuend(Minuend)
{
}

blobby::subtract::~subtract()
{
	delete subtrahend;
	delete minuend;
}

blobby::opcode* blobby::subtract::clone()
{
	return new subtract(subtrahend->clone(), minuend->clone());
}

void blobby::subtract::accept(visitor& Visitor)
{
	Visitor.visit_subtract(*this);
}

/////////////////////////////////////////////////////////////////////////////
// blobby::divide

blobby::divide::divide(opcode* Dividend, opcode* Divisor) :
	dividend(Dividend),
	divisor(Divisor)
{
}

blobby::divide::~divide()
{
	delete dividend;
	delete divisor;
}

blobby::opcode* blobby::divide::clone()
{
	return new divide(dividend->clone(), divisor->clone());
}

void blobby::divide::accept(visitor& Visitor)
{
	Visitor.visit_divide(*this);
}

/////////////////////////////////////////////////////////////////////////////
// blobby::variable_operands

void blobby::variable_operands::add_operand(blobby::opcode* Operand)
{
	operands.push_back(Operand);
}

void blobby::variable_operands::operands_accept(visitor& Visitor)
{
	for(operands_t::iterator operand = operands.begin(); operand != operands.end(); ++operand)
		(*operand)->accept(Visitor);
}

blobby::variable_operands::~variable_operands()
{
	std::for_each(operands.begin(), operands.end(), delete_object());
}

void blobby::variable_operands::clone_operands()
{
	for(operands_t::iterator operand = operands.begin(); operand != operands.end(); ++operand)
		(*operand) = (*operand)->clone();
}

/////////////////////////////////////////////////////////////////////////////
// blobby::add

blobby::opcode* blobby::add::clone()
{
	add* result = new add(*this);
	result->clone_operands();
	return result;
}

void blobby::add::accept(visitor& Visitor)
{
	Visitor.visit_add(*this);
}

/////////////////////////////////////////////////////////////////////////////
// blobby::multiply

blobby::opcode* blobby::multiply::clone()
{
	multiply* result = new multiply(*this);
	result->clone_operands();
	return result;
}

void blobby::multiply::accept(visitor& Visitor)
{
	Visitor.visit_multiply(*this);
}

/////////////////////////////////////////////////////////////////////////////
// blobby::max

blobby::opcode* blobby::max::clone()
{
	blobby::max* result = new blobby::max(*this);
	result->clone_operands();
	return result;
}

void blobby::max::accept(visitor& Visitor)
{
	Visitor.visit_max(*this);
}

/////////////////////////////////////////////////////////////////////////////
// blobby::min

blobby::opcode* blobby::min::clone()
{
	blobby::min* result = new blobby::min(*this);
	result->clone_operands();
	return result;
}

void blobby::min::accept(visitor& Visitor)
{
	Visitor.visit_min(*this);
}

/////////////////////////////////////////////////////////////////////////////
// mesh

mesh::mesh()
{
}

mesh::~mesh()
{
	std::for_each(bicubic_patches.begin(), bicubic_patches.end(), delete_object());
	std::for_each(bilinear_patches.begin(), bilinear_patches.end(), delete_object());
	std::for_each(cubic_curve_groups.begin(), cubic_curve_groups.end(), delete_object());
	std::for_each(linear_curve_groups.begin(), linear_curve_groups.end(), delete_object());
	std::for_each(polyhedra.begin(), polyhedra.end(), delete_object());
	std::for_each(points.begin(), points.end(), delete_object());
}

/** \todo support all gprims */
mesh& mesh::operator=(const k3d::mesh& Mesh)
{
	primitives = Mesh.primitives;

	if(Mesh.points && Mesh.point_selection)
	{
		const k3d::mesh::points_t& new_points = *Mesh.points;
		const k3d::mesh::selection_t& point_selection = *Mesh.point_selection.get();

		const uint_t point_begin = 0;
		const uint_t point_end = point_begin + Mesh.points->size();
		for(uint_t point = point_begin; point != point_end; ++point)
		{
			k3d::legacy::point* const legacy_point = new k3d::legacy::point(new_points[point]);
			legacy_point->selection_weight = point_selection[point];

			points.push_back(legacy_point);
		}
	}

	boost::scoped_ptr<k3d::nurbs_curve::const_primitive> nurbs_curve(k3d::nurbs_curve::validate(Mesh));
	if(nurbs_curve)
	{
		const uint_t group_begin = 0;
		const uint_t group_end = group_begin + nurbs_curve->first_curves.size();
		for(uint_t group = group_begin; group != group_end; ++group)
		{
			legacy::nucurve_group* const legacy_group = new legacy::nucurve_group();
			nucurve_groups.push_back(legacy_group);

			legacy_group->material = nurbs_curve->materials[group];

			const uint_t curve_begin = nurbs_curve->first_curves[group];
			const uint_t curve_end = curve_begin + nurbs_curve->curve_counts[group];
			for(uint_t curve = curve_begin; curve != curve_end; ++curve)
			{
				legacy::nucurve* const legacy_curve = new legacy::nucurve();
				legacy_group->curves.push_back(legacy_curve);

				legacy_curve->order = nurbs_curve->curve_orders[curve];
				legacy_curve->knots.assign(
					nurbs_curve->curve_knots.begin() + nurbs_curve->curve_first_knots[curve],
					nurbs_curve->curve_knots.begin() + nurbs_curve->curve_first_knots[curve] + nurbs_curve->curve_orders[curve] + nurbs_curve->curve_point_counts[curve]);
					
				legacy_curve->selection_weight = nurbs_curve->curve_selections[curve];

				const uint_t point_begin = nurbs_curve->curve_first_points[curve];
				const uint_t point_end = point_begin + nurbs_curve->curve_point_counts[curve];
				for(uint_t point = point_begin; point != point_end; ++point)
					legacy_curve->control_points.push_back(legacy::nucurve::control_point(points[nurbs_curve->curve_points[point]], nurbs_curve->curve_point_weights[point]));
			}
		}
	}

	boost::scoped_ptr<k3d::nurbs_patch::const_primitive> nurbs_patch(k3d::nurbs_patch::validate(Mesh));
	if(nurbs_patch)
	{
		const uint_t patch_begin = 0;
		const uint_t patch_end = patch_begin + nurbs_patch->patch_first_points.size();
		for(uint_t patch = patch_begin; patch != patch_end; ++patch)
		{
			legacy::nupatch* const legacy_patch = new legacy::nupatch();
			nupatches.push_back(legacy_patch);

			legacy_patch->u_order = nurbs_patch->patch_u_orders[patch];
			legacy_patch->v_order = nurbs_patch->patch_v_orders[patch];
			legacy_patch->u_knots.assign(
				nurbs_patch->patch_u_knots.begin() + nurbs_patch->patch_u_first_knots[patch],
				nurbs_patch->patch_u_knots.begin() + nurbs_patch->patch_u_first_knots[patch] + nurbs_patch->patch_u_point_counts[patch] + nurbs_patch->patch_u_orders[patch]); 
			legacy_patch->v_knots.assign(
				nurbs_patch->patch_v_knots.begin() + nurbs_patch->patch_v_first_knots[patch],
				nurbs_patch->patch_v_knots.begin() + nurbs_patch->patch_v_first_knots[patch] + nurbs_patch->patch_v_point_counts[patch] + nurbs_patch->patch_v_orders[patch]); 
			legacy_patch->selection_weight = nurbs_patch->patch_selections[patch];
			legacy_patch->material = nurbs_patch->patch_materials[patch];

			const uint_t point_begin = nurbs_patch->patch_first_points[patch];
			const uint_t point_end = point_begin + (nurbs_patch->patch_u_point_counts[patch] * nurbs_patch->patch_v_point_counts[patch]);
			for(uint_t point = point_begin; point != point_end; ++point)
			{
				legacy::nupatch::control_point const legacy_point(
					points[nurbs_patch->patch_points[point]],
					nurbs_patch->patch_point_weights[point]);
				legacy_patch->control_points.push_back(legacy_point);
			}
		}
	}

	boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Mesh));
	if(polyhedron)
	{
		const uint_t shell_begin = 0;
		const uint_t shell_end = shell_begin + polyhedron->first_faces.size();
		for(uint_t shell = shell_begin; shell != shell_end; ++shell)
		{
			legacy::polyhedron* const legacy_polyhedron = new legacy::polyhedron();
			polyhedra.push_back(legacy_polyhedron);

			switch(polyhedron->polyhedron_types[shell])
			{
				case k3d::mesh::polyhedra_t::POLYGONS:
					legacy_polyhedron->type = legacy::polyhedron::POLYGONS;
					break;
				case k3d::mesh::polyhedra_t::CATMULL_CLARK:
					legacy_polyhedron->type = legacy::polyhedron::CATMULL_CLARK_SUBDIVISION_MESH;
					break;
			}

			const uint_t face_begin = polyhedron->first_faces[shell];
			const uint_t face_end = face_begin + polyhedron->face_counts[shell];
			for(uint_t face = face_begin; face != face_end; ++face)
			{
				const uint_t loop_begin = polyhedron->face_first_loops[face];
				const uint_t loop_end = loop_begin + polyhedron->face_loop_counts[face];
				for(uint_t loop = loop_begin; loop != loop_end; ++loop)
				{
					const uint_t first_edge = polyhedron->loop_first_edges[loop];
					std::vector<legacy::split_edge*> legacy_loop;
					for(uint_t edge = first_edge; ; )
					{
						legacy::split_edge* const legacy_edge = new legacy::split_edge(points[polyhedron->edge_points[edge]]);
						legacy_edge->selection_weight = polyhedron->edge_selections[edge];
						legacy_loop.push_back(legacy_edge);

						edge = polyhedron->clockwise_edges[edge];
						if(edge == first_edge)
							break;
					}

					legacy::loop_edges(legacy_loop.begin(), legacy_loop.end());

					if(loop == loop_begin && legacy_loop.size())
					{
						legacy::face* const legacy_face = new legacy::face(legacy_loop.front(), 0);

						legacy_polyhedron->faces.push_back(legacy_face);

						legacy_face->material = polyhedron->face_materials[face];
						legacy_face->selection_weight = polyhedron->face_selections[face];
					}

					/** \todo Support faces with holes */
					break;
				}
			}

			set_companions(*legacy_polyhedron);
		}
	}

	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// triangulate

#include "mesh_triangulate_detail.h"

template<typename InputFaceIterator, typename OutputFaceIterator, typename OutputPointIterator>
void triangulate(InputFaceIterator Begin, InputFaceIterator End, OutputFaceIterator OutputFaces, OutputPointIterator OutputPoints)
{
	detail::glu_triangulator_t<OutputFaceIterator, OutputPointIterator> triangulator(OutputFaces, OutputPoints);
	for(InputFaceIterator face = Begin; face != End; ++face)
		triangulator(**face);
}

void triangulate(const polyhedron::faces_t& Faces, polyhedron::faces_t& NewFaces, mesh::points_t& NewPoints)
{
	triangulate(Faces.begin(), Faces.end(), std::back_inserter(NewFaces), std::back_inserter(NewPoints));
}

/////////////////////////////////////////////////////////////////////////////
// add_unit_cube

void add_unit_cube(mesh& Mesh, polyhedron& Polyhedron, imaterial* const Material)
{
	// Create points ...
	boost::multi_array<point*, 3> points(boost::extents[2][2][2]);
	points[0][0][0] = new point(-0.5, -0.5, -0.5);
	points[1][0][0] = new point(0.5, -0.5, -0.5);
	points[1][1][0] = new point(0.5, 0.5, -0.5);
	points[0][1][0] = new point(-0.5, 0.5, -0.5);
	points[0][0][1] = new point(-0.5, -0.5, 0.5);
	points[1][0][1] = new point(0.5, -0.5, 0.5);
	points[1][1][1] = new point(0.5, 0.5, 0.5);
	points[0][1][1] = new point(-0.5, 0.5, 0.5);

	for(unsigned long i = 0; i != 2; ++i)
		for(unsigned long j = 0; j != 2; ++j)
			for(unsigned long k = 0; k != 2; ++k)
				Mesh.points.push_back(points[i][j][k]);

	// Create edges ...
	boost::multi_array<split_edge*, 2> edges(boost::extents[6][4]);
	edges[0][0] = new split_edge(points[0][1][0]);
	edges[0][1] = new split_edge(points[1][1][0]);
	edges[0][2] = new split_edge(points[1][0][0]);
	edges[0][3] = new split_edge(points[0][0][0]);

	edges[1][0] = new split_edge(points[1][1][0]);
	edges[1][1] = new split_edge(points[1][1][1]);
	edges[1][2] = new split_edge(points[1][0][1]);
	edges[1][3] = new split_edge(points[1][0][0]);

	edges[2][0] = new split_edge(points[1][1][1]);
	edges[2][1] = new split_edge(points[0][1][1]);
	edges[2][2] = new split_edge(points[0][0][1]);
	edges[2][3] = new split_edge(points[1][0][1]);

	edges[3][0] = new split_edge(points[0][1][1]);
	edges[3][1] = new split_edge(points[0][1][0]);
	edges[3][2] = new split_edge(points[0][0][0]);
	edges[3][3] = new split_edge(points[0][0][1]);

	edges[4][0] = new split_edge(points[0][1][1]);
	edges[4][1] = new split_edge(points[1][1][1]);
	edges[4][2] = new split_edge(points[1][1][0]);
	edges[4][3] = new split_edge(points[0][1][0]);

	edges[5][0] = new split_edge(points[0][0][0]);
	edges[5][1] = new split_edge(points[1][0][0]);
	edges[5][2] = new split_edge(points[1][0][1]);
	edges[5][3] = new split_edge(points[0][0][1]);

	edges[0][0]->companion = edges[4][2];
	edges[0][1]->companion = edges[1][3];
	edges[0][2]->companion = edges[5][0];
	edges[0][3]->companion = edges[3][1];

	edges[1][0]->companion = edges[4][1];
	edges[1][1]->companion = edges[2][3];
	edges[1][2]->companion = edges[5][1];
	edges[1][3]->companion = edges[0][1];

	edges[2][0]->companion = edges[4][0];
	edges[2][1]->companion = edges[3][3];
	edges[2][2]->companion = edges[5][2];
	edges[2][3]->companion = edges[1][1];

	edges[3][0]->companion = edges[4][3];
	edges[3][1]->companion = edges[0][3];
	edges[3][2]->companion = edges[5][3];
	edges[3][3]->companion = edges[2][1];

	edges[4][0]->companion = edges[2][0];
	edges[4][1]->companion = edges[1][0];
	edges[4][2]->companion = edges[0][0];
	edges[4][3]->companion = edges[3][0];

	edges[5][0]->companion = edges[0][2];
	edges[5][1]->companion = edges[1][2];
	edges[5][2]->companion = edges[2][2];
	edges[5][3]->companion = edges[3][2];

	for(unsigned long i = 0; i != 6; ++i)
		for(unsigned long j = 0; j != 4; ++j)
			edges[i][j]->face_clockwise = edges[i][(j+1)%4];

	// Create faces ...
	for(unsigned long i = 0; i != 6; ++i)
		Polyhedron.faces.push_back(new face(edges[i][0], Material));
}

/////////////////////////////////////////////////////////////////////////////
// add_grid

grid_results_t add_grid(mesh& Mesh, polyhedron& Polyhedron, const unsigned long Rows, const unsigned long Columns, const bool StitchTop, const bool StitchSide, imaterial* const Material)
{
	// Sanity checks ...
	assert(Rows);
	assert(Columns);

	// Calculate the number of faces to create along each axis ...
	const unsigned long face_rows = Rows;
	const unsigned long face_columns = Columns;

	// Calculate the number of points that need to be created along each axis ...
	unsigned long point_rows = face_rows + (StitchTop ? 0 : 1);
	unsigned long point_columns = face_columns + (StitchSide ? 0 : 1);

	// Create points ...
	boost::multi_array<point*, 2> points(boost::extents[point_rows][point_columns]);
	for(unsigned long row = 0; row != point_rows; ++row)
	{
		for(unsigned long column = 0; column != point_columns; ++column)
		{
			points[row][column] = new point(0, 0, 0);
			Mesh.points.push_back(points[row][column]);
		}
	}

	// Create edges ...
	boost::multi_array<split_edge*, 3> edges(boost::extents[face_rows][face_columns][4]);
	for(unsigned long row = 0; row != face_rows; ++row)
	{
		for(unsigned long column = 0; column != face_columns; ++column)
		{
			edges[row][column][0] = new split_edge(points[row][column]);
			edges[row][column][1] = new split_edge(points[row][(column+1) % point_columns]);
			edges[row][column][2] = new split_edge(points[(row+1) % point_rows][(column+1) % point_columns]);
			edges[row][column][3] = new split_edge(points[(row+1) % point_rows][column]);

			for(unsigned long i = 0; i != 4; ++i)
				edges[row][column][i]->face_clockwise = edges[row][column][(i+1)%4];
		}
	}

	// Join edges ...
	const unsigned long edge_rows = face_rows - (StitchTop ? 0 : 1);
	const unsigned long edge_columns = face_columns - (StitchSide ? 0 : 1);

	for(unsigned long row = 0; row != edge_rows; ++row)
	{
		for(unsigned long column = 0; column != face_columns; ++column)
			join_edges(*edges[row][column][2], *edges[(row+1) % face_rows][column][0]);
	}

	for(unsigned long column = 0; column != edge_columns; ++column)
	{
		for(unsigned long row = 0; row != face_rows; ++row)
			join_edges(*edges[row][column][1], *edges[row][(column+1) % face_columns][3]);
	}

	// Create faces ...
	boost::multi_array<face*, 2> faces(boost::extents[face_rows][face_columns]);
	for(unsigned long row = 0; row != face_rows; ++row)
	{
		for(unsigned long column = 0; column != face_columns; ++column)
		{
			face* const new_face = new face(edges[row][column][0], Material);
			Polyhedron.faces.push_back(new_face);
		}
	}

	return boost::make_tuple(points, edges, faces);
}

namespace detail
{

/// Provides a mapping of old-to-new points that can be used with std::transform
struct point_map_t :
	public std::map<point*, point*>,
	public blobby::visitor
{
	virtual ~point_map_t()
	{
	}

	point* operator()(point* Key)
	{
		return operator[](Key);
	}

	void visit_constant(blobby::constant& Constant)
	{
	}

	void visit_ellipsoid(blobby::ellipsoid& Ellipsoid)
	{
		Ellipsoid.origin = operator[](Ellipsoid.origin);
	}

	void visit_segment(blobby::segment& Segment)
	{
		Segment.start = operator[](Segment.start);
		Segment.end = operator[](Segment.end);
	}

	void visit_subtract(blobby::subtract& Subtract)
	{
		Subtract.subtrahend->accept(*this);
		Subtract.minuend->accept(*this);
	}

	void visit_divide(blobby::divide& Divide)
	{
		Divide.dividend->accept(*this);
		Divide.divisor->accept(*this);
	}

	void visit_add(blobby::add& Add)
	{
		Add.operands_accept(*this);
	}

	void visit_multiply(blobby::multiply& Multiply)
	{
		Multiply.operands_accept(*this);
	}

	void visit_min(blobby::min& Min)
	{
		Min.operands_accept(*this);
	}

	void visit_max(blobby::max& Max)
	{
		Max.operands_accept(*this);
	}
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// deep_copy

void deep_copy(const mesh& Input, mesh& Output)
{
	// Duplicate points ...
	detail::point_map_t point_map;
	point_map[0] = 0;
	for(mesh::points_t::const_iterator p = Input.points.begin(); p != Input.points.end(); ++p)
	{
		Output.points.push_back(new point(**p));
		point_map.insert(std::make_pair(*p, Output.points.back()));
	}

	// Duplicate polyhedra ...
	for(mesh::polyhedra_t::const_iterator pn = Input.polyhedra.begin(); pn != Input.polyhedra.end(); ++pn)
	{
		polyhedron* const new_polyhedron = new polyhedron(**pn);
		Output.polyhedra.push_back(new_polyhedron);

		// Duplicate edges ...
		typedef std::map<split_edge*, split_edge*> edge_map_t;
		edge_map_t edge_map;
		edge_map[0] = 0;
		for(polyhedron::faces_t::const_iterator f = new_polyhedron->faces.begin(); f != new_polyhedron->faces.end(); ++f)
		{
			for(split_edge* edge = (*f)->first_edge; edge; edge = edge->face_clockwise)
			{
				edge_map.insert(std::make_pair(edge, new split_edge(*edge)));
				if(edge->face_clockwise == (*f)->first_edge)
					break;
			}

			for(face::holes_t::const_iterator hole = (*f)->holes.begin(); hole != (*f)->holes.end(); ++hole)
			{
				for(split_edge* edge = *hole; edge; edge = edge->face_clockwise)
				{
					edge_map.insert(std::make_pair(edge, new split_edge(*edge)));
					if(edge->face_clockwise == (*hole))
						break;
				}
			}
		}

		// Relink edges ...
		const edge_map_t::const_iterator edge_map_end = edge_map.end();
		for(edge_map_t::const_iterator edge = edge_map.begin(); edge != edge_map_end; ++edge)
		{
			if(!edge->first)
				continue;

			// Replaced the shortest 'edge->second->face_clockwise = edge_map[edge->second->face_clockwise]'
			// statement since searching a non-existing edge inserted it in the map
			edge->second->vertex = point_map[edge->second->vertex];
			const edge_map_t::const_iterator face_clockwise = edge_map.find(edge->second->face_clockwise);
			return_if_fail(face_clockwise != edge_map_end);
			edge->second->face_clockwise = face_clockwise->second;

			const edge_map_t::const_iterator companion = edge_map.find(edge->second->companion);
			return_if_fail(companion != edge_map_end);
			edge->second->companion = companion->second;
		}

		// Duplicate faces and re-link edges ...
		for(polyhedron::faces_t::iterator f = new_polyhedron->faces.begin(); f != new_polyhedron->faces.end(); ++f)
		{
			face* const new_face = new face(**f);

			new_face->first_edge = edge_map[new_face->first_edge];
			for(face::holes_t::iterator hole = new_face->holes.begin(); hole != new_face->holes.end(); ++hole)
				*hole = edge_map[*hole];

			*f = new_face;
		}
	}

	// Duplicate linear curve groups ...
	for(mesh::linear_curve_groups_t::const_iterator group = Input.linear_curve_groups.begin(); group != Input.linear_curve_groups.end(); ++group)
	{
		linear_curve_group* const new_group = new linear_curve_group(**group);

		for(linear_curve_group::curves_t::iterator curve = new_group->curves.begin(); curve != new_group->curves.end(); ++curve)
		{
			*curve = new linear_curve(**curve);
			std::transform((*curve)->control_points.begin(), (*curve)->control_points.end(), (*curve)->control_points.begin(), point_map);
		}

		Output.linear_curve_groups.push_back(new_group);
	}

	// Duplicate cubic curve groups ...
	for(mesh::cubic_curve_groups_t::const_iterator group = Input.cubic_curve_groups.begin(); group != Input.cubic_curve_groups.end(); ++group)
	{
		cubic_curve_group* const new_group = new cubic_curve_group(**group);

		for(cubic_curve_group::curves_t::iterator curve = new_group->curves.begin(); curve != new_group->curves.end(); ++curve)
		{
			*curve = new cubic_curve(**curve);
			std::transform((*curve)->control_points.begin(), (*curve)->control_points.end(), (*curve)->control_points.begin(), point_map);
		}

		Output.cubic_curve_groups.push_back(new_group);
	}

	// Duplicate nucurve groups ...
	for(mesh::nucurve_groups_t::const_iterator group = Input.nucurve_groups.begin(); group != Input.nucurve_groups.end(); ++group)
	{
		nucurve_group* const new_group = new nucurve_group(**group);

		for(nucurve_group::curves_t::iterator curve = new_group->curves.begin(); curve != new_group->curves.end(); ++curve)
		{
			*curve = new nucurve(**curve);
			for(nucurve::control_points_t::iterator control_point = (*curve)->control_points.begin(); control_point != (*curve)->control_points.end(); ++control_point)
				control_point->position = point_map[control_point->position];
		}

		Output.nucurve_groups.push_back(new_group);
	}

	// Duplicate bilinear patches ...
	for(mesh::bilinear_patches_t::const_iterator patch = Input.bilinear_patches.begin(); patch != Input.bilinear_patches.end(); ++patch)
	{
		bilinear_patch* const new_patch = new bilinear_patch(**patch);
		std::transform(new_patch->control_points.begin(), new_patch->control_points.end(), new_patch->control_points.begin(), point_map);

		Output.bilinear_patches.push_back(new_patch);
	}

	// Duplicate bicubic patches ...
	for(mesh::bicubic_patches_t::const_iterator patch = Input.bicubic_patches.begin(); patch != Input.bicubic_patches.end(); ++patch)
	{
		bicubic_patch* const new_patch = new bicubic_patch(**patch);
		std::transform(new_patch->control_points.begin(), new_patch->control_points.end(), new_patch->control_points.begin(), point_map);

		Output.bicubic_patches.push_back(new_patch);
	}

	// Duplicate nupatches ...
	for(mesh::nupatches_t::const_iterator patch = Input.nupatches.begin(); patch != Input.nupatches.end(); ++patch)
	{
		nupatch* const new_patch = new nupatch(**patch);
		for(nupatch::control_points_t::iterator control_point = new_patch->control_points.begin(); control_point != new_patch->control_points.end(); ++control_point)
			control_point->position = point_map[control_point->position];

		Output.nupatches.push_back(new_patch);
	}

	// Duplicate blobbies ...
	for(mesh::blobbies_t::const_iterator blob = Input.blobbies.begin(); blob != Input.blobbies.end(); blob++)
	{
		blobby* const new_blobby = new blobby((*blob)->root->clone());
		new_blobby->accept(point_map);

		Output.blobbies.push_back(new_blobby);
	}
}

/////////////////////////////////////////////////////////////////////////////
// segment_count

const unsigned long segment_count(const linear_curve& Curve, const bool Wrap)
{
	if(Wrap)
		return Curve.control_points.size();

	return Curve.control_points.size() ? Curve.control_points.size() - 1 : 0;
}

const unsigned long segment_count(const cubic_curve& Curve, const bool Wrap)
{
	const unsigned long vstep = 3;

	if(Wrap)
		return Curve.control_points.size() / vstep;

	return Curve.control_points.size() > 3 ? ((Curve.control_points.size() - 4) / vstep) + 1 : 0;
}

/////////////////////////////////////////////////////////////////////////////
// varying count

const unsigned long varying_count(const linear_curve& Curve, const bool Wrap)
{
	return Wrap ? segment_count(Curve, Wrap) : segment_count(Curve, Wrap) + 1;
}

const unsigned long varying_count(const cubic_curve& Curve, const bool Wrap)
{
	return Wrap ? segment_count(Curve, Wrap) : segment_count(Curve, Wrap) + 1;
}

/////////////////////////////////////////////////////////////////////////////
// is_valid

bool is_valid(const polyhedron& Polyhedron)
{
	// For every face ...
	for(polyhedron::faces_t::const_iterator face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
	{
		// This is obviously wrong!!!
		return_val_if_fail(*face, false);

		// Check face edges
		split_edge* first_edge = (*face)->first_edge;

		split_edge* edge = first_edge;
		unsigned long edge_count = 0;
		do
		{
			// Every edge should have a neighbour ...
			return_val_if_fail(edge, false);

			// Every edge should have a vertex ...
			return_val_if_fail(edge->vertex, false);

			// For edges with companions ...
			if(edge->companion)
			{
				// Companions had better point to each other ...
				return_val_if_fail(edge->companion->companion == edge, false);

				// Companions had better share the same vertices ...
				return_val_if_fail(edge->vertex == edge->companion->face_clockwise->vertex, false);
				return_val_if_fail(edge->face_clockwise->vertex == edge->companion->vertex, false);
			}

			++edge_count;
			edge = edge->face_clockwise;
		}
		while(edge != first_edge);

		// Every face should have at least three edges ...
		return_val_if_fail(edge_count > 2, false);

		// Check holes
		for(k3d::legacy::face::holes_t::iterator hole = (*face)->holes.begin(); hole != (*face)->holes.end(); ++hole)
		{
			for(k3d::legacy::split_edge* edge = *hole; edge; edge = edge->face_clockwise)
			{
				// Every edge should have a vertex ...
				return_val_if_fail(edge->vertex, false);

				if(edge->face_clockwise == (*hole))
					break;
			}
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// is_valid

bool is_valid(const nucurve& Curve)
{
	// Order must always be at least 2 (i.e. a linear curve)
	return_val_if_fail(Curve.order >= 2, false);

	// The number of control points must be >= order
	return_val_if_fail(Curve.control_points.size() >= Curve.order, false);

	// The number of knots must be equal to the number of control points plus the order
	return_val_if_fail(Curve.knots.size() == Curve.control_points.size() + Curve.order, false);

	// Knot vector values must always be in ascending order
	for(unsigned long i = 1; i != Curve.knots.size(); ++i)
		return_val_if_fail(Curve.knots[i] >= Curve.knots[i-1], false);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// is_valid

bool is_valid(const nupatch& Patch)
{
	// Order must always be at least 2 (i.e. linear curves), in each parametric direction
	return_val_if_fail(Patch.u_order >= 2 && Patch.v_order >= 2, false);

	// The number of control points must be >= order, in each parametric direction

	// The number of knots must be equal to the number of control points plus the order, in each parametric direction

	// Knot vector values must always be in ascending order, in each parametric direction
	for(unsigned long i = 1; i != Patch.u_knots.size(); ++i)
		return_val_if_fail(Patch.u_knots[i] >= Patch.u_knots[i-1], false);

	for(unsigned long i = 1; i != Patch.v_knots.size(); ++i)
		return_val_if_fail(Patch.v_knots[i] >= Patch.v_knots[i-1], false);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// is_solid

bool is_solid(const polyhedron& Polyhedron)
{
	if(!is_valid(Polyhedron))
		return false;

	for(polyhedron::faces_t::const_iterator face = Polyhedron.faces.begin(); face != Polyhedron.faces.end(); ++face)
	{
		split_edge* first_edge = (*face)->first_edge;
		split_edge* edge = first_edge;

		do
		{
			// There should be no border edge
			return_val_if_fail(edge->companion, false);

			edge = edge->face_clockwise;
		}
		while(edge != first_edge);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// bounds

const bounding_box3 bounds(const mesh& Mesh)
{
	bounding_box3 results;
	for(mesh::points_t::const_iterator point = Mesh.points.begin(); point != Mesh.points.end(); ++point)
		results.insert((*point)->position);

	return results;
}

} // namespace legacy

} // namespace k3d

