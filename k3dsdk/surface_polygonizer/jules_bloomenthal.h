#ifndef K3DSDK_SURFACE_POLYGONIZER_JULES_BLOOMENTHAL_H
#define K3DSDK_SURFACE_POLYGONIZER_JULES_BLOOMENTHAL_H

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
		\brief Declares bloomenthal_polygonizer, an implicit surface polygonizer
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "isurface_polygonizer.h"

#include <k3dsdk/vectors.h>

#include <map>
#include <stack>

// It is based on Jules Bloomenthal's work :
//
// C code from the article
// "An Implicit Surface Polygonizer"
// by Jules Bloomenthal, jbloom@beauty.gmu.edu
// in "Graphics Gems IV", Academic Press, 1994
//
// implicit.c
//     an implicit surface polygonizer, translated from Mesa
//     applications should call polygonize()
//
// Authored by Jules Bloomenthal, Xerox PARC.
// Copyright (c) Xerox Corporation, 1991.  All rights reserved.
// Permission is granted to reproduce, use and distribute this code for
// any and all purposes, provided that this notice appears in all copies.

typedef k3d::point3 vertex_t;
typedef std::vector<vertex_t> vertices_t;
typedef std::vector<unsigned long> polygon_t;
typedef std::vector<polygon_t> polygons_t;

typedef unsigned long enum_t;

// Lattice position (centered on (0, 0, 0), signed values)
class Location
{
public:
	Location(const int I = 0, const int J = 0, const int K = 0) :
		i(I),
		j(J),
		k(K)
	{
	}

	inline friend bool operator == (const Location& a, const Location& b)
	{
		return (a.i == b.i) && (a.j == b.j) && (a.k == b.k);
	}
	inline friend Location operator + (const Location& a, const Location& b)
	{
		return Location(a.i + b.i, a.j + b.j, a.k + b.k);
	}
	inline friend bool operator <= (const Location& a, const Location& b)
	{
		return (a.i <= b.i && a.j <= b.j && a.k <= b.k);
	}
	inline friend bool operator < (const Location& a, const Location& b)
	{
		return (a.i < b.i && a.j < b.j && a.k < b.k);
	}

	friend std::ostream& operator << (std::ostream& Stream, const Location& RHS)
	{
		Stream << RHS.i << " " << RHS.j << " " << RHS.k;
		return Stream;
	}

	Location Left() { return Location(i-1, j, k); }
	Location Right() { return Location(i+1, j, k); }
	Location Bottom() { return Location(i, j-1, k); }
	Location Top() { return Location(i, j+1, k); }
	Location Near() { return Location(i, j, k-1); }
	Location Far() { return Location(i, j, k+1); }

	int i;
	int j;
	int k;
};

/*
class LocationHash
{
public:
	inline int HashFunc(const Location& Value)
	{
		static const int HashBit = 5;
		static const int Mask = (1 << HashBit) - 1;
		return ((Value.i & Mask) << (HashBit*2)) | ((Value.j & Mask) << HashBit) | (Value.k & Mask);
	}
};
*/

template<typename type_t>
class LocationMap
{
public:
	typedef std::vector< std::pair<Location, type_t> > table_t;

	LocationMap() {}
	~LocationMap() {}

	void insert(const Location& loc, const type_t item)
	{
		int key = loc.i + loc.j + loc.k;
		m_table[key].push_back(std::pair<Location, type_t>(loc, item));
	}

	bool get(const Location& loc, type_t& out)
	{
		int key = loc.i + loc.j + loc.k;
		table_t& table(m_table[key]);
		for(typename table_t::const_iterator t = table.begin(); t != table.end(); t++)
			if(t->first == loc)
				{
					out = t->second;
					return true;
				}

		return false;
	}

private:
	std::map<unsigned long, std::vector< std::pair<Location, type_t> > > m_table;
};

// bloomenthal_polygonizer implementation
class bloomenthal_polygonizer
{
public:
	typedef enum
	{
		MARCHINGCUBES,
		TETRAHEDRAL
	} polygonization_t;

	bloomenthal_polygonizer(
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
		polygons_t& surface_polygons);

	~bloomenthal_polygonizer();

	bool polygonize_from_inside_point(const vertex_t& startingpoint);

	void polygonize_whole_grid();

	void cross_limits() { m_keep_within_limits = false; }

	// Cube corner
	class Corner
	{
	public:
		Location l;
		vertex_t p;
		double value;

		Corner(const Location& L) :
			l(L)
		{
		}

		inline friend bool operator == (const Corner& c1, const Corner& c2)
		{
			return (c1.l == c2.l) && (c1.p == c2.p) && (c1.value == c2.value);
		}
	};

