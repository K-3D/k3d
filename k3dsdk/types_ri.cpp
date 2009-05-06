// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Implements the k3d::ri::render class, which provides a default implementation of k3d::ri::irender
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include "types_ri.h"

namespace k3d
{

namespace ri
{

//////////////////////////////////////////////////////////////////////////////////////
// parameter

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const integer& Value) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(new typed_array<integer>(1, Value))
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const unsigned_integer& Value) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(new typed_array<unsigned_integer>(1, Value))
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const real& Value) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(new typed_array<real>(1, Value))
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const string& Value) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(new typed_array<string>(1, Value))
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const point& Value) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(new typed_array<point>(1, Value))
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const vector& Value) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(new typed_array<vector>(1, Value))
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const normal& Value) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(new typed_array<normal>(1, Value))
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const color& Value) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(new typed_array<color>(1, Value))
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const hpoint& Value) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(new typed_array<hpoint>(1, Value))
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const matrix& Value) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(new typed_array<matrix>(1, Value))
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<integer>* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<unsigned_integer>* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<real>* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<string>* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<point>* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<vector>* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<normal>* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<color>* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<hpoint>* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<matrix>* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const array* Values) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize),
	storage(Values)
{
}

parameter::parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const boost::any&) :
	name(Name),
	storage_class(StorageClass),
	tuple_size(TupleSize)
{
	assert_not_implemented();
}


} // namespace ri

} // namespace k3d

