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

/** \file
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "array.h"
#include "imaterial.h"
#include "ipersistent.h"
#include "ipersistent_lookup.h"
#include "legacy_mesh.h"
#include "mesh_selection.h"
#include "mesh.h"
#include "shared_pointer.h"
#include "types.h"
#include "types_ri.h"
#include "xml.h"

using namespace k3d::xml;

namespace k3d
{

namespace detail
{

/////////////////////////////////////////////////////////////////////////////
// save_array

template<typename array_type>
void save_array(element& Container, element Storage, const array_type& Array, const ipersistent::save_context& Context)
{
	if(Array.empty())
		return;

	typename array_type::const_iterator item = Array.begin();
	const typename array_type::const_iterator end = Array.end();

	std::ostringstream buffer;

	buffer << *item++;
	for(; item != end; ++item)
		buffer << " " << *item;

	Storage.text = buffer.str();
	Container.append(Storage);
}

/////////////////////////////////////////////////////////////////////////////
// save_array

void save_array(element& Container, element Storage, const typed_array<imaterial*>& Array, const ipersistent::save_context& Context)
{
	typedef typed_array<imaterial*> array_type;

	if(Array.empty())
		return;

	array_type::const_iterator item = Array.begin();
	const array_type::const_iterator end = Array.end();

	std::ostringstream buffer;

	buffer << Context.lookup.lookup_id(*item++);
	for(; item != end; ++item)
		buffer << " " << Context.lookup.lookup_id(*item);

	Storage.text = buffer.str();
	Container.append(Storage);
}

/////////////////////////////////////////////////////////////////////////////
// save_array

template<typename array_type>
void save_array(element& Container, element Storage, const boost::shared_ptr<const array_type>& Array, const ipersistent::save_context& Context)
{
	if(!Array)
		return;

	save_array(Container, Storage, *Array, Context);
}

/////////////////////////////////////////////////////////////////////////////
// save_typed_array

template<typename value_type>
bool save_typed_array(element& Container, const std::string& Name, array& Array, const ipersistent::save_context& Context)
{
	if(typed_array<value_type>* const array = dynamic_cast<typed_array<value_type>*>(&Array))
	{
		save_array(Container, element("array", attribute("name", Name), attribute("type", type_string<value_type>())), *array, Context);
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// save_arrays

void save_arrays(element& Container, element Storage, const dev::mesh::named_arrays& Arrays, const ipersistent::save_context& Context)
{
	if(!Arrays.size())
		return;

	element& container = Container.append(Storage);
	for(dev::mesh::named_arrays::const_iterator array_iterator = Arrays.begin(); array_iterator != Arrays.end(); ++array_iterator)
	{
		const std::string name = array_iterator->first;
		array* const abstract_array = array_iterator->second.get();

		if(name.empty())
		{
			k3d::log() << error << "will not serialize unnamed array" << std::endl;
			continue;
		}

		if(!abstract_array)
		{
			k3d::log() << error << "will not serialize null array [" << name << "]" << std::endl;
			continue;
		}

		if(save_typed_array<ri::integer>(container, name, *abstract_array, Context)) continue;
		if(save_typed_array<ri::real>(container, name, *abstract_array, Context)) continue;
		if(save_typed_array<ri::string>(container, name, *abstract_array, Context)) continue;
		if(save_typed_array<ri::point>(container, name, *abstract_array, Context)) continue;
		if(save_typed_array<ri::vector>(container, name, *abstract_array, Context)) continue;
		if(save_typed_array<ri::normal>(container, name, *abstract_array, Context)) continue;
		if(save_typed_array<ri::color>(container, name, *abstract_array, Context)) continue;
		if(save_typed_array<ri::hpoint>(container, name, *abstract_array, Context)) continue;
		if(save_typed_array<ri::matrix>(container, name, *abstract_array, Context)) continue;
		if(save_typed_array<bool>(container, name, *abstract_array, Context)) continue;

		k3d::log() << error << "array [" << name << "] unknown type [" << typeid(*abstract_array).name() << "]" << std::endl;
	}
}

/////////////////////////////////////////////////////////////////////////////
// load_array

template<typename array_type>
void load_array(const element& Storage, array_type& Array, const ipersistent::load_context& Context)
{
	typename array_type::value_type value;

	std::istringstream buffer(Storage.text);
	while(true)
	{
		buffer >> value;

		if(!buffer)
			break;

		Array.push_back(value);
	}
}

/////////////////////////////////////////////////////////////////////////////
// load_array

void load_array(const element& Storage, typed_array<imaterial*>& Array, const ipersistent::load_context& Context)
{
	std::istringstream buffer(Storage.text);
	while(true)
	{
		ipersistent_lookup::id_type id = 0;
		buffer >> id;

		if(!buffer)
			break;

		Array.push_back(dynamic_cast<imaterial*>(Context.lookup.lookup_object(id)));
	}
}

/////////////////////////////////////////////////////////////////////////////
// load_array

template<typename array_type>
void load_array(const element& Container, const std::string& Storage, boost::shared_ptr<const array_type>& Array, const ipersistent::load_context& Context)
{
	const element* const storage = find_element(Container, Storage);
	if(!storage)
		return;

	array_type* const array = Array ? make_unique(Array) : new array_type();
	if(!Array)
		Array.reset(array);

	load_array(*storage, *array, Context);
}

/////////////////////////////////////////////////////////////////////////////
// load_typed_array

template<typename value_type>
bool load_typed_array(const element& Storage, const std::string& Name, const std::string& Type, dev::mesh::named_arrays& Arrays, const ipersistent::load_context& Context)
{
	if(type_string<value_type>() == Type)
	{
		typed_array<value_type>* const array = new typed_array<value_type>();
		load_array(Storage, *array, Context);
		Arrays.insert(std::make_pair(Name, array));
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// load_arrays

void load_arrays(const element& Container, const std::string& Storage, dev::mesh::named_arrays& Arrays, const ipersistent::load_context& Context)
{
	const element* const container = find_element(Container, Storage);
	if(!container)
		return;

	for(size_t i = 0; i != container->children.size(); ++i)
	{
		const element& storage = container->children[i];

		if(storage.name != "array")
			continue;

		const std::string name = attribute_text(storage, "name");
		if(name.empty())
		{
			k3d::log() << error << "unnamed array will not be loaded" << std::endl;
			continue;
		}

		if(Arrays.count(name))
		{
			k3d::log() << error << "duplicate array [" << name << "] will not be loaded" << std::endl;
			continue;
		}

		const std::string type = attribute_text(storage, "type");
		if(type.empty())
		{
			k3d::log() << error << "untyped array [" << name << "] will not be loaded" << std::endl;
			continue;
		}

		if(load_typed_array<ri::integer>(storage, name, type, Arrays, Context)) continue;
		if(load_typed_array<ri::real>(storage, name, type, Arrays, Context)) continue;
		if(load_typed_array<ri::string>(storage, name, type, Arrays, Context)) continue;
		if(load_typed_array<ri::point>(storage, name, type, Arrays, Context)) continue;
		if(load_typed_array<ri::vector>(storage, name, type, Arrays, Context)) continue;
		if(load_typed_array<ri::normal>(storage, name, type, Arrays, Context)) continue;
		if(load_typed_array<ri::color>(storage, name, type, Arrays, Context)) continue;
		if(load_typed_array<ri::hpoint>(storage, name, type, Arrays, Context)) continue;
		if(load_typed_array<ri::matrix>(storage, name, type, Arrays, Context)) continue;
		if(load_typed_array<bool>(storage, name, type, Arrays, Context)) continue;

		k3d::log() << error << "array [" << name << "] of unknown type [" << type << "] will not be loaded" << std::endl;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// save_selection

void save_selection(element& Element, const mesh_selection::records_t& Records, const std::string& ElementName)
{
	if(Records.empty())
		return;

	element& xml_records = Element.append(element(ElementName));

	for(mesh_selection::records_t::const_iterator record = Records.begin(); record != Records.end(); ++record)
	{
		xml_records.append(
			element("selection",
				attribute("begin", record->begin),
				attribute("end", record->end),
				attribute("weight", record->weight)));
	}
}

void load_selection(const element& Element, mesh_selection::records_t& Records)
{
	for(element::elements_t::const_iterator xml_selection = Element.children.begin(); xml_selection != Element.children.end(); ++xml_selection)
	{
		if(xml_selection->name != "selection")
			continue;

		const size_t begin = attribute_value<size_t>(*xml_selection, "begin", 0);
		const size_t end = attribute_value<size_t>(*xml_selection, "end", 0);
		const double weight = attribute_value<double>(*xml_selection, "weight", 0.0);

		Records.push_back(mesh_selection::record(begin, end, weight));
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// save_mesh

void save_mesh(const dev::mesh& Mesh, element& Container, const ipersistent::save_context& Context)
{
	// Save points ...
	detail::save_array(Container, element("points"), Mesh.points, Context);
	detail::save_array(Container, element("point_selection"), Mesh.point_selection, Context);
	detail::save_arrays(Container, element("vertex_data"), Mesh.vertex_data, Context);

	if(Mesh.point_groups)
	{
		element& container = Container.append(element("point_groups"));
		detail::save_array(container, element("first_points"), Mesh.point_groups->first_points, Context);
		detail::save_array(container, element("point_counts"), Mesh.point_groups->point_counts, Context);
		detail::save_array(container, element("materials"), Mesh.point_groups->materials, Context);
		detail::save_arrays(container, element("constant_data"), Mesh.point_groups->constant_data, Context);
		detail::save_array(container, element("points"), Mesh.point_groups->points, Context);
		detail::save_arrays(container, element("varying_data"), Mesh.point_groups->varying_data, Context);
	}

	if(Mesh.linear_curve_groups)
	{
		element& container = Container.append(element("linear_curve_groups"));
		detail::save_array(container, element("first_curves"), Mesh.linear_curve_groups->first_curves, Context);
		detail::save_array(container, element("curve_counts"), Mesh.linear_curve_groups->curve_counts, Context);
		detail::save_array(container, element("periodic_curves"), Mesh.linear_curve_groups->periodic_curves, Context);
		detail::save_array(container, element("materials"), Mesh.linear_curve_groups->materials, Context);
		detail::save_arrays(container, element("constant_data"), Mesh.linear_curve_groups->constant_data, Context);
		detail::save_array(container, element("curve_first_points"), Mesh.linear_curve_groups->curve_first_points, Context);
		detail::save_array(container, element("curve_point_counts"), Mesh.linear_curve_groups->curve_point_counts, Context);
		detail::save_array(container, element("curve_selection"), Mesh.linear_curve_groups->curve_selection, Context);
		detail::save_arrays(container, element("uniform_data"), Mesh.linear_curve_groups->uniform_data, Context);
		detail::save_array(container, element("curve_points"), Mesh.linear_curve_groups->curve_points, Context);
	}

	if(Mesh.cubic_curve_groups)
	{
		element& container = Container.append(element("cubic_curve_groups"));
		detail::save_array(container, element("first_curves"), Mesh.cubic_curve_groups->first_curves, Context);
		detail::save_array(container, element("curve_counts"), Mesh.cubic_curve_groups->curve_counts, Context);
		detail::save_array(container, element("periodic_curves"), Mesh.cubic_curve_groups->periodic_curves, Context);
		detail::save_array(container, element("materials"), Mesh.cubic_curve_groups->materials, Context);
		detail::save_arrays(container, element("constant_data"), Mesh.cubic_curve_groups->constant_data, Context);
		detail::save_array(container, element("curve_first_points"), Mesh.cubic_curve_groups->curve_first_points, Context);
		detail::save_array(container, element("curve_point_counts"), Mesh.cubic_curve_groups->curve_point_counts, Context);
		detail::save_array(container, element("curve_selection"), Mesh.cubic_curve_groups->curve_selection, Context);
		detail::save_arrays(container, element("uniform_data"), Mesh.cubic_curve_groups->uniform_data, Context);
		detail::save_array(container, element("curve_points"), Mesh.cubic_curve_groups->curve_points, Context);
	}

	if(Mesh.nurbs_curve_groups)
	{
		element& container = Container.append(element("nurbs_curve_groups"));
		detail::save_array(container, element("first_curves"), Mesh.nurbs_curve_groups->first_curves, Context);
		detail::save_array(container, element("curve_counts"), Mesh.nurbs_curve_groups->curve_counts, Context);
		detail::save_array(container, element("materials"), Mesh.nurbs_curve_groups->materials, Context);
		detail::save_arrays(container, element("constant_data"), Mesh.nurbs_curve_groups->constant_data, Context);
		detail::save_array(container, element("curve_first_points"), Mesh.nurbs_curve_groups->curve_first_points, Context);
		detail::save_array(container, element("curve_point_counts"), Mesh.nurbs_curve_groups->curve_point_counts, Context);
		detail::save_array(container, element("curve_orders"), Mesh.nurbs_curve_groups->curve_orders, Context);
		detail::save_array(container, element("curve_first_knots"), Mesh.nurbs_curve_groups->curve_first_knots, Context);
		detail::save_array(container, element("curve_selection"), Mesh.nurbs_curve_groups->curve_selection, Context);
		detail::save_arrays(container, element("uniform_data"), Mesh.nurbs_curve_groups->uniform_data, Context);
		detail::save_array(container, element("curve_points"), Mesh.nurbs_curve_groups->curve_points, Context);
		detail::save_array(container, element("curve_point_weights"), Mesh.nurbs_curve_groups->curve_point_weights, Context);
		detail::save_array(container, element("curve_knots"), Mesh.nurbs_curve_groups->curve_knots, Context);
	}

	if(Mesh.bilinear_patches)
	{
		element& container = Container.append(element("bilinear_patches"));
		detail::save_array(container, element("patch_selection"), Mesh.bilinear_patches->patch_selection, Context);
		detail::save_array(container, element("patch_materials"), Mesh.bilinear_patches->patch_materials, Context);
		detail::save_arrays(container, element("constant_data"), Mesh.bilinear_patches->constant_data, Context);
		detail::save_arrays(container, element("uniform_data"), Mesh.bilinear_patches->uniform_data, Context);
		detail::save_array(container, element("patch_points"), Mesh.bilinear_patches->patch_points, Context);
		detail::save_arrays(container, element("varying_data"), Mesh.bilinear_patches->varying_data, Context);
	}

	if(Mesh.bicubic_patches)
	{
		element& container = Container.append(element("bicubic_patches"));
		detail::save_array(container, element("patch_selection"), Mesh.bicubic_patches->patch_selection, Context);
		detail::save_array(container, element("patch_materials"), Mesh.bicubic_patches->patch_materials, Context);
		detail::save_arrays(container, element("constant_data"), Mesh.bicubic_patches->constant_data, Context);
		detail::save_arrays(container, element("uniform_data"), Mesh.bicubic_patches->uniform_data, Context);
		detail::save_array(container, element("patch_points"), Mesh.bicubic_patches->patch_points, Context);
		detail::save_arrays(container, element("varying_data"), Mesh.bicubic_patches->varying_data, Context);
	}

	if(Mesh.nurbs_patches)
	{
		element& container = Container.append(element("nurbs_patches"));
		detail::save_array(container, element("patch_first_points"), Mesh.nurbs_patches->patch_first_points, Context);
		detail::save_array(container, element("patch_u_point_counts"), Mesh.nurbs_patches->patch_u_point_counts, Context);
		detail::save_array(container, element("patch_v_point_counts"), Mesh.nurbs_patches->patch_v_point_counts, Context);
		detail::save_array(container, element("patch_u_orders"), Mesh.nurbs_patches->patch_u_orders, Context);
		detail::save_array(container, element("patch_v_orders"), Mesh.nurbs_patches->patch_v_orders, Context);
		detail::save_array(container, element("patch_u_first_knots"), Mesh.nurbs_patches->patch_u_first_knots, Context);
		detail::save_array(container, element("patch_v_first_knots"), Mesh.nurbs_patches->patch_v_first_knots, Context);
		detail::save_array(container, element("patch_selection"), Mesh.nurbs_patches->patch_selection, Context);
		detail::save_array(container, element("patch_materials"), Mesh.nurbs_patches->patch_materials, Context);
		detail::save_arrays(container, element("constant_data"), Mesh.nurbs_patches->constant_data, Context);
		detail::save_arrays(container, element("uniform_data"), Mesh.nurbs_patches->uniform_data, Context);
		detail::save_array(container, element("patch_points"), Mesh.nurbs_patches->patch_points, Context);
		detail::save_array(container, element("patch_point_weights"), Mesh.nurbs_patches->patch_point_weights, Context);
		detail::save_array(container, element("patch_u_knots"), Mesh.nurbs_patches->patch_u_knots, Context);
		detail::save_array(container, element("patch_v_knots"), Mesh.nurbs_patches->patch_v_knots, Context);
		detail::save_arrays(container, element("varying_data"), Mesh.nurbs_patches->varying_data, Context);
	}

	if(Mesh.polyhedra)
	{
		element& container = Container.append(element("polyhedra"));
		detail::save_array(container, element("first_faces"), Mesh.polyhedra->first_faces, Context);
		detail::save_array(container, element("face_counts"), Mesh.polyhedra->face_counts, Context);
		detail::save_array(container, element("types"), Mesh.polyhedra->types, Context);
		detail::save_arrays(container, element("constant_data"), Mesh.polyhedra->constant_data, Context);
		detail::save_array(container, element("face_first_loops"), Mesh.polyhedra->face_first_loops, Context);
		detail::save_array(container, element("face_loop_counts"), Mesh.polyhedra->face_loop_counts, Context);
		detail::save_array(container, element("face_selection"), Mesh.polyhedra->face_selection, Context);
		detail::save_array(container, element("face_materials"), Mesh.polyhedra->face_materials, Context);
		detail::save_arrays(container, element("uniform_data"), Mesh.polyhedra->uniform_data, Context);
		detail::save_array(container, element("loop_first_edges"), Mesh.polyhedra->loop_first_edges, Context);
		detail::save_array(container, element("edge_points"), Mesh.polyhedra->edge_points, Context);
		detail::save_array(container, element("clockwise_edges"), Mesh.polyhedra->clockwise_edges, Context);
		detail::save_array(container, element("edge_selection"), Mesh.polyhedra->edge_selection, Context);
		detail::save_arrays(container, element("face_varying_data"), Mesh.polyhedra->face_varying_data, Context);
	}
}

/////////////////////////////////////////////////////////////////////////////
// load_mesh

void load_mesh(dev::mesh& Mesh, element& Container, const ipersistent::load_context& Context)
{
	detail::load_array(Container, "points", Mesh.points, Context);
	detail::load_array(Container, "point_selection", Mesh.point_selection, Context);
	detail::load_arrays(Container, "vertex_data", Mesh.vertex_data, Context);

	if(element* const container = find_element(Container, "point_groups"))
	{
		dev::mesh::point_groups_t* const point_groups = make_unique(Mesh.point_groups);
		detail::load_array(*container, "first_points", point_groups->first_points, Context);
		detail::load_array(*container, "point_counts", point_groups->point_counts, Context);
		detail::load_array(*container, "materials", point_groups->materials, Context);
		detail::load_arrays(*container, "constant_data", point_groups->constant_data, Context);
		detail::load_array(*container, "points", point_groups->points, Context);
		detail::load_arrays(*container, "varying_data", point_groups->varying_data, Context);
	}

	if(element* const container = find_element(Container, "linear_curve_groups"))
	{
		dev::mesh::linear_curve_groups_t* const linear_curve_groups = make_unique(Mesh.linear_curve_groups);
		detail::load_array(*container, "first_curves", linear_curve_groups->first_curves, Context);
		detail::load_array(*container, "curve_counts", linear_curve_groups->curve_counts, Context);
		detail::load_array(*container, "periodic_curves", linear_curve_groups->periodic_curves, Context);
		detail::load_array(*container, "materials", linear_curve_groups->materials, Context);
		detail::load_arrays(*container, "constant_data", linear_curve_groups->constant_data, Context);
		detail::load_array(*container, "curve_first_points", linear_curve_groups->curve_first_points, Context);
		detail::load_array(*container, "curve_point_counts", linear_curve_groups->curve_point_counts, Context);
		detail::load_array(*container, "curve_selection", linear_curve_groups->curve_selection, Context);
		detail::load_arrays(*container, "uniform_data", linear_curve_groups->uniform_data, Context);
		detail::load_array(*container, "curve_points", linear_curve_groups->curve_points, Context);
	}

	if(element* const container = find_element(Container, "cubic_curve_groups"))
	{
		dev::mesh::cubic_curve_groups_t* const cubic_curve_groups = make_unique(Mesh.cubic_curve_groups);
		detail::load_array(*container, "first_curves", cubic_curve_groups->first_curves, Context);
		detail::load_array(*container, "curve_counts", cubic_curve_groups->curve_counts, Context);
		detail::load_array(*container, "periodic_curves", cubic_curve_groups->periodic_curves, Context);
		detail::load_array(*container, "materials", cubic_curve_groups->materials, Context);
		detail::load_arrays(*container, "constant_data", cubic_curve_groups->constant_data, Context);
		detail::load_array(*container, "curve_first_points", cubic_curve_groups->curve_first_points, Context);
		detail::load_array(*container, "curve_point_counts", cubic_curve_groups->curve_point_counts, Context);
		detail::load_array(*container, "curve_selection", cubic_curve_groups->curve_selection, Context);
		detail::load_arrays(*container, "uniform_data", cubic_curve_groups->uniform_data, Context);
		detail::load_array(*container, "curve_points", cubic_curve_groups->curve_points, Context);
	}

	if(element* const container = find_element(Container, "nurbs_curve_groups"))
	{
		dev::mesh::nurbs_curve_groups_t* const nurbs_curve_groups = make_unique(Mesh.nurbs_curve_groups);
		detail::load_array(*container, "first_curves", nurbs_curve_groups->first_curves, Context);
		detail::load_array(*container, "curve_counts", nurbs_curve_groups->curve_counts, Context);
		detail::load_array(*container, "materials", nurbs_curve_groups->materials, Context);
		detail::load_arrays(*container, "constant_data", nurbs_curve_groups->constant_data, Context);
		detail::load_array(*container, "curve_first_points", nurbs_curve_groups->curve_first_points, Context);
		detail::load_array(*container, "curve_point_counts", nurbs_curve_groups->curve_point_counts, Context);
		detail::load_array(*container, "curve_orders", nurbs_curve_groups->curve_orders, Context);
		detail::load_array(*container, "curve_first_knots", nurbs_curve_groups->curve_first_knots, Context);
		detail::load_array(*container, "curve_selection", nurbs_curve_groups->curve_selection, Context);
		detail::load_arrays(*container, "uniform_data", nurbs_curve_groups->uniform_data, Context);
		detail::load_array(*container, "curve_points", nurbs_curve_groups->curve_points, Context);
		detail::load_array(*container, "curve_point_weights", nurbs_curve_groups->curve_point_weights, Context);
		detail::load_array(*container, "curve_knots", nurbs_curve_groups->curve_knots, Context);
	}

	if(element* const container = find_element(Container, "bilinear_patches"))
	{
		dev::mesh::bilinear_patches_t* const bilinear_patches = make_unique(Mesh.bilinear_patches);
		detail::load_array(*container, "patch_selection", bilinear_patches->patch_selection, Context);
		detail::load_array(*container, "patch_materials", bilinear_patches->patch_materials, Context);
		detail::load_arrays(*container, "constant_data", bilinear_patches->constant_data, Context);
		detail::load_arrays(*container, "uniform_data", bilinear_patches->uniform_data, Context);
		detail::load_array(*container, "patch_points", bilinear_patches->patch_points, Context);
		detail::load_arrays(*container, "varying_data", bilinear_patches->varying_data, Context);
	}

	if(element* const container = find_element(Container, "bicubic_patches"))
	{
		dev::mesh::bicubic_patches_t* const bicubic_patches = make_unique(Mesh.bicubic_patches);
		detail::load_array(*container, "patch_selection", bicubic_patches->patch_selection, Context);
		detail::load_array(*container, "patch_materials", bicubic_patches->patch_materials, Context);
		detail::load_arrays(*container, "constant_data", bicubic_patches->constant_data, Context);
		detail::load_arrays(*container, "uniform_data", bicubic_patches->uniform_data, Context);
		detail::load_array(*container, "patch_points", bicubic_patches->patch_points, Context);
		detail::load_arrays(*container, "varying_data", bicubic_patches->varying_data, Context);
	}

	if(element* const container = find_element(Container, "nurbs_patches"))
	{
		dev::mesh::nurbs_patches_t* const nurbs_patches = make_unique(Mesh.nurbs_patches);
		detail::load_array(*container, "patch_first_points", nurbs_patches->patch_first_points, Context);
		detail::load_array(*container, "patch_u_point_counts", nurbs_patches->patch_u_point_counts, Context);
		detail::load_array(*container, "patch_v_point_counts", nurbs_patches->patch_v_point_counts, Context);
		detail::load_array(*container, "patch_u_orders", nurbs_patches->patch_u_orders, Context);
		detail::load_array(*container, "patch_v_orders", nurbs_patches->patch_v_orders, Context);
		detail::load_array(*container, "patch_u_first_knots", nurbs_patches->patch_u_first_knots, Context);
		detail::load_array(*container, "patch_v_first_knots", nurbs_patches->patch_v_first_knots, Context);
		detail::load_array(*container, "patch_selection", nurbs_patches->patch_selection, Context);
		detail::load_array(*container, "patch_materials", nurbs_patches->patch_materials, Context);
		detail::load_arrays(*container, "constant_data", nurbs_patches->constant_data, Context);
		detail::load_arrays(*container, "uniform_data", nurbs_patches->uniform_data, Context);
		detail::load_array(*container, "patch_points", nurbs_patches->patch_points, Context);
		detail::load_array(*container, "patch_point_weights", nurbs_patches->patch_point_weights, Context);
		detail::load_array(*container, "patch_u_knots", nurbs_patches->patch_u_knots, Context);
		detail::load_array(*container, "patch_v_knots", nurbs_patches->patch_v_knots, Context);
		detail::load_arrays(*container, "varying_data", nurbs_patches->varying_data, Context);
	}

	if(element* const container = find_element(Container, "polyhedra"))
	{
		dev::mesh::polyhedra_t* const polyhedra = make_unique(Mesh.polyhedra);
		detail::load_array(*container, "first_faces", polyhedra->first_faces, Context);
		detail::load_array(*container, "face_counts", polyhedra->face_counts, Context);
		detail::load_array(*container, "types", polyhedra->types, Context);
		detail::load_arrays(*container, "constant_data", polyhedra->constant_data, Context);
		detail::load_array(*container, "face_first_loops", polyhedra->face_first_loops, Context);
		detail::load_array(*container, "face_loop_counts", polyhedra->face_loop_counts, Context);
		detail::load_array(*container, "face_selection", polyhedra->face_selection, Context);
		detail::load_array(*container, "face_materials", polyhedra->face_materials, Context);
		detail::load_arrays(*container, "uniform_data", polyhedra->uniform_data, Context);
		detail::load_array(*container, "loop_first_edges", polyhedra->loop_first_edges, Context);
		detail::load_array(*container, "edge_points", polyhedra->edge_points, Context);
		detail::load_array(*container, "clockwise_edges", polyhedra->clockwise_edges, Context);
		detail::load_array(*container, "edge_selection", polyhedra->edge_selection, Context);
		detail::load_arrays(*container, "face_varying_data", polyhedra->face_varying_data, Context);
	}
}

void save_mesh_selection(const mesh_selection& Selection, xml::element& XML, const ipersistent::save_context& Context)
{
	detail::save_selection(XML, Selection.points, "points");
	detail::save_selection(XML, Selection.edges, "edges");
	detail::save_selection(XML, Selection.faces, "faces");
	detail::save_selection(XML, Selection.linear_curves, "linearcurves");
	detail::save_selection(XML, Selection.cubic_curves, "cubiccurves");
	detail::save_selection(XML, Selection.nurbs_curves, "nurbscurves");
	detail::save_selection(XML, Selection.bilinear_patches, "bilinearpatches");
	detail::save_selection(XML, Selection.bicubic_patches, "bicubicpatches");
	detail::save_selection(XML, Selection.nurbs_patches, "nurbspatches");
}

void load_mesh_selection(mesh_selection& Selection, xml::element& XML, const ipersistent::load_context& Context)
{
	for(element::elements_t::const_iterator xml_selection = XML.children.begin(); xml_selection != XML.children.end(); ++xml_selection)
	{
		if(xml_selection->name == "points")
			detail::load_selection(*xml_selection, Selection.points);
		if(xml_selection->name == "edges")
			detail::load_selection(*xml_selection, Selection.edges);
		if(xml_selection->name == "faces")
			detail::load_selection(*xml_selection, Selection.faces);
		if(xml_selection->name == "linearcurves")
			detail::load_selection(*xml_selection, Selection.linear_curves);
		if(xml_selection->name == "cubiccurves")
			detail::load_selection(*xml_selection, Selection.cubic_curves);
		if(xml_selection->name == "nurbscurves" || xml_selection->name == "nucurves")
			detail::load_selection(*xml_selection, Selection.nurbs_curves);
		if(xml_selection->name == "bilinearpatches")
			detail::load_selection(*xml_selection, Selection.bilinear_patches);
		if(xml_selection->name == "bicubicpatches")
			detail::load_selection(*xml_selection, Selection.bicubic_patches);
		if(xml_selection->name == "nurbspatches" || xml_selection->name == "nupatches")
			detail::load_selection(*xml_selection, Selection.nurbs_patches);
	}
}

} // namespace k3d

