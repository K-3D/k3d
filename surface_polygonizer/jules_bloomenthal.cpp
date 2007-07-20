// A C++ Implicit Surface Polygonizer
// Copyright 2002-2004, Romain Behar <romainbehar@yahoo.com>
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
		\brief Implements bloomenthal_polygonizer
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/result.h>

#include "jules_bloomenthal.h"

#include <algorithm>
#include <iostream>

// Number of iterations (convergence)
const int RES = 10;

// Directions
const enum_t L = 0;	// left:	-x, -i
const enum_t R = 1;	// right:	+x, +i
const enum_t B = 2;	// bottom:	-y, -j
const enum_t T = 3;	// top:		+y, +j
const enum_t N = 4;	// near:	-z, -k
const enum_t F = 5;	// far:		+z, +k

// Corners
const enum_t LBN = 0;	// left bottom near
const enum_t LBF = 1;	// left bottom far
const enum_t LTN = 2;	// left top near
const enum_t LTF = 3;	// left top far
const enum_t RBN = 4;	// right bottom near
const enum_t RBF = 5;	// right bottom far
const enum_t RTN = 6;	// right top near
const enum_t RTF = 7;	// right top far

bloomenthal_polygonizer::bloomenthal_polygonizer(
		const polygonization_t polygonization_type,
		const double voxel_size,
		const double threshold,
		const int xmin, const int xmax,
		const int ymin, const int ymax,
		const int zmin, const int zmax,
		const vertex_t& origin,
		implicit_functor& functor,
		vertices_t& surface_vertices,
		vertices_t& surface_normals,
		polygons_t& surface_polygons) :
	m_Decomposition(polygonization_type),
	m_VoxelSize(voxel_size),
	m_Threshold(threshold),
	m_MinCorner(Location(xmin, ymin, zmin)),
	m_MaxCorner(Location(xmax, ymax, zmax)),
	m_keep_within_limits(true),
	m_GridOrigin(origin),
	m_FieldFunctor(functor),
	m_Vertices(surface_vertices),
	m_normals(surface_normals),
	m_Polygons(surface_polygons)
{
	// Sanity checks ...
	if(!(m_MinCorner <= nearest_location(m_GridOrigin) && nearest_location(m_GridOrigin) < m_MaxCorner))
		{
			k3d::log() << debug << "Surface Polygonizer: grid origin must be in grid, defaulting to min corner" << std::endl;
			m_GridOrigin = location_vertex(m_MinCorner);
		}

	// Init polygonizing algorithm
	if(m_Decomposition == MARCHINGCUBES)
		MakeCubeTable();
}

bloomenthal_polygonizer::~bloomenthal_polygonizer()
{
	// Delete corners
}

// Return the vertex_t corresponding to the Location
vertex_t bloomenthal_polygonizer::location_vertex(const Location& l)
{
	return m_GridOrigin + m_VoxelSize * vertex_t((double)l.i, (double)l.j, (double)l.k);
}

// Return the nearest location corresponding to the vertex_t
Location bloomenthal_polygonizer::nearest_location(const vertex_t& point)
{
	vertex_t vertex_position = (point - m_GridOrigin) / m_VoxelSize;

	int i = static_cast<int>(vertex_position.n[0]);
	int j = static_cast<int>(vertex_position.n[1]);
	int k = static_cast<int>(vertex_position.n[2]);

	return Location(i, j, k);
}

// Sample the whole grid and polygonize
void bloomenthal_polygonizer::polygonize_whole_grid()
{
	for(Location x = m_MinCorner; x <= m_MaxCorner; x = x.Right())
		for(Location y = x; y <= m_MaxCorner; y = y.Top())
			for(Location z = y; z <= m_MaxCorner; z = z.Far())
				{
					Corner* corner = get_cached_corner(z);
					if(corner->value < m_Threshold)
						continue;

					Location surface_location = z;
					if(SurfaceLocation(surface_location))
						PolygonizeSurface(surface_location);
				}
}

// Find surface and polygonize from a known inside point
bool bloomenthal_polygonizer::polygonize_from_inside_point(const vertex_t& starting_point)
{
	Location starting_location = nearest_location(starting_point);

	// Make sure the point is inside a surface
	Corner* corner = get_cached_corner(starting_location);
	if(corner->value < m_Threshold)
		return false;

	// Get a Location enclosing surface
	if(!SurfaceLocation(starting_location))
		return false;

	// Surface found, polygonize it
	PolygonizeSurface(starting_location);

	return true;
}

