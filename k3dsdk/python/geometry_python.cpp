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

#include <k3dsdk/python/instance_wrapper_python.h>
#include <k3dsdk/python/geometry_python.h>
#include <k3dsdk/python/mesh_python.h>
#include <k3dsdk/python/owned_instance_wrapper_python.h>
#include <k3dsdk/python/selection_python.h>

#include <k3dsdk/iomanip.h>
#include <k3dsdk/geometry.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// geometry 

class geometry
{
public:
	class point_selection
	{
	public:
		class const_storage
		{
		public:
			typedef owned_instance_wrapper<k3d::geometry::point_selection::const_storage> wrapper;

			static object index_begin(wrapper& Self) { return wrap(Self.wrapped().index_begin); }
			static object index_end(wrapper& Self) { return wrap(Self.wrapped().index_end); }
			static object weight(wrapper& Self) { return wrap(Self.wrapped().weight); }
		};

		class storage
		{
		public:
			typedef owned_instance_wrapper<k3d::geometry::point_selection::storage> wrapper;

			static object index_begin(wrapper& Self) { return wrap(Self.wrapped().index_begin); }
			static object index_end(wrapper& Self) { return wrap(Self.wrapped().index_end); }
			static object weight(wrapper& Self) { return wrap(Self.wrapped().weight); }
		};

		static object create1(k3d::selection::set& Set)
		{
			return wrap_owned(k3d::geometry::point_selection::create(Set));
		}

		static object create2(k3d::selection::set& Set, const double_t Weight)
		{
			return wrap_owned(k3d::geometry::point_selection::create(Set, Weight));
		}

		static object validate(selection_storage_wrapper& Storage)
		{
			return wrap_owned(k3d::geometry::point_selection::validate(Storage.wrapped()));
		}

		static void append1(storage::wrapper& Storage, const double_t Weight)
		{
			k3d::geometry::point_selection::append(Storage.wrapped(), Weight);
		}

		static void append2(storage::wrapper& Storage, const uint_t Begin, const uint_t End, const double_t Weight)
		{
			k3d::geometry::point_selection::append(Storage.wrapped(), Begin, End, Weight);
		}

		static void merge(const_storage::wrapper& Storage, mesh_wrapper& Mesh)
		{
			k3d::geometry::point_selection::merge(Storage.wrapped(), Mesh.wrapped());
		}
	};

	class primitive_selection
	{
	public:
		class const_storage
		{
		public:
			typedef owned_instance_wrapper<k3d::geometry::primitive_selection::const_storage> wrapper;

			static object primitive_begin(wrapper& Self) { return wrap(Self.wrapped().primitive_begin); }
			static object primitive_end(wrapper& Self) { return wrap(Self.wrapped().primitive_end); }
			static object primitive_selection_type(wrapper& Self) { return wrap(Self.wrapped().primitive_selection_type); }
			static object primitive_first_range(wrapper& Self) { return wrap(Self.wrapped().primitive_first_range); }
			static object primitive_range_count(wrapper& Self) { return wrap(Self.wrapped().primitive_range_count); }
			static object index_begin(wrapper& Self) { return wrap(Self.wrapped().index_begin); }
			static object index_end(wrapper& Self) { return wrap(Self.wrapped().index_end); }
			static object weight(wrapper& Self) { return wrap(Self.wrapped().weight); }
		};

		class storage
		{
		public:
			typedef owned_instance_wrapper<k3d::geometry::primitive_selection::storage> wrapper;

			static object primitive_begin(wrapper& Self) { return wrap(Self.wrapped().primitive_begin); }
			static object primitive_end(wrapper& Self) { return wrap(Self.wrapped().primitive_end); }
			static object primitive_selection_type(wrapper& Self) { return wrap(Self.wrapped().primitive_selection_type); }
			static object primitive_first_range(wrapper& Self) { return wrap(Self.wrapped().primitive_first_range); }
			static object primitive_range_count(wrapper& Self) { return wrap(Self.wrapped().primitive_range_count); }
			static object index_begin(wrapper& Self) { return wrap(Self.wrapped().index_begin); }
			static object index_end(wrapper& Self) { return wrap(Self.wrapped().index_end); }
			static object weight(wrapper& Self) { return wrap(Self.wrapped().weight); }
		};

		static object create1(k3d::selection::set& Set)
		{
			return wrap_owned(k3d::geometry::primitive_selection::create(Set));
		}

		static object create2(k3d::selection::set& Set, const k3d::selection::type SelectionType)
		{
			return wrap_owned(k3d::geometry::primitive_selection::create(Set, SelectionType));
		}

		static object create3(k3d::selection::set& Set, const uint_t PrimitiveBegin, const uint_t PrimitiveEnd, const k3d::selection::type SelectionType)
		{
			return wrap_owned(k3d::geometry::primitive_selection::create(Set, PrimitiveBegin, PrimitiveEnd, SelectionType));
		}

		static object validate(selection_storage_wrapper& Storage)
		{
			return wrap_owned(k3d::geometry::primitive_selection::validate(Storage.wrapped()));
		}

