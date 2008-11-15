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

/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "attribute_arrays_python.h"
#include "const_attribute_arrays_python.h"
#include "const_typed_array_python.h"
#include "imaterial_python.h"
#include "instance_wrapper_python.h"
#include "mesh_python.h"
#include "typed_array_python.h"
#include "utility_python.h"

#include <k3dsdk/color.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/type_registry.h>

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

namespace detail
{

const bool validate(mesh& Mesh)
{
	return k3d::validate(Mesh.wrapped());
}

const bool is_solid(const mesh& Mesh)
{
	return k3d::is_solid(Mesh.wrapped());
}

const bool is_triangles(const mesh& Mesh)
{
	return k3d::is_triangles(Mesh.wrapped());
}

const bool is_uninitialized(const mesh& Mesh)
{
	return k3d::is_uninitialized(Mesh.wrapped());
}

///////////////////////////////////////////////////////////////////////////////////////
// wrap_const_object

template<typename return_type, typename value_type>
object wrap_const_object(const pipeline_data<value_type>& Data)
{
	if(!Data)
		return object();

	return object(return_type(*Data));
}

////////////////////////////////////////////////////////////////////////////////////////
// wrap_non_const_object

template<typename return_type, typename value_type>
object wrap_non_const_object(pipeline_data<value_type>& Data)
{
	if(!Data)
		return object();

	return object(return_type(Data.writable()));
}

////////////////////////////////////////////////////////////////////////////////////////
// create_object

template<typename return_type, typename value_type>
object create_object(pipeline_data<value_type>& Data)
{
	return object(return_type(Data.create()));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// wrap_const_array

template<typename array_type>
object wrap_const_array(const pipeline_data<array_type>& Data)
{
	if(!Data)
		return object();

	return object(instance_wrapper<const array_type>(*Data));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// wrap_non_const_array

template<typename array_type>
object wrap_non_const_array(pipeline_data<array_type>& Data)
{
	if(!Data)
		return object();