void bloomenthal_polygonizer::PolygonizeSurface(const Location& startinglocation)
{
	// Create initial cube
	if(mark_center(startinglocation))
		return;

	Cube c(startinglocation);
	for(int n = 0; n < 8; n++)
		c.corners[n] = get_cached_corner(startinglocation + Location(bit_value(n, 2), bit_value(n, 1), bit_value(n, 0)));

	// Push it on stack
	m_active_cubes.push(c);

	// Process active cubes till none left
	while(!m_active_cubes.empty())
		{
			Cube c = m_active_cubes.top();
			m_active_cubes.pop();

			// Polygonize
			switch(m_Decomposition)
				{
					case MARCHINGCUBES:
						MarchingCube(c);
					break;
					case TETRAHEDRAL:
						// Decompose into tetrahedra and polygonize
						TriangulateTet(c, LBN, LTN, RBN, LBF);
						TriangulateTet(c, RTN, LTN, LBF, RBN);
						TriangulateTet(c, RTN, LTN, LTF, LBF);
						TriangulateTet(c, RTN, RBN, LBF, RBF);
						TriangulateTet(c, RTN, LBF, LTF, RBF);
						TriangulateTet(c, RTN, LTF, RTF, RBF);
					break;
					default:
						assert_not_reached();
				}

			// Test six face directions, maybe add to stack
			TestFace(c.l.Left(), c, L, LBN, LBF, LTN, LTF);
			TestFace(c.l.Right(), c, R, RBN, RBF, RTN, RTF);
			TestFace(c.l.Bottom(), c, B, LBN, LBF, RBN, RBF);
			TestFace(c.l.Top(), c, T, LTN, LTF, RTN, RTF);
			TestFace(c.l.Near(), c, N, LBN, LTN, RBN, RTN);
			TestFace(c.l.Far(), c, F, LBF, LTF, RBF, RTF);
		}
}

// Find a location enclosing surface
bool bloomenthal_polygonizer::SurfaceLocation(Location& startinglocation)
{
	Location loc2 = startinglocation;
	double value2 = m_FieldFunctor.implicit_value(location_vertex(loc2)) - m_Threshold;

	// Top
	do
		{
			Location loc1 = loc2;
			double value1 = value2;

			loc2 = loc2.Top();
			value2 = m_FieldFunctor.implicit_value(location_vertex(loc2)) - m_Threshold;

			if((value1*value2 < 0) || ((value1 == 0) && (value2 < 0)) || ((value2 == 0) && (value1 < 0)))
				{
					startinglocation = loc1;
					return true;
				}
		}
	while(loc2 <= m_MaxCorner);

	// We reached the grid boundary: check the whole grid
	return false;
}

// Triangulate the tetrahedron (b, c, d should appear clockwise when viewed from a)
void bloomenthal_polygonizer::TriangulateTet(const Cube& cube1, int c1, int c2, int c3, int c4)
{
	Corner *a = cube1.corners[c1];
	Corner *b = cube1.corners[c2];
	Corner *c = cube1.corners[c3];
	Corner *d = cube1.corners[c4];

	bool apos = (a->value >= m_Threshold);
	bool bpos = (b->value >= m_Threshold);
	bool cpos = (c->value >= m_Threshold);
	bool dpos = (d->value >= m_Threshold);

	int index = 0;
	if(apos)
		index += 8;
	if(bpos)
		index += 4;
	if(cpos)
		index += 2;
	if(dpos)
		index += 1;

	// Index is now 4-bit number representing one of the 16 possible cases
	int e1 = 0;
	int e2 = 0;
	int e3 = 0;
	int e4 = 0;
	int e5 = 0;
	int e6 = 0;
	if(apos != bpos)
		e1 = VerticeId(a, b);
	if(apos != cpos)
		e2 = VerticeId(a, c);
	if(apos != dpos)
		e3 = VerticeId(a, d);
	if(bpos != cpos)
		e4 = VerticeId(b, c);
	if(bpos != dpos)
		e5 = VerticeId(b, d);
	if(cpos != dpos)
		e6 = VerticeId(c, d);

	// 14 productive tetrahedral cases (0000 and 1111 do not yield polygons)
	switch(index)
		{
			case 1: SaveTriangle(e5, e6, e3); break;
			case 2: SaveTriangle(e2, e6, e4); break;
			case 3:	SaveTriangle(e3, e5, e4); SaveTriangle(e3, e4, e2); break;
			case 4: SaveTriangle(e1, e4, e5); break;
			case 5: SaveTriangle(e3, e1, e4); SaveTriangle(e3, e4, e6); break;
			case 6: SaveTriangle(e1, e2, e6); SaveTriangle(e1, e6, e5); break;
			case 7: SaveTriangle(e1, e2, e3); break;
			case 8: SaveTriangle(e1, e3, e2); break;
			case 9: SaveTriangle(e1, e5, e6); SaveTriangle(e1, e6, e2); break;
			case 10: SaveTriangle(e1, e3, e6); SaveTriangle(e1, e6, e4); break;
			case 11: SaveTriangle(e1, e5, e4); break;
			case 12: SaveTriangle(e3, e2, e4); SaveTriangle(e3, e4, e5); break;
			case 13: SaveTriangle(e6, e2, e4); break;
			case 14: SaveTriangle(e5, e3, e6); break;
		}
}