		static void append1(storage::wrapper& Storage, const uint_t PrimitiveBegin, const uint_t PrimitiveEnd, const k3d::selection::type SelectionType, const uint_t Begin, const uint_t End, const double_t Weight)
		{
			k3d::geometry::primitive_selection::append(Storage.wrapped(), PrimitiveBegin, PrimitiveEnd, SelectionType, Begin, End, Weight);
		}

		static void append2(storage::wrapper& Storage, const k3d::selection::type SelectionType, const uint_t Begin, const uint_t End, const double_t Weight)
		{
			k3d::geometry::primitive_selection::append(Storage.wrapped(), SelectionType, Begin, End, Weight);
		}

		static void append3(storage::wrapper& Storage, const k3d::selection::type SelectionType, const double_t Weight)
		{
			k3d::geometry::primitive_selection::append(Storage.wrapped(), SelectionType, Weight);
		}

		static void append4(storage::wrapper& Storage, const uint_t Begin, const uint_t End, const double_t Weight)
		{
			k3d::geometry::primitive_selection::append(Storage.wrapped(), Begin, End, Weight);
		}

		static void append5(storage::wrapper& Storage, const double_t Weight)
		{
			k3d::geometry::primitive_selection::append(Storage.wrapped(), Weight);
		}

		static void merge(const_storage::wrapper& Storage, mesh_wrapper& Mesh)
		{
			k3d::geometry::primitive_selection::merge(Storage.wrapped(), Mesh.wrapped());
		}
	};

	class selection
	{
	public:
	};
};

void define_namespace_geometry()
{
	scope outer = class_<geometry>("geometry", no_init)
		;

	{
		scope inner = class_<geometry::point_selection>("point_selection", no_init)
			.def("create", &geometry::point_selection::create1)
			.def("create", &geometry::point_selection::create2)
			.staticmethod("create")
			.def("validate", &geometry::point_selection::validate)
			.staticmethod("validate")
			.def("append", &geometry::point_selection::append1)
			.def("append", &geometry::point_selection::append2)
			.staticmethod("append")
			.def("merge", &geometry::point_selection::merge)
			.staticmethod("merge")
			;

		class_<geometry::point_selection::const_storage::wrapper>("const_storage", no_init)
			.def("index_begin", &geometry::point_selection::const_storage::index_begin)
			.def("index_end", &geometry::point_selection::const_storage::index_end)
			.def("weight", &geometry::point_selection::const_storage::weight)
			;

		class_<geometry::point_selection::storage::wrapper>("storage", no_init)
			.def("index_begin", &geometry::point_selection::storage::index_begin)
			.def("index_end", &geometry::point_selection::storage::index_end)
			.def("weight", &geometry::point_selection::storage::weight)
			;
	}

	{
		scope inner = class_<geometry::primitive_selection>("primitive_selection", no_init)
			.def("create", &geometry::primitive_selection::create1)
			.def("create", &geometry::primitive_selection::create2)
			.staticmethod("create")
			.def("validate", &geometry::primitive_selection::validate)
			.staticmethod("validate")
			.def("append", &geometry::primitive_selection::append1)
			.def("append", &geometry::primitive_selection::append2)
			.def("append", &geometry::primitive_selection::append3)
			.def("append", &geometry::primitive_selection::append4)
			.def("append", &geometry::primitive_selection::append5)
			.staticmethod("append")
			.def("merge", &geometry::primitive_selection::merge)
			.staticmethod("merge")
			;

		class_<geometry::primitive_selection::const_storage::wrapper>("const_storage", no_init)
			.def("primitive_begin", &geometry::primitive_selection::const_storage::primitive_begin)
			.def("primitive_end", &geometry::primitive_selection::const_storage::primitive_end)
			.def("primitive_selection_type", &geometry::primitive_selection::const_storage::primitive_selection_type)
			.def("primitive_first_range", &geometry::primitive_selection::const_storage::primitive_first_range)
			.def("primitive_range_count", &geometry::primitive_selection::const_storage::primitive_range_count)
			.def("index_begin", &geometry::primitive_selection::const_storage::index_begin)
			.def("index_end", &geometry::primitive_selection::const_storage::index_end)
			.def("weight", &geometry::primitive_selection::const_storage::weight)
			;

		class_<geometry::primitive_selection::storage::wrapper>("storage", no_init)
			.def("primitive_begin", &geometry::primitive_selection::storage::primitive_begin)
			.def("primitive_end", &geometry::primitive_selection::storage::primitive_end)
			.def("primitive_selection_type", &geometry::primitive_selection::storage::primitive_selection_type)
			.def("primitive_first_range", &geometry::primitive_selection::storage::primitive_first_range)
			.def("primitive_range_count", &geometry::primitive_selection::storage::primitive_range_count)
			.def("index_begin", &geometry::primitive_selection::storage::index_begin)
			.def("index_end", &geometry::primitive_selection::storage::index_end)
			.def("weight", &geometry::primitive_selection::storage::weight)
			;
	}

	{
		scope inner = class_<geometry::selection>("selection", no_init)
			.def("create", &k3d::geometry::selection::create)
			.staticmethod("create")
			;
	}
}

} // namespace python

} // namespace k3d

