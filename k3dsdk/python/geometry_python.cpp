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

#include "instance_wrapper_python.h"
#include "geometry_python.h"
#include "mesh_python.h"
#include "owned_instance_wrapper_python.h"
#include "selection_python.h"

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
		class storage
		{
		public:
			typedef owned_instance_wrapper<k3d::geometry::point_selection::storage> wrapper;

			static object index_begin(wrapper& Self) { return wrap(Self.wrapped().index_begin); }
			static object index_end(wrapper& Self) { return wrap(Self.wrapped().index_end); }
			static object weight(wrapper& Self) { return wrap(Self.wrapped().weight); }
		};

		static object create(k3d::selection::set& Set)
		{
			return wrap_owned(k3d::geometry::point_selection::create(Set));
		}

		static object validate(selection_storage_wrapper& Storage)
		{
			return wrap_owned(k3d::geometry::point_selection::validate(Storage.wrapped()));
		}

		static void merge(storage::wrapper& Storage, python::mesh& Mesh)
		{
			k3d::geometry::point_selection::merge(Storage.wrapped(), Mesh.wrapped());
		}
	};

	class primitive_selection
	{
	public:
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

		static object create(k3d::selection::set& Set)
		{
			return wrap_owned(k3d::geometry::primitive_selection::create(Set));
		}

		static object validate(selection_storage_wrapper& Storage)
		{
			return wrap_owned(k3d::geometry::primitive_selection::validate(Storage.wrapped()));
		}

		static void merge(storage::wrapper& Storage, python::mesh& Mesh)
		{
			k3d::geometry::primitive_selection::merge(Storage.wrapped(), Mesh.wrapped());
		}
	};
};

void define_namespace_geometry()
{
	scope outer = class_<geometry>("geometry", no_init)
		;

	{
		scope inner = class_<geometry::point_selection>("point_selection", no_init)
			.def("create", &geometry::point_selection::create)
			.staticmethod("create")
			.def("validate", &geometry::point_selection::validate)
			.staticmethod("validate")
			.def("merge", &geometry::point_selection::merge)
			.staticmethod("merge")
			;

		class_<geometry::point_selection::storage::wrapper>("storage", no_init)
			.def("index_begin", &geometry::point_selection::storage::index_begin)
			.def("index_end", &geometry::point_selection::storage::index_end)
			.def("weight", &geometry::point_selection::storage::weight)
			;
	}

	{
		scope inner = class_<geometry::primitive_selection>("primitive_selection", no_init)
			.def("create", &geometry::primitive_selection::create)
			.staticmethod("create")
			.def("validate", &geometry::primitive_selection::validate)
			.staticmethod("validate")
			.def("merge", &geometry::primitive_selection::merge)
			.staticmethod("merge")
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
}

} // namespace python

} // namespace k3d