namespace mc
{

// Edges
const enum_t LB = 0;	// left bottom
const enum_t LT = 1;	// left top
const enum_t LN = 2;	// left near
const enum_t LF = 3;	// left far
const enum_t RB = 4;	// right bottom
const enum_t RT = 5;	// right top
const enum_t RN = 6;	// right near
const enum_t RF = 7;	// right far
const enum_t BN = 8;	// bottom near
const enum_t BF = 9;	// bottom far
const enum_t TN = 10;	// top near
const enum_t TF = 11;	// top far

// Face on left when going from corner1 to corner2
const enum_t leftface[12] = {B, L, L, F, R, T, N, R, N, B, T, F};
// Face on right when going from corner1 to corner2
const enum_t rightface[12] = {L, T, N, L, B, R, R, F, B, F, N, T};

// Return next clockwise edge from given edge around given face
enum_t next_edge(const enum_t edge, const enum_t face)
{
	switch(edge)
		{
			case LB: return (face == L) ? LF : BN;
			case LT: return (face == L) ? LN : TF;
			case LN: return (face == L) ? LB : TN;
			case LF: return (face == L) ? LT : BF;
			case RB: return (face == R) ? RN : BF;
			case RT: return (face == R) ? RF : TN;
			case RN: return (face == R) ? RT : BN;
			case RF: return (face == R) ? RB : TF;
			case BN: return (face == B) ? RB : LN;
			case BF: return (face == B) ? LB : RF;
			case TN: return (face == T) ? LT : RN;
			case TF: return (face == T) ? RT : LF;
			default:
				assert_not_reached();
		}

	return LF;
}

// Corners
// edges:                   LB, LT, LN, LF, RB, RT, RN, RF, BN, BF, TN, TF
const enum_t corner1[12] = {LBN,LTN,LBN,LBF,RBN,RTN,RBN,RBF,LBN,LBF,LTN,LTF};
const enum_t corner2[12] = {LBF,LTF,LTN,LTF,RBF,RTF,RTN,RTF,RBN,RBF,RTN,RTF};

} // namespace mc

// Triangulate the cube directly, without decomposition
void bloomenthal_polygonizer::MarchingCube(const Cube& cube1)
{
	table_item_t index = 0;
	for(table_item_t i = 0; i < 8; i++)
		if(cube1.corners[i]->value >= m_Threshold)
			index += 1 << i;

	std::vector< std::vector<table_item_t> > currentindex = m_CubeTable[index];
	for(table_item_t i = 0; i < currentindex.size(); i++)
		{
			table_item_t a = 0;
			table_item_t b = 0;
			table_item_t count = 0;

			for(table_item_t j = 0; j < currentindex[i].size(); j++)
				{
					Corner* c1 = cube1.corners[mc::corner1[currentindex[i][j]]];
					Corner* c2 = cube1.corners[mc::corner2[currentindex[i][j]]];
					table_item_t c = VerticeId(c1, c2);

					if(++count > 2)
						SaveTriangle(a, b, c);

					if(count < 3)
						a = b;
					b = c;
				}
		}
}

// Create the 256 entry table for cubical polygonization
void bloomenthal_polygonizer::MakeCubeTable()
{
	for(table_item_t configuration = 0; configuration < 256; configuration++)
		{
			std::vector< std::vector<table_item_t> > triangles;

			table_item_t bits[8];
			for(table_item_t c = 0; c < 8; c++)
				bits[c] = bit_value(configuration, c);

			bool done[12];
			for(table_item_t edge = 0; edge < 12; edge++) done[edge] = false;
			for(table_item_t edge = 0; edge < 12; edge++)
				if(!done[edge] && (bits[mc::corner1[edge]] != bits[mc::corner2[edge]]))
					{
						std::vector<table_item_t> triangle;

						// Get face that is to right of edge from pos to neg corner
						table_item_t face = bits[mc::corner1[edge]] ? mc::rightface[edge] : mc::leftface[edge];

						table_item_t startingedge = edge, currentedge = edge;
						do
							{
							currentedge = mc::next_edge(currentedge, face);
							done[currentedge] = true;

							if(bits[mc::corner1[currentedge]] != bits[mc::corner2[currentedge]])
								{
									triangle.push_back(currentedge);

									// face adjoining edge that is not the given face
									if(face == mc::leftface[currentedge])
										face = mc::rightface[currentedge];
									else
										face = mc::leftface[currentedge];
								}
							}
						while(currentedge != startingedge);

						triangles.push_back(triangle);
					}

			m_CubeTable.push_back(triangles);
		}

	for(table_item_t i = 0; i < m_CubeTable.size(); i++)
		for(table_item_t j = 0; j < m_CubeTable[i].size(); j++)
			reverse(m_CubeTable[i][j].begin(), m_CubeTable[i][j].end());
}

