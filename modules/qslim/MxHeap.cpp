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

#include <k3dsdk/result.h>

#include "MxHeap.h"

void Heap::place(Heapable* x, int i)
{
	array[i] = x;
	x->set_heap_pos(i);
}

void Heap::swap(int i,int j)
{
	Heapable* tmp = array[i];

	place(array[j], i);
	place(tmp, j);
}

void Heap::upheap(int i)
{
	Heapable* moving = array[i];
	int index = i;
	int p = parent(i);

	while(index > 0 && moving->heap_key() > array[p]->heap_key())
		{
			place(array[p], index);
			index = p;
			p = parent(p);
		}

	if(index != i)
		place(moving, index);
}

void Heap::downheap(int i)
{
	Heapable* moving = array[i];
	int index = i;
	int l = left(i);
	int r = right(i);
	int largest;

	while(l < array.size())
		{
			if(r < array.size() && array[l]->heap_key() < array[r]->heap_key())
				largest = r;
			else
				largest = l;

			if(moving->heap_key() < array[largest]->heap_key())
				{
					place(array[largest], index);
					index = largest;
					l = left(index);
					r = right(index);
				}
			else
				break;
		}

	if(index != i)
		place(moving, index);
}

void Heap::insert(Heapable* t, double v)
{
	t->heap_key(v);

	int i = array.size();
	array.push_back(t);
	t->set_heap_pos(i);

	upheap(i);
}

void Heap::update(Heapable* t, double v)
{
	assert_warning(t->is_in_heap());
	t->heap_key(v);

	int i = t->get_heap_pos();

	if(i > 0 && v > array[parent(i)] ->heap_key())
		upheap(i);
	else
		downheap(i);
}

Heapable* Heap::extract()
{
	if(array.size() < 1)
		return 0;

	swap(0, array.size()-1);
	Heapable* dead = array.back();
	array.pop_back();

	downheap(0);

	dead->not_in_heap();

	return dead;
}

Heapable* Heap::remove(Heapable* t)
{
	if(!t->is_in_heap())
		return 0;

	int i = t->get_heap_pos();
	swap(i, array.size()-1);
	array.pop_back();
	t->not_in_heap();

	if(array[i]->heap_key() < t->heap_key())
		downheap(i);
	else
		upheap(i);

	return t;
}


