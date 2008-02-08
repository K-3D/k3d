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

#include "any_python.h"
#include "iproperty_python.h"
#include "iproperty_collection_python.h"

#include <k3dsdk/idocument.h>
#include <k3dsdk/ipersistent_container.h>
#include <k3dsdk/irenderman_property.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/properties_ri.h>
#include <k3dsdk/user_properties.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

namespace detail
{

template<typename PropertyT, typename ValueT>
iproperty create_user_property(const std::string& Name, const std::string& Label, const std::string& Description, k3d::idocument& Document, k3d::iproperty_collection& PropertyCollection, k3d::ipersistent_container& PersistentContainer, k3d::inode* Object, const ValueT& Value)
{
	return iproperty(k3d::user::create_property<PropertyT, ValueT>(Name, Label, Description, Document, PropertyCollection, PersistentContainer, Object, Value));
}

template<typename PropertyT, typename ValueT>
iproperty create_renderman_property(const k3d::irenderman_property::parameter_type_t Type, const std::string& List, const std::string& Name, const std::string& Label, const std::string& Description, k3d::idocument& Document, k3d::iproperty_collection& PropertyCollection, k3d::ipersistent_container& PersistentContainer, k3d::inode* Object, const ValueT& Value)
{
	return iproperty(k3d::ri::create_property<PropertyT, ValueT>(Type, List, Name, Label, Description, Document, PropertyCollection, PersistentContainer, Object, Value));
}

} // namespace detail
	
iproperty_collection::iproperty_collection() :
	base()
{
}

iproperty_collection::iproperty_collection(k3d::iproperty_collection* PropertyCollection) :
	base(PropertyCollection)
{
}

list iproperty_collection::properties()
{
	list results;
	const k3d::iproperty_collection::properties_t& properties = wrapped().properties();
	for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
		results.append(iproperty(*property));
	return results;
}

iproperty iproperty_collection::get_property(const std::string& Name)
{
	return iproperty(k3d::property::get(wrapped(), Name));
}

iproperty iproperty_collection::add_user_property(const std::string& Type, const std::string& Name, const std::string& Label, const std::string& Description)
{
	k3d::inode* const node = dynamic_cast<k3d::inode*>(wrapped_ptr());
	if(!node)
		throw std::runtime_error("missing node");

	k3d::ipersistent_container* const persistent_container = dynamic_cast<k3d::ipersistent_container*>(wrapped_ptr());
	if(!persistent_container)
		throw std::runtime_error("missing persistent container");

	if(Type == k3d::type_string<k3d::bool_t>()) return detail::create_user_property<k3d::user::k3d_bool_t_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, false);
	else if(Type == k3d::type_string<k3d::double_t>()) return detail::create_user_property<k3d::user::k3d_double_t_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, 0.0);
	else if(Type == k3d::type_string<k3d::string_t>()) return detail::create_user_property<k3d::user::k3d_string_t_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, std::string());
	else if(Type == k3d::type_string<k3d::point3>()) return detail::create_user_property<k3d::user::k3d_point3_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::point3(0, 0, 0));
	else if(Type == k3d::type_string<k3d::int32_t>()) return detail::create_user_property<k3d::user::k3d_int32_t_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, 0);
	else if(Type == k3d::type_string<k3d::vector3>()) return detail::create_user_property<k3d::user::k3d_vector3_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::vector3(0, 0, 0));
	else if(Type == k3d::type_string<k3d::normal3>()) return detail::create_user_property<k3d::user::k3d_normal3_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::normal3(0, 0, 0));
	else if(Type == k3d::type_string<k3d::matrix4>()) return detail::create_user_property<k3d::user::k3d_matrix4_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::identity3D());
	else if(Type == k3d::type_string<k3d::color>()) return detail::create_user_property<k3d::user::k3d_color_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::color(1, 1, 1));
	else if(Type == k3d::type_string<k3d::mesh*>()) return detail::create_user_property<k3d::user::k3d_mesh_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, static_cast<k3d::mesh*>(0));
	else if(Type == k3d::type_string<k3d::gl::imesh_painter*>()) return detail::create_user_property<k3d::user::k3d_gl_imesh_painter_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, static_cast<k3d::gl::imesh_painter*>(0));
	else if(Type == k3d::type_string<k3d::ri::imesh_painter*>()) return detail::create_user_property<k3d::user::k3d_ri_imesh_painter_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, static_cast<k3d::ri::imesh_painter*>(0));
	else if(Type == k3d::type_string<k3d::imaterial*>()) return detail::create_user_property<k3d::user::k3d_imaterial_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, static_cast<k3d::imaterial*>(0));
	else if(Type == k3d::type_string<k3d::filesystem::path>()) return detail::create_user_property<k3d::user::k3d_filesystem_path_property>(Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::filesystem::path());

	throw std::invalid_argument("unknown user property type: " + Type);
}