//**** Storage ****

// Given cube at lattice (i, j, k), and four corners of face,
// if surface crosses face, compute other four corners of adjacent cube
// and add new cube to cube stack

void bloomenthal_polygonizer::TestFace(const Location& facelocation, Cube& old, int face, int c1, int c2, int c3, int c4)
{
	// No surface crossing?
	bool pos = old.corners[c1]->value >= m_Threshold;
	if(((old.corners[c2]->value >= m_Threshold) == pos) &&
		((old.corners[c3]->value >= m_Threshold) == pos) &&
		((old.corners[c4]->value >= m_Threshold) == pos))
		return;

	// Out of bounds?
	if(m_keep_within_limits && !(m_MinCorner <= facelocation && facelocation < m_MaxCorner))
		return;

	// Already visited?
	if(mark_center(facelocation))
		return;

	// Create new cube and add it to top of stack
	Cube newc(facelocation);

	const int facebit[6] = {2, 2, 1, 1, 0, 0};
	int bit = facebit[face];
	newc.corners[invert_bit(c1, bit)] = old.corners[c1];
	newc.corners[invert_bit(c2, bit)] = old.corners[c2];
	newc.corners[invert_bit(c3, bit)] = old.corners[c3];
	newc.corners[invert_bit(c4, bit)] = old.corners[c4];

	for(int n = 0; n < 8; n++)
		if(!newc.corners[n])
			newc.corners[n] = get_cached_corner(facelocation + Location(bit_value(n, 2), bit_value(n, 1), bit_value(n, 0)));

	m_active_cubes.push(newc);
}

// Return the gradient at Location l
vertex_t bloomenthal_polygonizer::normal(const vertex_t& Point)
{
	double delta = m_VoxelSize / static_cast<double>(RES*RES);

	double f = m_FieldFunctor.implicit_value(Point);
	double gx = m_FieldFunctor.implicit_value(Point + vertex_t(delta, 0, 0)) - f;
	double gy = m_FieldFunctor.implicit_value(Point + vertex_t(0, delta, 0)) - f;
	double gz = m_FieldFunctor.implicit_value(Point + vertex_t(0, 0, delta)) - f;
	f = std::sqrt(gx*gx + gy*gy + gz*gz);
	if(f != 0)
		{
			gx /= f;
			gy /= f;
			gz /= f;
		}

	return vertex_t(gx, gy, gz);
}

// Return cached corner with the given lattice Location
bloomenthal_polygonizer::Corner* bloomenthal_polygonizer::get_cached_corner(const Location& L)
{
	Corner* c = get_corner(L);
	if(!c)
		{
			c = new Corner(L);
			c->p = location_vertex(L);
			c->value = m_FieldFunctor.implicit_value(c->p);

			m_Corners.insert(L, c);
		}

	return c;
}

// Save a triangle
void bloomenthal_polygonizer::SaveTriangle(unsigned long u, unsigned long v, unsigned long w)
{
	polygon_t triangle;
	triangle.push_back(u);
	triangle.push_back(v);
	triangle.push_back(w);

	m_Polygons.push_back(triangle);
}

// Return index for vertex on edge
int bloomenthal_polygonizer::VerticeId(Corner *c1, Corner *c2)
// c1->value and c2->value are presumed one on each side of the equipotential surface
{
	int vid = m_Edges.GetValue(Edge(c1->l, c2->l));
	if(vid != -1)
		{
			// Has been previously computed, return saved index
			return vid;
		}

	// Compute index, save and return it
	vertex_t p;
	Converge(c1->p, c2->p, c1->value, p);
	m_Vertices.push_back(p);
	m_normals.push_back(normal(p));

	vid = m_Vertices.size() - 1;
	m_Edges.push_back(Edge(c1->l, c2->l, vid));

	return vid;
}

// From two points of differing sign, converge to zero crossing
void bloomenthal_polygonizer::Converge(const vertex_t& p1, const vertex_t& p2, double v, vertex_t& point)
{
	vertex_t pos = p1;
	vertex_t neg = p2;

	if(v < m_Threshold)
		std::swap(pos, neg);

	point = 0.5 * (pos + neg);

	for(int iter = 0; iter < RES; iter++)
		{
			if(m_FieldFunctor.implicit_value(point) >= m_Threshold)
				pos = point;
			else
				neg = point;

			point = 0.5 * (pos + neg);
		}
}