	// Partitioning cell
	class Cube
	{
	public:
		Location l;
		Corner* corners[8];

		Cube(const Location& L) :
			l(L)
		{
			for(int i = 0; i < 8; i++)
				corners[i] = 0;
		}
	};

	class Edge
	{
	public:
		Edge(const Location& L1, const Location& L2, const int VID = -1) :
			vid(VID)
		{
			if(L1.i > L2.i || (L1.i == L2.i && (L1.j > L2.j || (L1.j == L2.j && L1.k > L2.k))))
				{
					l1 = L2;
					l2 = L1;
				}
			else
				{
					l1 = L1;
					l2 = L2;
				}
		}

		inline friend bool operator == (const Edge& e1, const Edge& e2)
		{
			return (e1.l1 == e2.l1) && (e1.l2 == e2.l2) && (e1.vid == e2.vid);
		}

		Location l1;
		Location l2;
		int vid;
	};

	class EdgeHash
	{
	private:
		static const int HashBit = 5;
		static const int Mask = (1 << HashBit) - 1;
		static const int HashSize = 1 << (3 * HashBit);

		inline int HashFunc(const Location& l)
		{
			return ((((l.i & Mask) << HashBit) | (l.j & Mask)) << HashBit) | (l.k & Mask);
		}

	public:
		EdgeHash()
			{
			edges.resize(HashSize*2);
			}

		void push_back(const Edge& Value)
			{
			int index = HashFunc(Value.l1) + HashFunc(Value.l2);
			edges[index].push_back(Value);
			}

		int GetValue(const Edge& Value)
			{
				int index = HashFunc(Value.l1) + HashFunc(Value.l2);
				for(unsigned int n = 0; n < edges[index].size(); n++)
					{
						if(edges[index][n].l1 == Value.l1 && edges[index][n].l2 == Value.l2)
							return edges[index][n].vid;
					}

				return -1;
			}

	protected:
		std::vector< std::vector<Edge> > edges;
	};

private:
	/// Polygonizer parameters

	// Polygonization type
	polygonization_t m_Decomposition;
	// Width of the partitioning cube
	double m_VoxelSize;
	// Threshold value (defining the equipotential surface)
	double m_Threshold;
	// Grid limit corners (left-bottom-near and right-top-far)
	Location m_MinCorner;
	Location m_MaxCorner;
	bool m_keep_within_limits;
	// Grid center ( Location(0, 0, 0) )
	vertex_t m_GridOrigin;
	// Implicit function
	implicit_functor& m_FieldFunctor;
	// Surface storage
	vertices_t& m_Vertices;
	vertices_t& m_normals;
	polygons_t& m_Polygons;

	/// Temp storage

	// Active cubes
	std::stack<Cube> m_active_cubes;

	// Centers hash
	LocationMap<bool> m_centers;
	// Return true if already set, otherwise set and return false
	bool mark_center(const Location& l)
	{
		bool out;
		if(m_centers.get(l, out))
			return true;

		m_centers.insert(l, true);
		return false;
	}

	// Corners hash
	LocationMap<Corner*> m_Corners;
	// Return corner if found, else return 0
	Corner* get_corner(const Location& l)
	{
		Corner* out;
		if(m_Corners.get(l, out))
			return out;

		return 0;
	}

	Corner* get_cached_corner(const Location& l);

	// Edge hash
	EdgeHash m_Edges;

	// Build fast Marching Cube tables
	typedef unsigned long table_item_t;
	std::vector< std::vector< std::vector<table_item_t> > > m_CubeTable;


	// Convert between vertex and Location
	vertex_t location_vertex(const Location& l);
	Location nearest_location(const vertex_t& p);

	void PolygonizeSurface(const Location& startinglocation);

	// Inline functions
	inline int bit_value(int number, int bit_number) { return (number >> bit_number) & 1; }
	inline int invert_bit(int i, int bit) { return i ^ (1 << bit); }

	vertex_t normal(const vertex_t& Point);

	bool SurfaceLocation(Location& startinglocation);

	// Tetrahedral Polygonization
	void TriangulateTet(const Cube& cube1, int c1, int c2, int c3, int c4);

	// Cubical Polygonization
	void MakeCubeTable();
	void MarchingCube(const Cube& cube1);

	void TestFace(const Location& facelocation, Cube& old, int face, int c1, int c2, int c3, int c4);

	int VerticeId(Corner *c1, Corner *c2);
	void Converge(const vertex_t& p1, const vertex_t& p2, double v, vertex_t& p);

	void SaveTriangle(unsigned long u, unsigned long v, unsigned long w);
};

#endif // !K3DSDK_SURFACE_POLYGONIZER_JULES_BLOOMENTHAL_H


