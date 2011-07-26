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

#include <boost/python.hpp>

#include <k3dsdk/inode.h>
#include <k3dsdk/property.h>
#include <k3dsdk/python/idocument_python.h>
#include <k3dsdk/python/iunknown_python.h>


namespace k3d
{

namespace python
{

class property
{
public:
	static void connect(idocument_wrapper& Document, iunknown_wrapper& From, iunknown_wrapper& To)
	{
		k3d::iproperty* const from = From.wrapped_ptr<k3d::iproperty>();
		if(!from)
			throw std::invalid_argument("From argument must be a valid property object.");

		k3d::iproperty* const to = To.wrapped_ptr<k3d::iproperty>();
		if(!to)
			throw std::invalid_argument("To argument must be a valid property object.");

		k3d::property::connect(Document.wrapped(), *from, *to);
	}

	static boost::python::object connection(idocument_wrapper& Document, iunknown_wrapper& Property)
	{
		k3d::iproperty* const property = Property.wrapped_ptr<k3d::iproperty>();
		if(!property)
			throw std::invalid_argument("Argument must be a valid property.");

		return wrap_unknown(k3d::property::connection(Document.wrapped(), *property));
	}

	static void disconnect(idocument_wrapper& Document, iunknown_wrapper& Property)
	{
		k3d::iproperty* const property = Property.wrapped_ptr<k3d::iproperty>();
		if(!property)
			throw std::invalid_argument("Argument must be a valid property.");

		k3d::property::disconnect(Document.wrapped(), *property);
	}

	static boost::python::object create(iunknown_wrapper& Self, const string_t& Type, const string_t& Name, const string_t& Label, const string_t& Description)
	{
		k3d::inode* const node = Self.wrapped_ptr<k3d::inode>();
		if(!node)
			throw std::runtime_error("missing node");

		k3d::iproperty* const result = k3d::property::create(*node, Type, Name, Label, Description);
		if(!result)
			throw std::invalid_argument("unknown user property type: " + Type);

		return wrap_unknown(result);
	}

	class ri
	{
	public:
		static boost::python::object create_attribute(iunknown_wrapper& Self, const string_t& Type, const string_t& AttributeName, const string_t& Name, const string_t& Label, const string_t& Description)
		{
			k3d::inode* const node = Self.wrapped_ptr<k3d::inode>();
			if(!node)
				throw std::runtime_error("missing node");

			k3d::iproperty* const result = k3d::property::ri::create_attribute(*node, Type, AttributeName, Name, Label, Description);
			if(!result)
				throw std::invalid_argument("unknown attribute type: " + Type);

			return wrap_unknown(result);
		}

		static boost::python::object create_option(iunknown_wrapper& Self, const string_t& Type, const string_t& OptionName, const string_t& Name, const string_t& Label, const string_t& Description)
		{
			k3d::inode* const node = Self.wrapped_ptr<k3d::inode>();
			if(!node)
				throw std::runtime_error("missing node");

			k3d::iproperty* const result = k3d::property::ri::create_option(*node, Type, OptionName, Name, Label, Description);
			if(!result)
				throw std::invalid_argument("unknown option type: " + Type);

			return wrap_unknown(result);
		}
	};
};

void define_namespace_property()
{
	boost::python::scope outer = boost::python::class_<property>("property", boost::python::no_init)
		.def("connect", property::connect,
			"Makes a pipeline connection between two properties.")
		.staticmethod("connect")

		.def("connection", property::connection,
			"Returns the connection (if any) coming into the given property.")
		.staticmethod("connection")

		.def("disconnect", property::disconnect,
			"Breaks the pipeline connection (if any) to the given property.")
		.staticmethod("disconnect")

		.def("create", property::create,
			"Adds a new property to an existing node.")
		.staticmethod("create")
		;

	boost::python::class_<property::ri>("ri", boost::python::no_init)
		.def("create_attribute", property::ri::create_attribute,
			"Adds a new RenderMan attribute property to an existing node.")
		.staticmethod("create_attribute")

		.def("create_option", property::ri::create_option,
			"Adds a new RenderMan option property to an existing node.")
		.staticmethod("create_option")
		;
}

} // namespace python

} // namespace k3d