iproperty iproperty_collection::add_ri_attribute(const std::string& Type, const std::string& List, const std::string& Name, const std::string& Label, const std::string& Description)
{
	k3d::inode* const node = dynamic_cast<k3d::inode*>(wrapped_ptr());
	if(!node)
		throw std::runtime_error("missing node");

	k3d::ipersistent_container* const persistent_container = dynamic_cast<k3d::ipersistent_container*>(wrapped_ptr());
	if(!persistent_container)
		throw std::runtime_error("missing persistent container");

	if(Type == "k3d::ri::integer") return detail::create_renderman_property<k3d::ri::integer_property>(k3d::irenderman_property::ATTRIBUTE, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, 0);
	else if(Type == "k3d::ri::real") return detail::create_renderman_property<k3d::ri::real_property>(k3d::irenderman_property::ATTRIBUTE, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, 0.0);
	else if(Type == "k3d::ri::string") return detail::create_renderman_property<k3d::ri::string_property>(k3d::irenderman_property::ATTRIBUTE, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::string(""));
	else if(Type == "k3d::ri::point") return detail::create_renderman_property<k3d::ri::point_property>(k3d::irenderman_property::ATTRIBUTE, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::point());
	else if(Type == "k3d::ri::vector") return detail::create_renderman_property<k3d::ri::vector_property>(k3d::irenderman_property::ATTRIBUTE, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::vector());
	else if(Type == "k3d::ri::normal") return detail::create_renderman_property<k3d::ri::normal_property>(k3d::irenderman_property::ATTRIBUTE, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::normal());
	else if(Type == "k3d::ri::hpoint") return detail::create_renderman_property<k3d::ri::hpoint_property>(k3d::irenderman_property::ATTRIBUTE, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::hpoint());
	else if(Type == "k3d::ri::matrix") return detail::create_renderman_property<k3d::ri::matrix_property>(k3d::irenderman_property::ATTRIBUTE, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::matrix());
	else if(Type == "k3d::ri::color") return detail::create_renderman_property<k3d::ri::color_property>(k3d::irenderman_property::ATTRIBUTE, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::color(0, 0, 0));

	throw std::invalid_argument("unknown renderman attribute type: " + Type);
}

iproperty iproperty_collection::add_ri_option(const std::string& Type, const std::string& List, const std::string& Name, const std::string& Label, const std::string& Description)
{
	k3d::inode* const node = dynamic_cast<k3d::inode*>(wrapped_ptr());
	if(!node)
		throw std::runtime_error("missing node");

	k3d::ipersistent_container* const persistent_container = dynamic_cast<k3d::ipersistent_container*>(wrapped_ptr());
	if(!persistent_container)
		throw std::runtime_error("missing persistent container");

	if(Type == "k3d::ri::integer") return detail::create_renderman_property<k3d::ri::integer_property>(k3d::irenderman_property::OPTION, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, 0);
	else if(Type == "k3d::ri::real") return detail::create_renderman_property<k3d::ri::real_property>(k3d::irenderman_property::OPTION, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, 0.0);
	else if(Type == "k3d::ri::string") return detail::create_renderman_property<k3d::ri::string_property>(k3d::irenderman_property::OPTION, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::string(""));
	else if(Type == "k3d::ri::point") return detail::create_renderman_property<k3d::ri::point_property>(k3d::irenderman_property::OPTION, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::point());
	else if(Type == "k3d::ri::vector") return detail::create_renderman_property<k3d::ri::vector_property>(k3d::irenderman_property::OPTION, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::vector());
	else if(Type == "k3d::ri::normal") return detail::create_renderman_property<k3d::ri::normal_property>(k3d::irenderman_property::OPTION, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::normal());
	else if(Type == "k3d::ri::hpoint") return detail::create_renderman_property<k3d::ri::hpoint_property>(k3d::irenderman_property::OPTION, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::hpoint());
	else if(Type == "k3d::ri::matrix") return detail::create_renderman_property<k3d::ri::matrix_property>(k3d::irenderman_property::OPTION, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::matrix());
	else if(Type == "k3d::ri::color") return detail::create_renderman_property<k3d::ri::color_property>(k3d::irenderman_property::OPTION, List, Name, Label, Description, node->document(), wrapped(), *persistent_container, node, k3d::ri::color(0, 0, 0));

	throw std::invalid_argument("unknown renderman option type: " + Type);
}

bool iproperty_collection::has_property(const std::string& Name)
{
	if (k3d::property::get(wrapped(), Name))
		return true;
	return false;
}

object iproperty_collection::getattr(const std::string& Name)
{
	if(k3d::iproperty* property = k3d::property::get(wrapped(), Name))
		return any_to_python(k3d::property::pipeline_value(*property));

	throw std::invalid_argument("unknown property: " + Name);
}

void iproperty_collection::setattr(const std::string& Name, const object& Value)
{
	if(k3d::iproperty* const property = k3d::property::get(wrapped(), Name))
	{
		if(k3d::iwritable_property* const writable = dynamic_cast<k3d::iwritable_property*>(property))
		{
			writable->property_set_value(python_to_any(Value, property->property_type()));
			return;
		}

		throw std::invalid_argument("read-only property: " + Name);
	}

	throw std::invalid_argument("unknown property: " + Name);
}

void iproperty_collection::define_class()
{
	class_<iproperty_collection>("iproperty_collection",
		"Abstract interface for a collection of L{iproperty} objects.", no_init)
		.def("properties", &iproperty_collection::properties,
			"Returns the set of all properties held within this collection.\n\n"
			"@return: A list of L{iproperty} objects.")
		.def("get_property", &iproperty_collection::get_property,
			"Returns a single property by name.\n\n"
			"@return: An L{iproperty} object, or None.")
		.def("add_user_property", &iproperty_collection::add_user_property,
			"Adds a custom user property to the collection.")
		.def("add_ri_attribute", &iproperty_collection::add_ri_attribute,
			"Adds a custom RenderMan attribute property to the collection.")
		.def("add_ri_option", &iproperty_collection::add_ri_option,
			"Adds a custom RenderMan option property to the collection.")
		.def("__getattr__", &iproperty_collection::getattr,
			"Returns the pipeline value of an L{iproperty} by name.")
		.def("__setattr__", &iproperty_collection::setattr,
			"Sets the internal value of an L{iproperty} value by name.")
		.def("has_property", &iproperty_collection::has_property,
			"True if the named property is registered");
}

} // namespace python

} // namespace k3d