	return object(instance_wrapper<array_type>(Data.writable()));
}

////////////////////////////////////////////////////////////////////////////////////////////////
// create_array

template<typename array_type>
object create_array(pipeline_data<array_type>& Data)
{
	return object(instance_wrapper<array_type>(Data.create()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_nurbs_curve_groups

class const_nurbs_curve_groups :
	public instance_wrapper<const k3d::mesh::nurbs_curve_groups_t>
{
	typedef instance_wrapper<const k3d::mesh::nurbs_curve_groups_t> base;
public:
	const_nurbs_curve_groups() :
		base()
	{
	}

	const_nurbs_curve_groups(const k3d::mesh::nurbs_curve_groups_t* CurveGroups) :
		base(CurveGroups)
	{
	}

	const_nurbs_curve_groups(const k3d::mesh::nurbs_curve_groups_t& CurveGroups) :
		base(CurveGroups)
	{
	}

	object first_curves() { return wrap_const_array(wrapped().first_curves); }
	object curve_counts() { return wrap_const_array(wrapped().curve_counts); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object curve_first_points() { return wrap_const_array(wrapped().curve_first_points); }
	object curve_point_counts() { return wrap_const_array(wrapped().curve_point_counts); }
	object curve_orders() { return wrap_const_array(wrapped().curve_orders); }
	object curve_first_knots() { return wrap_const_array(wrapped().curve_first_knots); }
	object curve_selection() { return wrap_const_array(wrapped().curve_selection); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object curve_points() { return wrap_const_array(wrapped().curve_points); }
	object curve_point_weights() { return wrap_const_array(wrapped().curve_point_weights); }
	object curve_knots() { return wrap_const_array(wrapped().curve_knots); }
	object varying_data() { return wrap(wrapped().varying_data); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// nurbs_curve_groups

class nurbs_curve_groups :
	public instance_wrapper<k3d::mesh::nurbs_curve_groups_t>
{
	typedef instance_wrapper<k3d::mesh::nurbs_curve_groups_t> base;
public:
	nurbs_curve_groups() :
		base()
	{
	}

	nurbs_curve_groups(k3d::mesh::nurbs_curve_groups_t* CurveGroups) :
		base(CurveGroups)
	{
	}

	nurbs_curve_groups(k3d::mesh::nurbs_curve_groups_t& CurveGroups) :
		base(CurveGroups)
	{
	}

	object first_curves() { return wrap_const_array(wrapped().first_curves); }
	object curve_counts() { return wrap_const_array(wrapped().curve_counts); }
	object materials() { return wrap_const_array(wrapped().materials); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object curve_first_points() { return wrap_const_array(wrapped().curve_first_points); }
	object curve_point_counts() { return wrap_const_array(wrapped().curve_point_counts); }
	object curve_orders() { return wrap_const_array(wrapped().curve_orders); }
	object curve_first_knots() { return wrap_const_array(wrapped().curve_first_knots); }
	object curve_selection() { return wrap_const_array(wrapped().curve_selection); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object curve_points() { return wrap_const_array(wrapped().curve_points); }
	object curve_point_weights() { return wrap_const_array(wrapped().curve_point_weights); }
	object curve_knots() { return wrap_const_array(wrapped().curve_knots); }
	object varying_data() { return wrap(wrapped().varying_data); }

	object writable_first_curves() { return wrap_non_const_array(wrapped().first_curves); }
	object writable_curve_counts() { return wrap_non_const_array(wrapped().curve_counts); }
	object writable_materials() { return wrap_non_const_array(wrapped().materials); }
	object writable_constant_data() { return wrap(wrapped().constant_data); }
	object writable_curve_first_points() { return wrap_non_const_array(wrapped().curve_first_points); }
	object writable_curve_point_counts() { return wrap_non_const_array(wrapped().curve_point_counts); }
	object writable_curve_orders() { return wrap_non_const_array(wrapped().curve_orders); }
	object writable_curve_first_knots() { return wrap_non_const_array(wrapped().curve_first_knots); }
	object writable_curve_selection() { return wrap_non_const_array(wrapped().curve_selection); }
	object writable_uniform_data() { return wrap(wrapped().uniform_data); }
	object writable_curve_points() { return wrap_non_const_array(wrapped().curve_points); }
	object writable_curve_point_weights() { return wrap_non_const_array(wrapped().curve_point_weights); }
	object writable_curve_knots() { return wrap_non_const_array(wrapped().curve_knots); }
	object writable_varying_data() { return wrap(wrapped().varying_data); }

	object create_first_curves() { return create_array(wrapped().first_curves); }
	object create_curve_counts() { return create_array(wrapped().curve_counts); }
	object create_materials() { return create_array(wrapped().materials); }
	object create_curve_first_points() { return create_array(wrapped().curve_first_points); }
	object create_curve_point_counts() { return create_array(wrapped().curve_point_counts); }
	object create_curve_orders() { return create_array(wrapped().curve_orders); }
	object create_curve_first_knots() { return create_array(wrapped().curve_first_knots); }
	object create_curve_selection() { return create_array(wrapped().curve_selection); }
	object create_curve_points() { return create_array(wrapped().curve_points); }
	object create_curve_point_weights() { return create_array(wrapped().curve_point_weights); }
	object create_curve_knots() { return create_array(wrapped().curve_knots); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_bilinear_patches

class const_bilinear_patches :
	public instance_wrapper<const k3d::mesh::bilinear_patches_t>
{
	typedef instance_wrapper<const k3d::mesh::bilinear_patches_t> base;
public:
	const_bilinear_patches() :
		base()
	{
	}

	const_bilinear_patches(const k3d::mesh::bilinear_patches_t* Patches) :
		base(Patches)
	{
	}

	const_bilinear_patches(const k3d::mesh::bilinear_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object varying_data() { return wrap(wrapped().varying_data); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// bilinear_patches

class bilinear_patches :
	public instance_wrapper<k3d::mesh::bilinear_patches_t>
{
	typedef instance_wrapper<k3d::mesh::bilinear_patches_t> base;
public:
	bilinear_patches() :
		base()
	{
	}

	bilinear_patches(k3d::mesh::bilinear_patches_t* Patches) :
		base(Patches)
	{
	}

	bilinear_patches(k3d::mesh::bilinear_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object varying_data() { return wrap(wrapped().varying_data); }

	object writable_patch_selection() { return wrap_non_const_array(wrapped().patch_selection); }
	object writable_patch_materials() { return wrap_non_const_array(wrapped().patch_materials); }
	object writable_constant_data() { return wrap(wrapped().constant_data); }
	object writable_uniform_data() { return wrap(wrapped().uniform_data); }
	object writable_patch_points() { return wrap_non_const_array(wrapped().patch_points); }
	object writable_varying_data() { return wrap(wrapped().varying_data); }

	object create_patch_selection() { return create_array(wrapped().patch_selection); }
	object create_patch_materials() { return create_array(wrapped().patch_materials); }
	object create_patch_points() { return create_array(wrapped().patch_points); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_bicubic_patches

class const_bicubic_patches :
	public instance_wrapper<const k3d::mesh::bicubic_patches_t>
{
	typedef instance_wrapper<const k3d::mesh::bicubic_patches_t> base;
public:
	const_bicubic_patches() :
		base()
	{
	}

	const_bicubic_patches(const k3d::mesh::bicubic_patches_t* Patches) :
		base(Patches)
	{
	}

	const_bicubic_patches(const k3d::mesh::bicubic_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object varying_data() { return wrap(wrapped().varying_data); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// bicubic_patches

class bicubic_patches :
	public instance_wrapper<k3d::mesh::bicubic_patches_t>
{
	typedef instance_wrapper<k3d::mesh::bicubic_patches_t> base;
public:
	bicubic_patches() :
		base()
	{
	}

	bicubic_patches(k3d::mesh::bicubic_patches_t* Patches) :
		base(Patches)
	{
	}

	bicubic_patches(k3d::mesh::bicubic_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object varying_data() { return wrap(wrapped().varying_data); }

	object writable_patch_selection() { return wrap_non_const_array(wrapped().patch_selection); }
	object writable_patch_materials() { return wrap_non_const_array(wrapped().patch_materials); }
	object writable_constant_data() { return wrap(wrapped().constant_data); }
	object writable_uniform_data() { return wrap(wrapped().uniform_data); }
	object writable_patch_points() { return wrap_non_const_array(wrapped().patch_points); }
	object writable_varying_data() { return wrap(wrapped().varying_data); }

	object create_patch_selection() { return create_array(wrapped().patch_selection); }
	object create_patch_materials() { return create_array(wrapped().patch_materials); }
	object create_patch_points() { return create_array(wrapped().patch_points); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_nurbs_patches

class const_nurbs_patches :
	public instance_wrapper<const k3d::mesh::nurbs_patches_t>
{
	typedef instance_wrapper<const k3d::mesh::nurbs_patches_t> base;
public:
	const_nurbs_patches() :
		base()
	{
	}

	const_nurbs_patches(const k3d::mesh::nurbs_patches_t* Patches) :
		base(Patches)
	{
	}

	const_nurbs_patches(const k3d::mesh::nurbs_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_first_points() { return wrap_const_array(wrapped().patch_first_points); }
	object patch_u_point_counts() { return wrap_const_array(wrapped().patch_u_point_counts); }
	object patch_v_point_counts() { return wrap_const_array(wrapped().patch_v_point_counts); }
	object patch_u_orders() { return wrap_const_array(wrapped().patch_u_orders); }
	object patch_v_orders() { return wrap_const_array(wrapped().patch_v_orders); }
	object patch_u_first_knots() { return wrap_const_array(wrapped().patch_u_first_knots); }
	object patch_v_first_knots() { return wrap_const_array(wrapped().patch_v_first_knots); }
	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object patch_point_weights() { return wrap_const_array(wrapped().patch_point_weights); }
	object patch_u_knots() { return wrap_const_array(wrapped().patch_u_knots); }
	object patch_v_knots() { return wrap_const_array(wrapped().patch_v_knots); }
	object varying_data() { return wrap(wrapped().varying_data); }
	object patch_trim_curve_loop_counts() { return wrap_const_array(wrapped().patch_trim_curve_loop_counts); }
	object patch_first_trim_curve_loops() { return wrap_const_array(wrapped().patch_first_trim_curve_loops); }
	object trim_points() { return wrap_const_array(wrapped().trim_points); }
	object trim_point_selection() { return wrap_const_array(wrapped().trim_point_selection); }
	object first_trim_curves() { return wrap_const_array(wrapped().first_trim_curves); }
	object trim_curve_counts() { return wrap_const_array(wrapped().trim_curve_counts); }
	object trim_curve_loop_selection() { return wrap_const_array(wrapped().trim_curve_loop_selection); }
	object trim_curve_first_points() { return wrap_const_array(wrapped().trim_curve_first_points); }
	object trim_curve_point_counts() { return wrap_const_array(wrapped().trim_curve_point_counts); }
	object trim_curve_orders() { return wrap_const_array(wrapped().trim_curve_orders); }
	object trim_curve_first_knots() { return wrap_const_array(wrapped().trim_curve_first_knots); }
	object trim_curve_selection() { return wrap_const_array(wrapped().trim_curve_selection); }
	object trim_curve_points() { return wrap_const_array(wrapped().trim_curve_points); }
	object trim_curve_point_weights() { return wrap_const_array(wrapped().trim_curve_point_weights); }
	object trim_curve_knots() { return wrap_const_array(wrapped().trim_curve_knots); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// nurbs_patches

class nurbs_patches :
	public instance_wrapper<k3d::mesh::nurbs_patches_t>
{
	typedef instance_wrapper<k3d::mesh::nurbs_patches_t> base;
public:
	nurbs_patches() :
		base()
	{
	}

	nurbs_patches(k3d::mesh::nurbs_patches_t* Patches) :
		base(Patches)
	{
	}

	nurbs_patches(k3d::mesh::nurbs_patches_t& Patches) :
		base(Patches)
	{
	}

	object patch_first_points() { return wrap_const_array(wrapped().patch_first_points); }
	object patch_u_point_counts() { return wrap_const_array(wrapped().patch_u_point_counts); }
	object patch_v_point_counts() { return wrap_const_array(wrapped().patch_v_point_counts); }
	object patch_u_orders() { return wrap_const_array(wrapped().patch_u_orders); }
	object patch_v_orders() { return wrap_const_array(wrapped().patch_v_orders); }
	object patch_u_first_knots() { return wrap_const_array(wrapped().patch_u_first_knots); }
	object patch_v_first_knots() { return wrap_const_array(wrapped().patch_v_first_knots); }
	object patch_selection() { return wrap_const_array(wrapped().patch_selection); }
	object patch_materials() { return wrap_const_array(wrapped().patch_materials); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object patch_points() { return wrap_const_array(wrapped().patch_points); }
	object patch_point_weights() { return wrap_const_array(wrapped().patch_point_weights); }
	object patch_u_knots() { return wrap_const_array(wrapped().patch_u_knots); }
	object patch_v_knots() { return wrap_const_array(wrapped().patch_v_knots); }
	object varying_data() { return wrap(wrapped().varying_data); }
	object patch_trim_curve_loop_counts() { return wrap_const_array(wrapped().patch_trim_curve_loop_counts); }
	object patch_first_trim_curve_loops() { return wrap_const_array(wrapped().patch_first_trim_curve_loops); }
	object trim_points() { return wrap_const_array(wrapped().trim_points); }
	object trim_point_selection() { return wrap_const_array(wrapped().trim_point_selection); }
	object first_trim_curves() { return wrap_const_array(wrapped().first_trim_curves); }
	object trim_curve_counts() { return wrap_const_array(wrapped().trim_curve_counts); }
	object trim_curve_loop_selection() { return wrap_const_array(wrapped().trim_curve_loop_selection); }
	object trim_curve_first_points() { return wrap_const_array(wrapped().trim_curve_first_points); }
	object trim_curve_point_counts() { return wrap_const_array(wrapped().trim_curve_point_counts); }
	object trim_curve_orders() { return wrap_const_array(wrapped().trim_curve_orders); }
	object trim_curve_first_knots() { return wrap_const_array(wrapped().trim_curve_first_knots); }
	object trim_curve_selection() { return wrap_const_array(wrapped().trim_curve_selection); }
	object trim_curve_points() { return wrap_const_array(wrapped().trim_curve_points); }
	object trim_curve_point_weights() { return wrap_const_array(wrapped().trim_curve_point_weights); }
	object trim_curve_knots() { return wrap_const_array(wrapped().trim_curve_knots); }

	object writable_patch_first_points() { return wrap_non_const_array(wrapped().patch_first_points); }
	object writable_patch_u_point_counts() { return wrap_non_const_array(wrapped().patch_u_point_counts); }
	object writable_patch_v_point_counts() { return wrap_non_const_array(wrapped().patch_v_point_counts); }
	object writable_patch_u_orders() { return wrap_non_const_array(wrapped().patch_u_orders); }
	object writable_patch_v_orders() { return wrap_non_const_array(wrapped().patch_v_orders); }
	object writable_patch_u_first_knots() { return wrap_non_const_array(wrapped().patch_u_first_knots); }
	object writable_patch_v_first_knots() { return wrap_non_const_array(wrapped().patch_v_first_knots); }
	object writable_patch_selection() { return wrap_non_const_array(wrapped().patch_selection); }
	object writable_patch_materials() { return wrap_non_const_array(wrapped().patch_materials); }
	object writable_constant_data() { return wrap(wrapped().constant_data); }
	object writable_uniform_data() { return wrap(wrapped().uniform_data); }
	object writable_patch_points() { return wrap_non_const_array(wrapped().patch_points); }
	object writable_patch_point_weights() { return wrap_non_const_array(wrapped().patch_point_weights); }
	object writable_patch_u_knots() { return wrap_non_const_array(wrapped().patch_u_knots); }
	object writable_patch_v_knots() { return wrap_non_const_array(wrapped().patch_v_knots); }
	object writable_varying_data() { return wrap(wrapped().varying_data); }
	object writable_patch_trim_curve_loop_counts() { return wrap_non_const_array(wrapped().patch_trim_curve_loop_counts); }
	object writable_patch_first_trim_curve_loops() { return wrap_non_const_array(wrapped().patch_first_trim_curve_loops); }
	object writable_trim_points() { return wrap_non_const_array(wrapped().trim_points); }
	object writable_trim_point_selection() { return wrap_non_const_array(wrapped().trim_point_selection); }
	object writable_first_trim_curves() { return wrap_non_const_array(wrapped().first_trim_curves); }
	object writable_trim_curve_counts() { return wrap_non_const_array(wrapped().trim_curve_counts); }
	object writable_trim_curve_loop_selection() { return wrap_non_const_array(wrapped().trim_curve_loop_selection); }
	object writable_trim_curve_first_points() { return wrap_non_const_array(wrapped().trim_curve_first_points); }
	object writable_trim_curve_point_counts() { return wrap_non_const_array(wrapped().trim_curve_point_counts); }
	object writable_trim_curve_orders() { return wrap_non_const_array(wrapped().trim_curve_orders); }
	object writable_trim_curve_first_knots() { return wrap_non_const_array(wrapped().trim_curve_first_knots); }
	object writable_trim_curve_selection() { return wrap_non_const_array(wrapped().trim_curve_selection); }
	object writable_trim_curve_points() { return wrap_non_const_array(wrapped().trim_curve_points); }
	object writable_trim_curve_point_weights() { return wrap_non_const_array(wrapped().trim_curve_point_weights); }
	object writable_trim_curve_knots() { return wrap_non_const_array(wrapped().trim_curve_knots); }

	object create_patch_first_points() { return create_array(wrapped().patch_first_points); }
	object create_patch_u_point_counts() { return create_array(wrapped().patch_u_point_counts); }
	object create_patch_v_point_counts() { return create_array(wrapped().patch_v_point_counts); }
	object create_patch_u_orders() { return create_array(wrapped().patch_u_orders); }
	object create_patch_v_orders() { return create_array(wrapped().patch_v_orders); }
	object create_patch_u_first_knots() { return create_array(wrapped().patch_u_first_knots); }
	object create_patch_v_first_knots() { return create_array(wrapped().patch_v_first_knots); }
	object create_patch_selection() { return create_array(wrapped().patch_selection); }
	object create_patch_materials() { return create_array(wrapped().patch_materials); }
	object create_patch_points() { return create_array(wrapped().patch_points); }
	object create_patch_point_weights() { return create_array(wrapped().patch_point_weights); }
	object create_patch_u_knots() { return create_array(wrapped().patch_u_knots); }
	object create_patch_v_knots() { return create_array(wrapped().patch_v_knots); }
	object create_patch_trim_curve_loop_counts() { return create_array(wrapped().patch_trim_curve_loop_counts); }
	object create_patch_first_trim_curve_loops() { return create_array(wrapped().patch_first_trim_curve_loops); }
	object create_trim_points() { return create_array(wrapped().trim_points); }
	object create_trim_point_selection() { return create_array(wrapped().trim_point_selection); }
	object create_first_trim_curves() { return create_array(wrapped().first_trim_curves); }
	object create_trim_curve_counts() { return create_array(wrapped().trim_curve_counts); }
	object create_trim_curve_loop_selection() { return create_array(wrapped().trim_curve_loop_selection); }
	object create_trim_curve_first_points() { return create_array(wrapped().trim_curve_first_points); }
	object create_trim_curve_point_counts() { return create_array(wrapped().trim_curve_point_counts); }
	object create_trim_curve_orders() { return create_array(wrapped().trim_curve_orders); }
	object create_trim_curve_first_knots() { return create_array(wrapped().trim_curve_first_knots); }
	object create_trim_curve_selection() { return create_array(wrapped().trim_curve_selection); }
	object create_trim_curve_points() { return create_array(wrapped().trim_curve_points); }
	object create_trim_curve_point_weights() { return create_array(wrapped().trim_curve_point_weights); }
	object create_trim_curve_knots() { return create_array(wrapped().trim_curve_knots); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_polyhedra

class const_polyhedra :
	public instance_wrapper<const k3d::mesh::polyhedra_t>
{
	typedef instance_wrapper<const k3d::mesh::polyhedra_t> base;
public:
	const_polyhedra() :
		base()
	{
	}

	const_polyhedra(const k3d::mesh::polyhedra_t* Polyhedra) :
		base(Polyhedra)
	{
	}

	const_polyhedra(const k3d::mesh::polyhedra_t& Polyhedra) :
		base(Polyhedra)
	{
	}

	object first_faces() { return wrap_const_array(wrapped().first_faces); }
	object face_counts() { return wrap_const_array(wrapped().face_counts); }
	object types() { return wrap_const_array(wrapped().types); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object face_first_loops() { return wrap_const_array(wrapped().face_first_loops); }
	object face_loop_counts() { return wrap_const_array(wrapped().face_loop_counts); }
	object face_selection() { return wrap_const_array(wrapped().face_selection); }
	object face_materials() { return wrap_const_array(wrapped().face_materials); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object loop_first_edges() { return wrap_const_array(wrapped().loop_first_edges); }
	object edge_points() { return wrap_const_array(wrapped().edge_points); }
	object clockwise_edges() { return wrap_const_array(wrapped().clockwise_edges); }
	object edge_selection() { return wrap_const_array(wrapped().edge_selection); }
	object face_varying_data() { return wrap(wrapped().face_varying_data); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// polyhedra

class polyhedra :
	public instance_wrapper<k3d::mesh::polyhedra_t>
{
	typedef instance_wrapper<k3d::mesh::polyhedra_t> base;
public:
	polyhedra() :
		base()
	{
	}

	polyhedra(k3d::mesh::polyhedra_t* Polyhedra) :
		base(Polyhedra)
	{
	}

	polyhedra(k3d::mesh::polyhedra_t& Polyhedra) :
		base(Polyhedra)
	{
	}

	object first_faces() { return wrap_const_array(wrapped().first_faces); }
	object face_counts() { return wrap_const_array(wrapped().face_counts); }
	object types() { return wrap_const_array(wrapped().types); }
	object constant_data() { return wrap(wrapped().constant_data); }
	object face_first_loops() { return wrap_const_array(wrapped().face_first_loops); }
	object face_loop_counts() { return wrap_const_array(wrapped().face_loop_counts); }
	object face_selection() { return wrap_const_array(wrapped().face_selection); }
	object face_materials() { return wrap_const_array(wrapped().face_materials); }
	object uniform_data() { return wrap(wrapped().uniform_data); }
	object loop_first_edges() { return wrap_const_array(wrapped().loop_first_edges); }
	object edge_points() { return wrap_const_array(wrapped().edge_points); }
	object clockwise_edges() { return wrap_const_array(wrapped().clockwise_edges); }
	object edge_selection() { return wrap_const_array(wrapped().edge_selection); }
	object face_varying_data() { return wrap(wrapped().face_varying_data); }

	object writable_first_faces() { return wrap_non_const_array(wrapped().first_faces); }
	object writable_face_counts() { return wrap_non_const_array(wrapped().face_counts); }
	object writable_types() { return wrap_non_const_array(wrapped().types); }
	object writable_constant_data() { return wrap(wrapped().constant_data); }
	object writable_face_first_loops() { return wrap_non_const_array(wrapped().face_first_loops); }
	object writable_face_loop_counts() { return wrap_non_const_array(wrapped().face_loop_counts); }
	object writable_face_selection() { return wrap_non_const_array(wrapped().face_selection); }
	object writable_face_materials() { return wrap_non_const_array(wrapped().face_materials); }
	object writable_uniform_data() { return wrap(wrapped().uniform_data); }
	object writable_loop_first_edges() { return wrap_non_const_array(wrapped().loop_first_edges); }
	object writable_edge_points() { return wrap_non_const_array(wrapped().edge_points); }
	object writable_clockwise_edges() { return wrap_non_const_array(wrapped().clockwise_edges); }
	object writable_edge_selection() { return wrap_non_const_array(wrapped().edge_selection); }
	object writable_face_varying_data() { return wrap(wrapped().face_varying_data); }

	object create_first_faces() { return create_array(wrapped().first_faces); }
	object create_face_counts() { return create_array(wrapped().face_counts); }
	object create_types() { return create_array(wrapped().types); }
	object create_face_first_loops() { return create_array(wrapped().face_first_loops); }
	object create_face_loop_counts() { return create_array(wrapped().face_loop_counts); }
	object create_face_selection() { return create_array(wrapped().face_selection); }
	object create_face_materials() { return create_array(wrapped().face_materials); }
	object create_loop_first_edges() { return create_array(wrapped().loop_first_edges); }
	object create_edge_points() { return create_array(wrapped().edge_points); }
	object create_clockwise_edges() { return create_array(wrapped().clockwise_edges); }
	object create_edge_selection() { return create_array(wrapped().edge_selection); }
};

} // namespace detail

///////////////////////////////////////////////////////////////////////////////////////////////////
// mesh

mesh::mesh() :
	base()
{
}

mesh::mesh(k3d::mesh* Mesh) :
	base(Mesh)
{
}

void mesh::copy(const mesh& RHS)
{
	wrapped() = RHS.wrapped();
}

object mesh::bicubic_patches() { return detail::wrap_const_object<detail::const_bicubic_patches>(wrapped().bicubic_patches); } 
object mesh::bilinear_patches() { return detail::wrap_const_object<detail::const_bilinear_patches>(wrapped().bilinear_patches); } 
object mesh::create_bicubic_patches() { return detail::create_object<detail::bicubic_patches, k3d::mesh::bicubic_patches_t>(wrapped().bicubic_patches); }
object mesh::create_bilinear_patches() { return detail::create_object<detail::bilinear_patches, k3d::mesh::bilinear_patches_t>(wrapped().bilinear_patches); }
object mesh::create_nurbs_curve_groups() { return detail::create_object<detail::nurbs_curve_groups, k3d::mesh::nurbs_curve_groups_t>(wrapped().nurbs_curve_groups); }
object mesh::create_nurbs_patches() { return detail::create_object<detail::nurbs_patches, k3d::mesh::nurbs_patches_t>(wrapped().nurbs_patches); }
object mesh::create_point_selection() { return detail::create_array(wrapped().point_selection); } 
object mesh::create_points() { return detail::create_array(wrapped().points); }
object mesh::create_polyhedra() { return detail::create_object<detail::polyhedra, k3d::mesh::polyhedra_t>(wrapped().polyhedra); } 
object mesh::nurbs_curve_groups() { return detail::wrap_const_object<detail::const_nurbs_curve_groups>(wrapped().nurbs_curve_groups); } 
object mesh::nurbs_patches() { return detail::wrap_const_object<detail::const_nurbs_patches>(wrapped().nurbs_patches); } 
object mesh::point_selection() { return detail::wrap_const_array(wrapped().point_selection); } 
object mesh::points() { return detail::wrap_const_array(wrapped().points); } 
object mesh::polyhedra() { return detail::wrap_const_object<detail::const_polyhedra>(wrapped().polyhedra); } 
object mesh::vertex_data() { return wrap(wrapped().vertex_data); } 
object mesh::writable_bicubic_patches() { return detail::wrap_non_const_object<detail::bicubic_patches>(wrapped().bicubic_patches); } 
object mesh::writable_bilinear_patches() { return detail::wrap_non_const_object<detail::bilinear_patches>(wrapped().bilinear_patches); } 
object mesh::writable_nurbs_curve_groups() { return detail::wrap_non_const_object<detail::nurbs_curve_groups>(wrapped().nurbs_curve_groups); } 
object mesh::writable_nurbs_patches() { return detail::wrap_non_const_object<detail::nurbs_patches>(wrapped().nurbs_patches); } 
object mesh::writable_point_selection() { return detail::wrap_non_const_array(wrapped().point_selection); } 
object mesh::writable_points() { return detail::wrap_non_const_array(wrapped().points); } 
object mesh::writable_polyhedra() { return detail::wrap_non_const_object<detail::polyhedra>(wrapped().polyhedra); } 
object mesh::writable_vertex_data() { return wrap(wrapped().vertex_data); } 

static object mesh_primitives(mesh& Self)
{
	return wrap(Self.wrapped().primitives);
}

const string_t mesh::repr()
{
	std::ostringstream buffer;
	buffer << wrapped();
	return buffer.str();
}

const string_t mesh::str()
{
	std::ostringstream buffer;
	buffer << "<k3d.mesh object wrapping mesh " << &wrapped() << ">";
	return buffer.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_mesh_primitive 

static const string_t const_mesh_primitive_get_type(const_mesh_primitive_wrapper& Self)
{
	return Self.wrapped().type;
}

static object const_mesh_primitive_get_topology(const_mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().topology);
}

static object const_mesh_primitive_get_attributes(const_mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().attributes);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// mesh_primitive 

static const string_t mesh_primitive_get_type(mesh_primitive_wrapper& Self)
{
	return Self.wrapped().type;
}

static void mesh_primitive_set_type(mesh_primitive_wrapper& Self, const string_t& Type)
{
	Self.wrapped().type = Type;
}

static object mesh_primitive_get_topology(mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().topology);
}

static object mesh_primitive_get_attributes(mesh_primitive_wrapper& Self)
{
	return wrap(Self.wrapped().attributes);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// const_mesh_primitives_t

typedef instance_wrapper<const k3d::mesh::primitives_t> const_mesh_primitives_t_wrapper;

static object const_mesh_primitives_t_get_item(const_mesh_primitives_t_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item).get());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// mesh_primitives_t

typedef instance_wrapper<k3d::mesh::primitives_t> mesh_primitives_t_wrapper;

static object mesh_primitives_t_get_item(mesh_primitives_t_wrapper& Self, int Item)
{
	if(Item < 0 || Item >= Self.wrapped().size())
		throw std::out_of_range("index out-of-range");

	return wrap(Self.wrapped().at(Item).get());
}

static object mesh_primitives_t_create(mesh_primitives_t_wrapper& Self, const string_t& Type)
{
	return wrap(Self.wrapped().create(Type));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// define_namespace_mesh 

void define_namespace_mesh()
{
	class_<detail::const_nurbs_curve_groups>("const_nurbs_curve_groups",
		"Stores an immutable (read-only) collection of NURBS curve primitives.")
		.def("first_curves", &detail::const_nurbs_curve_groups::first_curves)
		.def("curve_counts", &detail::const_nurbs_curve_groups::curve_counts)
		.def("materials", &detail::const_nurbs_curve_groups::materials)
		.def("constant_data", &detail::const_nurbs_curve_groups::constant_data)
		.def("curve_first_points", &detail::const_nurbs_curve_groups::curve_first_points)
		.def("curve_point_counts", &detail::const_nurbs_curve_groups::curve_point_counts)
		.def("curve_orders", &detail::const_nurbs_curve_groups::curve_orders)
		.def("curve_first_knots", &detail::const_nurbs_curve_groups::curve_first_knots)
		.def("curve_selection", &detail::const_nurbs_curve_groups::curve_selection)
		.def("uniform_data", &detail::const_nurbs_curve_groups::uniform_data)
		.def("varying_data", &detail::const_nurbs_curve_groups::varying_data)
		.def("curve_points", &detail::const_nurbs_curve_groups::curve_points)
		.def("curve_point_weights", &detail::const_nurbs_curve_groups::curve_point_weights)
		.def("curve_knots", &detail::const_nurbs_curve_groups::curve_knots);

	class_<detail::nurbs_curve_groups>("nurbs_curve_groups",
		"Stores a mutable (read-write) collection of NURBS curve primitives.")
		.def("first_curves", &detail::nurbs_curve_groups::first_curves)
		.def("curve_counts", &detail::nurbs_curve_groups::curve_counts)
		.def("materials", &detail::nurbs_curve_groups::materials)
		.def("constant_data", &detail::nurbs_curve_groups::constant_data)
		.def("curve_first_points", &detail::nurbs_curve_groups::curve_first_points)
		.def("curve_point_counts", &detail::nurbs_curve_groups::curve_point_counts)
		.def("curve_orders", &detail::nurbs_curve_groups::curve_orders)
		.def("curve_first_knots", &detail::nurbs_curve_groups::curve_first_knots)
		.def("curve_selection", &detail::nurbs_curve_groups::curve_selection)
		.def("uniform_data", &detail::nurbs_curve_groups::uniform_data)
		.def("varying_data", &detail::nurbs_curve_groups::varying_data)
		.def("curve_points", &detail::nurbs_curve_groups::curve_points)
		.def("curve_point_weights", &detail::nurbs_curve_groups::curve_point_weights)
		.def("curve_knots", &detail::nurbs_curve_groups::curve_knots)

		.def("writable_first_curves", &detail::nurbs_curve_groups::writable_first_curves)
		.def("writable_curve_counts", &detail::nurbs_curve_groups::writable_curve_counts)
		.def("writable_materials", &detail::nurbs_curve_groups::writable_materials)
		.def("writable_constant_data", &detail::nurbs_curve_groups::writable_constant_data)
		.def("writable_curve_first_points", &detail::nurbs_curve_groups::writable_curve_first_points)
		.def("writable_curve_point_counts", &detail::nurbs_curve_groups::writable_curve_point_counts)
		.def("writable_curve_orders", &detail::nurbs_curve_groups::writable_curve_orders)
		.def("writable_curve_first_knots", &detail::nurbs_curve_groups::writable_curve_first_knots)
		.def("writable_curve_selection", &detail::nurbs_curve_groups::writable_curve_selection)
		.def("writable_uniform_data", &detail::nurbs_curve_groups::writable_uniform_data)
		.def("writable_varying_data", &detail::nurbs_curve_groups::writable_varying_data)
		.def("writable_curve_points", &detail::nurbs_curve_groups::writable_curve_points)
		.def("writable_curve_point_weights", &detail::nurbs_curve_groups::writable_curve_point_weights)
		.def("writable_curve_knots", &detail::nurbs_curve_groups::writable_curve_knots)

		.def("create_first_curves", &detail::nurbs_curve_groups::create_first_curves)
		.def("create_curve_counts", &detail::nurbs_curve_groups::create_curve_counts)
		.def("create_materials", &detail::nurbs_curve_groups::create_materials)
		.def("create_curve_first_points", &detail::nurbs_curve_groups::create_curve_first_points)
		.def("create_curve_point_counts", &detail::nurbs_curve_groups::create_curve_point_counts)
		.def("create_curve_orders", &detail::nurbs_curve_groups::create_curve_orders)
		.def("create_curve_first_knots", &detail::nurbs_curve_groups::create_curve_first_knots)
		.def("create_curve_selection", &detail::nurbs_curve_groups::create_curve_selection)
		.def("create_curve_points", &detail::nurbs_curve_groups::create_curve_points)
		.def("create_curve_point_weights", &detail::nurbs_curve_groups::create_curve_point_weights)
		.def("create_curve_knots", &detail::nurbs_curve_groups::create_curve_knots);

	class_<detail::const_bilinear_patches>("const_bilinear_patches",
		"Stores an immutable (read-only) collection of bilinear patch primitives.")
		.def("patch_selection", &detail::const_bilinear_patches::patch_selection)
		.def("patch_materials", &detail::const_bilinear_patches::patch_materials)
		.def("constant_data", &detail::const_bilinear_patches::constant_data)
		.def("uniform_data", &detail::const_bilinear_patches::uniform_data)
		.def("patch_points", &detail::const_bilinear_patches::patch_points)
		.def("varying_data", &detail::const_bilinear_patches::varying_data);

	class_<detail::bilinear_patches>("bilinear_patches",
		"Stores a mutable (read-write) collection of bilinear patch primitives.")
		.def("patch_selection", &detail::bilinear_patches::patch_selection)
		.def("patch_materials", &detail::bilinear_patches::patch_materials)
		.def("constant_data", &detail::bilinear_patches::constant_data)
		.def("uniform_data", &detail::bilinear_patches::uniform_data)
		.def("patch_points", &detail::bilinear_patches::patch_points)
		.def("varying_data", &detail::bilinear_patches::varying_data)

		.def("writable_patch_selection", &detail::bilinear_patches::writable_patch_selection)
		.def("writable_patch_materials", &detail::bilinear_patches::writable_patch_materials)
		.def("writable_constant_data", &detail::bilinear_patches::writable_constant_data)
		.def("writable_uniform_data", &detail::bilinear_patches::writable_uniform_data)
		.def("writable_patch_points", &detail::bilinear_patches::writable_patch_points)
		.def("writable_varying_data", &detail::bilinear_patches::writable_varying_data)

		.def("create_patch_selection", &detail::bilinear_patches::create_patch_selection)
		.def("create_patch_materials", &detail::bilinear_patches::create_patch_materials)
		.def("create_patch_points", &detail::bilinear_patches::create_patch_points);

	class_<detail::const_bicubic_patches>("const_bicubic_patches",
		"Stores an immutable (read-only) collection of bicubic patch primitives.")
		.def("patch_selection", &detail::const_bicubic_patches::patch_selection)
		.def("patch_materials", &detail::const_bicubic_patches::patch_materials)
		.def("constant_data", &detail::const_bicubic_patches::constant_data)
		.def("uniform_data", &detail::const_bicubic_patches::uniform_data)
		.def("patch_points", &detail::const_bicubic_patches::patch_points)
		.def("varying_data", &detail::const_bicubic_patches::varying_data);

	class_<detail::bicubic_patches>("bicubic_patches",
		"Stores a mutable (read-write) collection of bicubic patch primitives.")
		.def("patch_selection", &detail::bicubic_patches::patch_selection)
		.def("patch_materials", &detail::bicubic_patches::patch_materials)
		.def("constant_data", &detail::bicubic_patches::constant_data)
		.def("uniform_data", &detail::bicubic_patches::uniform_data)
		.def("patch_points", &detail::bicubic_patches::patch_points)
		.def("varying_data", &detail::bicubic_patches::varying_data)

		.def("writable_patch_selection", &detail::bicubic_patches::writable_patch_selection)
		.def("writable_patch_materials", &detail::bicubic_patches::writable_patch_materials)
		.def("writable_constant_data", &detail::bicubic_patches::writable_constant_data)
		.def("writable_uniform_data", &detail::bicubic_patches::writable_uniform_data)
		.def("writable_patch_points", &detail::bicubic_patches::writable_patch_points)
		.def("writable_varying_data", &detail::bicubic_patches::writable_varying_data)

		.def("create_patch_selection", &detail::bicubic_patches::create_patch_selection)
		.def("create_patch_materials", &detail::bicubic_patches::create_patch_materials)
		.def("create_patch_points", &detail::bicubic_patches::create_patch_points);

	class_<detail::const_nurbs_patches>("const_nurbs_patches",
		"Stores an immutable (read-only) collection of NURBS patch primitives.")
		.def("patch_first_points", &detail::const_nurbs_patches::patch_first_points)
		.def("patch_u_point_counts", &detail::const_nurbs_patches::patch_u_point_counts)
		.def("patch_v_point_counts", &detail::const_nurbs_patches::patch_v_point_counts)
		.def("patch_u_orders", &detail::const_nurbs_patches::patch_u_orders)
		.def("patch_v_orders", &detail::const_nurbs_patches::patch_v_orders)
		.def("patch_u_first_knots", &detail::const_nurbs_patches::patch_u_first_knots)
		.def("patch_v_first_knots", &detail::const_nurbs_patches::patch_v_first_knots)
		.def("patch_selection", &detail::const_nurbs_patches::patch_selection)
		.def("patch_materials", &detail::const_nurbs_patches::patch_materials)
		.def("constant_data", &detail::const_nurbs_patches::constant_data)
		.def("uniform_data", &detail::const_nurbs_patches::uniform_data)
		.def("patch_points", &detail::const_nurbs_patches::patch_points)
		.def("patch_point_weights", &detail::const_nurbs_patches::patch_point_weights)
		.def("patch_u_knots", &detail::const_nurbs_patches::patch_u_knots)
		.def("patch_v_knots", &detail::const_nurbs_patches::patch_v_knots)
		.def("varying_data", &detail::const_nurbs_patches::varying_data)
		.def("patch_trim_curve_loop_counts", &detail::const_nurbs_patches::patch_trim_curve_loop_counts)
		.def("patch_first_trim_curve_loops", &detail::const_nurbs_patches::patch_first_trim_curve_loops)
		.def("trim_points", &detail::const_nurbs_patches::trim_points)
		.def("trim_point_selection", &detail::const_nurbs_patches::trim_point_selection)
		.def("first_trim_curves", &detail::const_nurbs_patches::first_trim_curves)
		.def("trim_curve_counts", &detail::const_nurbs_patches::trim_curve_counts)
		.def("trim_curve_loop_selection", &detail::const_nurbs_patches::trim_curve_loop_selection)
		.def("trim_curve_first_points", &detail::const_nurbs_patches::trim_curve_first_points)
		.def("trim_curve_point_counts", &detail::const_nurbs_patches::trim_curve_point_counts)
		.def("trim_curve_orders", &detail::const_nurbs_patches::trim_curve_orders)
		.def("trim_curve_first_knots", &detail::const_nurbs_patches::trim_curve_first_knots)
		.def("trim_curve_selection", &detail::const_nurbs_patches::trim_curve_selection)
		.def("trim_curve_points", &detail::const_nurbs_patches::trim_curve_points)
		.def("trim_curve_point_weights", &detail::const_nurbs_patches::trim_curve_point_weights)
		.def("trim_curve_knots", &detail::const_nurbs_patches::trim_curve_knots);

	class_<detail::nurbs_patches>("nurbs_patches",
		"Stores a mutable (read-write) collection of NURBS patch primitives.")
		.def("patch_first_points", &detail::nurbs_patches::patch_first_points)
		.def("patch_u_point_counts", &detail::nurbs_patches::patch_u_point_counts)
		.def("patch_v_point_counts", &detail::nurbs_patches::patch_v_point_counts)
		.def("patch_u_orders", &detail::nurbs_patches::patch_u_orders)
		.def("patch_v_orders", &detail::nurbs_patches::patch_v_orders)
		.def("patch_u_first_knots", &detail::nurbs_patches::patch_u_first_knots)
		.def("patch_v_first_knots", &detail::nurbs_patches::patch_v_first_knots)
		.def("patch_selection", &detail::nurbs_patches::patch_selection)
		.def("patch_materials", &detail::nurbs_patches::patch_materials)
		.def("constant_data", &detail::nurbs_patches::constant_data)
		.def("uniform_data", &detail::nurbs_patches::uniform_data)
		.def("patch_points", &detail::nurbs_patches::patch_points)
		.def("patch_point_weights", &detail::nurbs_patches::patch_point_weights)
		.def("patch_u_knots", &detail::nurbs_patches::patch_u_knots)
		.def("patch_v_knots", &detail::nurbs_patches::patch_v_knots)
		.def("varying_data", &detail::nurbs_patches::varying_data)
		.def("patch_trim_curve_loop_counts", &detail::nurbs_patches::patch_trim_curve_loop_counts)
		.def("patch_first_trim_curve_loops", &detail::nurbs_patches::patch_first_trim_curve_loops)
		.def("trim_points", &detail::nurbs_patches::trim_points)
		.def("trim_point_selection", &detail::nurbs_patches::trim_point_selection)
		.def("first_trim_curves", &detail::nurbs_patches::first_trim_curves)
		.def("trim_curve_counts", &detail::nurbs_patches::trim_curve_counts)
		.def("trim_curve_loop_selection", &detail::nurbs_patches::trim_curve_loop_selection)
		.def("trim_curve_first_points", &detail::nurbs_patches::trim_curve_first_points)
		.def("trim_curve_point_counts", &detail::nurbs_patches::trim_curve_point_counts)
		.def("trim_curve_orders", &detail::nurbs_patches::trim_curve_orders)
		.def("trim_curve_first_knots", &detail::nurbs_patches::trim_curve_first_knots)
		.def("trim_curve_selection", &detail::nurbs_patches::trim_curve_selection)
		.def("trim_curve_points", &detail::nurbs_patches::trim_curve_points)
		.def("trim_curve_point_weights", &detail::nurbs_patches::trim_curve_point_weights)
		.def("trim_curve_knots", &detail::nurbs_patches::trim_curve_knots)

		.def("writable_patch_first_points", &detail::nurbs_patches::writable_patch_first_points)
		.def("writable_patch_u_point_counts", &detail::nurbs_patches::writable_patch_u_point_counts)
		.def("writable_patch_v_point_counts", &detail::nurbs_patches::writable_patch_v_point_counts)
		.def("writable_patch_u_orders", &detail::nurbs_patches::writable_patch_u_orders)
		.def("writable_patch_v_orders", &detail::nurbs_patches::writable_patch_v_orders)
		.def("writable_patch_u_first_knots", &detail::nurbs_patches::writable_patch_u_first_knots)
		.def("writable_patch_v_first_knots", &detail::nurbs_patches::writable_patch_v_first_knots)
		.def("writable_patch_selection", &detail::nurbs_patches::writable_patch_selection)
		.def("writable_patch_materials", &detail::nurbs_patches::writable_patch_materials)
		.def("writable_constant_data", &detail::nurbs_patches::writable_constant_data)
		.def("writable_uniform_data", &detail::nurbs_patches::writable_uniform_data)
		.def("writable_patch_points", &detail::nurbs_patches::writable_patch_points)
		.def("writable_patch_point_weights", &detail::nurbs_patches::writable_patch_point_weights)
		.def("writable_patch_u_knots", &detail::nurbs_patches::writable_patch_u_knots)
		.def("writable_patch_v_knots", &detail::nurbs_patches::writable_patch_v_knots)
		.def("writable_varying_data", &detail::nurbs_patches::writable_varying_data)
		.def("writable_patch_trim_curve_loop_counts", &detail::nurbs_patches::writable_patch_trim_curve_loop_counts)
		.def("writable_patch_first_trim_curve_loops", &detail::nurbs_patches::writable_patch_first_trim_curve_loops)
		.def("writable_trim_points", &detail::nurbs_patches::writable_trim_points)
		.def("writable_trim_point_selection", &detail::nurbs_patches::writable_trim_point_selection)
		.def("writable_first_trim_curves", &detail::nurbs_patches::writable_first_trim_curves)
		.def("writable_trim_curve_counts", &detail::nurbs_patches::writable_trim_curve_counts)
		.def("writable_trim_curve_loop_selection", &detail::nurbs_patches::writable_trim_curve_loop_selection)
		.def("writable_trim_curve_first_points", &detail::nurbs_patches::writable_trim_curve_first_points)
		.def("writable_trim_curve_point_counts", &detail::nurbs_patches::writable_trim_curve_point_counts)
		.def("writable_trim_curve_orders", &detail::nurbs_patches::writable_trim_curve_orders)
		.def("writable_trim_curve_first_knots", &detail::nurbs_patches::writable_trim_curve_first_knots)
		.def("writable_trim_curve_selection", &detail::nurbs_patches::writable_trim_curve_selection)
		.def("writable_trim_curve_points", &detail::nurbs_patches::writable_trim_curve_points)
		.def("writable_trim_curve_point_weights", &detail::nurbs_patches::writable_trim_curve_point_weights)
		.def("writable_trim_curve_knots", &detail::nurbs_patches::writable_trim_curve_knots)

		.def("create_patch_first_points", &detail::nurbs_patches::create_patch_first_points)
		.def("create_patch_u_point_counts", &detail::nurbs_patches::create_patch_u_point_counts)
		.def("create_patch_v_point_counts", &detail::nurbs_patches::create_patch_v_point_counts)
		.def("create_patch_u_orders", &detail::nurbs_patches::create_patch_u_orders)
		.def("create_patch_v_orders", &detail::nurbs_patches::create_patch_v_orders)
		.def("create_patch_u_first_knots", &detail::nurbs_patches::create_patch_u_first_knots)
		.def("create_patch_v_first_knots", &detail::nurbs_patches::create_patch_v_first_knots)
		.def("create_patch_selection", &detail::nurbs_patches::create_patch_selection)
		.def("create_patch_materials", &detail::nurbs_patches::create_patch_materials)
		.def("create_patch_points", &detail::nurbs_patches::create_patch_points)
		.def("create_patch_point_weights", &detail::nurbs_patches::create_patch_point_weights)
		.def("create_patch_u_knots", &detail::nurbs_patches::create_patch_u_knots)
		.def("create_patch_v_knots", &detail::nurbs_patches::create_patch_v_knots)
		.def("create_patch_trim_curve_loop_counts", &detail::nurbs_patches::create_patch_trim_curve_loop_counts)
		.def("create_patch_first_trim_curve_loops", &detail::nurbs_patches::create_patch_first_trim_curve_loops)
		.def("create_trim_points", &detail::nurbs_patches::create_trim_points)
		.def("create_trim_point_selection", &detail::nurbs_patches::create_trim_point_selection)
		.def("create_first_trim_curves", &detail::nurbs_patches::create_first_trim_curves)
		.def("create_trim_curve_counts", &detail::nurbs_patches::create_trim_curve_counts)
		.def("create_trim_curve_loop_selection", &detail::nurbs_patches::create_trim_curve_loop_selection)
		.def("create_trim_curve_first_points", &detail::nurbs_patches::create_trim_curve_first_points)
		.def("create_trim_curve_point_counts", &detail::nurbs_patches::create_trim_curve_point_counts)
		.def("create_trim_curve_orders", &detail::nurbs_patches::create_trim_curve_orders)
		.def("create_trim_curve_first_knots", &detail::nurbs_patches::create_trim_curve_first_knots)
		.def("create_trim_curve_selection", &detail::nurbs_patches::create_trim_curve_selection)
		.def("create_trim_curve_points", &detail::nurbs_patches::create_trim_curve_points)
		.def("create_trim_curve_point_weights", &detail::nurbs_patches::create_trim_curve_point_weights)
		.def("create_trim_curve_knots", &detail::nurbs_patches::create_trim_curve_knots);

	class_<detail::const_polyhedra>("const_polyhedra",
		"Stores an immutable (read-only) collection of polyhedron primitives.")
		.def("first_faces", &detail::const_polyhedra::first_faces)
		.def("face_counts", &detail::const_polyhedra::face_counts)
		.def("types", &detail::const_polyhedra::types)
		.def("constant_data", &detail::const_polyhedra::constant_data)
		.def("face_first_loops", &detail::const_polyhedra::face_first_loops)
		.def("face_loop_counts", &detail::const_polyhedra::face_loop_counts)
		.def("face_selection", &detail::const_polyhedra::face_selection)
		.def("face_materials", &detail::const_polyhedra::face_materials)
		.def("uniform_data", &detail::const_polyhedra::uniform_data)
		.def("loop_first_edges", &detail::const_polyhedra::loop_first_edges)
		.def("edge_points", &detail::const_polyhedra::edge_points)
		.def("clockwise_edges", &detail::const_polyhedra::clockwise_edges)
		.def("edge_selection", &detail::const_polyhedra::edge_selection)
		.def("face_varying_data", &detail::const_polyhedra::face_varying_data);

	class_<detail::polyhedra>("polyhedra",
		"Stores a mutable (read-write) collection of polyhedron primitives.")
		.def("first_faces", &detail::polyhedra::first_faces)
		.def("face_counts", &detail::polyhedra::face_counts)
		.def("types", &detail::polyhedra::types)
		.def("constant_data", &detail::polyhedra::constant_data)
		.def("face_first_loops", &detail::polyhedra::face_first_loops)
		.def("face_loop_counts", &detail::polyhedra::face_loop_counts)
		.def("face_selection", &detail::polyhedra::face_selection)
		.def("face_materials", &detail::polyhedra::face_materials)
		.def("uniform_data", &detail::polyhedra::uniform_data)
		.def("loop_first_edges", &detail::polyhedra::loop_first_edges)
		.def("edge_points", &detail::polyhedra::edge_points)
		.def("clockwise_edges", &detail::polyhedra::clockwise_edges)
		.def("edge_selection", &detail::polyhedra::edge_selection)
		.def("face_varying_data", &detail::polyhedra::face_varying_data)

		.def("writable_first_faces", &detail::polyhedra::writable_first_faces)
		.def("writable_face_counts", &detail::polyhedra::writable_face_counts)
		.def("writable_types", &detail::polyhedra::writable_types)
		.def("writable_constant_data", &detail::polyhedra::writable_constant_data)
		.def("writable_face_first_loops", &detail::polyhedra::writable_face_first_loops)
		.def("writable_face_loop_counts", &detail::polyhedra::writable_face_loop_counts)
		.def("writable_face_selection", &detail::polyhedra::writable_face_selection)
		.def("writable_face_materials", &detail::polyhedra::writable_face_materials)
		.def("writable_uniform_data", &detail::polyhedra::writable_uniform_data)
		.def("writable_loop_first_edges", &detail::polyhedra::writable_loop_first_edges)
		.def("writable_edge_points", &detail::polyhedra::writable_edge_points)
		.def("writable_clockwise_edges", &detail::polyhedra::writable_clockwise_edges)
		.def("writable_edge_selection", &detail::polyhedra::writable_edge_selection)
		.def("writable_face_varying_data", &detail::polyhedra::writable_face_varying_data)

		.def("create_first_faces", &detail::polyhedra::create_first_faces)
		.def("create_face_counts", &detail::polyhedra::create_face_counts)
		.def("create_types", &detail::polyhedra::create_types)
		.def("create_face_first_loops", &detail::polyhedra::create_face_first_loops)
		.def("create_face_loop_counts", &detail::polyhedra::create_face_loop_counts)
		.def("create_face_selection", &detail::polyhedra::create_face_selection)
		.def("create_face_materials", &detail::polyhedra::create_face_materials)
		.def("create_loop_first_edges", &detail::polyhedra::create_loop_first_edges)
		.def("create_edge_points", &detail::polyhedra::create_edge_points)
		.def("create_clockwise_edges", &detail::polyhedra::create_clockwise_edges)
		.def("create_edge_selection", &detail::polyhedra::create_edge_selection);

	enum_<k3d::mesh::polyhedra_t::polyhedron_type>("polyhedron_type")
		.value("polygons", k3d::mesh::polyhedra_t::POLYGONS)
		.value("catmull_clark", k3d::mesh::polyhedra_t::CATMULL_CLARK)
		.attr("__module__") = "k3d";

	def("validate", detail::validate,
		"Returns true if the given L{mesh} contains valid data.");
	def("is_solid", detail::is_solid,
		"Returns true if every L{polyhedra} in the given L{mesh} is solid.");
	def("is_triangles", detail::is_triangles,
		"Returns true if every face in the given L{mesh} is a triangle.");
	def("is_uninitialized", detail::is_uninitialized,
		"Returns true if all arrays and primitives are uninitialized (null).");

	scope outer = class_<mesh>("mesh", 
		"Stores a heterogeneous collection of geometric mesh primitives.", no_init)
		.def("bicubic_patches", &mesh::bicubic_patches,
			"Returns a L{const_bicubic_patches} object containing an immutable (read-only) collection of bicubic patch primitives, or None.")
		.def("bilinear_patches", &mesh::bilinear_patches,
			"Returna a L{const_bilinear_patches} object containing an immutable (read-only) collection of bilinear patch primitives, or None.")
		.def("nurbs_curve_groups", &mesh::nurbs_curve_groups,
			"Returns a L{const_nurbs_curve_groups} object containing an immutable (read-only) collection of NURBS curve primitives, or None.")
		.def("nurbs_patches", &mesh::nurbs_patches,
			"Returns a L{const_nurbs_patches} object containing an immutable (read-only) collection of NURBS patch primitives, or None.")
		.def("vertex_data", &mesh::vertex_data,
			"Returns a L{const_named_arrays} object containing a collection of immutable (read-only) per-vertex data, or None.")
		.def("point_selection", &mesh::point_selection,
			"Returns an immutable (read-only) L{const_double_array} object containing the selection state of every vertex in the mesh, or None.")
		.def("points", &mesh::points,
			"Returns an immutable (read-only) L{const_point3_array} object containing the geometric coordinates of every vertex in the mesh, or None.")
		.def("polyhedra", &mesh::polyhedra,
			"Returns a L{const_polyhedra} object containing a collection of immutable (read-only) polyhedron primitives, or None.")
		.def("writable_bicubic_patches", &mesh::writable_bicubic_patches,
			"Returns a L{bicubic_patches} object containing a mutable (read-write) collection of bicubic patch primitives, or None.")
		.def("writable_bilinear_patches", &mesh::writable_bilinear_patches,
			"Returns a L{bilinear_patches} object containing a mutable (read-write) collection of bilinear patch primitives, or None.")
		.def("writable_nurbs_curve_groups", &mesh::writable_nurbs_curve_groups,
			"Returns a L{nurbs_curve_groups} object containing a mutable (read-write) collection of NURBS curve primitives, or None.")
		.def("writable_nurbs_patches", &mesh::writable_nurbs_patches,
			"Returns a L{nurbs_patches} object containing a mutable (read-write) collection of NURBS patch primitives, or None.")
		.def("writable_vertex_data", &mesh::writable_vertex_data,
			"Returns a L{named_arrays} object containing a collection of mutable (read-write) per-vertex data, or None.")
		.def("writable_point_selection", &mesh::writable_point_selection,
			"Returns a mutable (read-write) L{double_array} object containing the selection state of every vertex in the mesh, or None.")
		.def("writable_points", &mesh::writable_points,
			"Returns a mutable (read-write) L{point3_array} object containing the geometric coordinates of every vertex in the mesh, or None.")
		.def("writable_polyhedra", &mesh::writable_polyhedra,
			"Returns a L{polyhedra} object containing a mutable (read-write) collection of polyhedron primitives, or None.")
		.def("copy", &mesh::copy,
			"Store a shallow copy of the given L{mesh}.")
		.def("create_bicubic_patches", &mesh::create_bicubic_patches,
			"Creates and returns a new L{bicubic_patches} object for storing bicubic patch primitives.")
		.def("create_bilinear_patches", &mesh::create_bilinear_patches,
			"Creates and returns a new L{bilinear_patches} object for storing bilinear patch primitives.")
		.def("create_nurbs_curve_groups", &mesh::create_nurbs_curve_groups,
			"Creates and returns a new L{nurbs_curve_groups} object for storing NURBS curve primitives.")
		.def("create_nurbs_patches", &mesh::create_nurbs_patches,
			"Creates and returns a new L{nurbs_patches} object for storing NURBS patch primitives.")
		.def("create_point_selection", &mesh::create_point_selection,
			"Creates and returns a new L{double_array} object used to store the selection state of every vertex in the mesh.")
		.def("create_points", &mesh::create_points,
			"Creates and returns a new L{point3_array} object used to store the geometric coordinates of every vertex in the mesh.")
		.def("create_polyhedra", &mesh::create_polyhedra,
			"Create and returns a new L{polyhedra} object for storing polyhedron primitives.")
		.def("primitives", &mesh_primitives,
			"Returns the set of L{const_primitive} objects in the mesh.")
		.def("__repr__", &mesh::repr)
		.def("__str__", &mesh::str);

	class_<const_mesh_primitive_wrapper>("const_primitive", no_init)
		.def("type", &const_mesh_primitive_get_type)
		.def("topology", &const_mesh_primitive_get_topology)
		.def("attributes", &const_mesh_primitive_get_attributes)
		;

	class_<mesh_primitive_wrapper>("primitive", no_init)
		.def("type", &mesh_primitive_get_type)
		.def("topology", &mesh_primitive_get_topology)
		.def("attributes", &mesh_primitive_get_attributes)
		;

	class_<const_mesh_primitives_t_wrapper>("const_primitives_t", no_init)
		.def("__len__", &utility::wrapped_len<const_mesh_primitives_t_wrapper>)
		.def("__getitem__", &const_mesh_primitives_t_get_item)
		;

	class_<mesh_primitives_t_wrapper>("primitives_t", no_init)
		.def("__len__", &utility::wrapped_len<mesh_primitives_t_wrapper>)
		.def("__getitem__", &mesh_primitives_t_get_item)
		.def("create", &mesh_primitives_t_create)
		;
}

} // namespace python

} // namespace k3d

