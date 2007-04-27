#ifndef GFXTOOLS_HEAP_INCLUDED
#define GFXTOOLS_HEAP_INCLUDED

// MixKit -- Code library for multiresolution surface modeling
// Copyright (c) 1998, Michael Garland
//
// Contact: http://graphics.cs.uiuc.edu/~garland/software/qslim.html
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\brief Heap data structure
		\author Adapted for K-3D: Romain Behar (romainbehar@yahoo.com)
*/

#define NOT_IN_HEAP -47

#include <vector>

class Heapable
{
private:
	double import;
	int token;

public:
	Heapable() { not_in_heap(); }

	inline int is_in_heap() { return token != NOT_IN_HEAP; }
	inline void not_in_heap() { token = NOT_IN_HEAP; }
	inline int get_heap_pos() { return token; }
	inline void set_heap_pos(int t) { token=t; }

	inline void heap_key(double k) { import = k; }
	inline double heap_key() const { return import; }
};

class Heap
{
	std::vector<Heapable*> array;

	void place(Heapable* x, int i);
	void swap(int i, int j);

	int parent(int i) { return (i-1)/2; }
	int left(int i) { return 2*i+1; }
	int right(int i) { return 2*i+2; }

	void upheap(int i);
	void downheap(int i);

	void insert(Heapable*, double);
	void update(Heapable*, double);

public:
	Heap() {}
	Heap(int s) {}

	Heapable* ref(unsigned long i) { return array[i]; }

	void insert(Heapable* t) { insert(t, t->heap_key()); }
	void update(Heapable* t) { update(t, t->heap_key()); }

	Heapable* item(unsigned long i) { return ref(i); }

	Heapable* extract();
	Heapable* remove(Heapable*);
};

#endif // GFXTOOLS_HEAP_INCLUDED

