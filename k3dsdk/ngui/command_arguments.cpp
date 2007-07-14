// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "viewport.h"
#include "command_arguments.h"

#include <k3dsdk/command_node.h>
#include <k3dsdk/inode_name_map.h>

#include <sstream>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// command_arguments

command_arguments::command_arguments() :
	m_storage(new k3d::xml::element("arguments"))
{
}

command_arguments::command_arguments(const std::string& Arguments) :
	m_storage(new k3d::xml::element())
{
	std::stringstream buffer(Arguments);
	buffer >> *m_storage;

	if(m_storage->name != "arguments")
		throw std::runtime_error("Invalid command_arguments data");
}

command_arguments::~command_arguments()
{
	delete m_storage;
}

void command_arguments::append_viewport(viewport::control& Viewport)
{
	if(!k3d::xml::find_element(*m_storage, "viewport"))
		m_storage->append(k3d::xml::element("viewport", k3d::command_node::path(Viewport)));
}

void command_arguments::append_viewport_coordinates(const std::string& Name, viewport::control& Viewport, const GdkEventButton& Event)
{
	append_viewport(Viewport);
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(widget_to_ndc(Viewport, k3d::point2(Event.x, Event.y)))));
}

void command_arguments::append_viewport_coordinates(const std::string& Name, viewport::control& Viewport, const GdkEventMotion& Event)
{
	append_viewport(Viewport);
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(widget_to_ndc(Viewport, k3d::point2(Event.x, Event.y)))));
}

void command_arguments::append_viewport_coordinates(const std::string& Name, viewport::control& Viewport, const GdkEventScroll& Event)
{
	append_viewport(Viewport);
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(widget_to_ndc(Viewport, k3d::point2(Event.x, Event.y)))));
}

void command_arguments::append_viewport_coordinates(const std::string& Name, viewport::control& Viewport, const k3d::rectangle& Value)
{
	append_viewport(Viewport);
	const k3d::rectangle value(widget_to_ndc(Viewport, Value.top_left()), widget_to_ndc(Viewport, Value.bottom_right()));
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(value)));
}

void command_arguments::append(const std::string& Name, const bool& Value)
{
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(Value)));
}

void command_arguments::append(const std::string& Name, const double& Value)
{
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(Value)));
}

void command_arguments::append(const std::string& Name, const k3d::matrix4& Value)
{
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(Value)));
}

void command_arguments::append(const std::string& Name, const k3d::point2& Value)
{
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(Value)));
}

void command_arguments::append(const std::string& Name, const k3d::point3& Value)
{
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(Value)));
}

void command_arguments::append(const std::string& Name, const k3d::vector2& Value)
{
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(Value)));
}

void command_arguments::append(const std::string& Name, const k3d::vector3& Value)
{
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(Value)));
}

void command_arguments::append(const std::string& Name, const k3d::rectangle& Value)
{
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(Value)));
}

void command_arguments::append(const std::string& Name, const k3d::angle_axis& Value)
{
	m_storage->append(k3d::xml::element(Name, k3d::string_cast(Value)));
}

void command_arguments::append(const std::string& Name, const std::string& Value)
{
	m_storage->append(k3d::xml::element(Name, Value));
}

void command_arguments::append(const std::string& Name, const char* const Value)
{
	m_storage->append(k3d::xml::element(Name, Value));
}

/** \note Be careful to keep this implementation in-sync with k3d::selection::record and k3d::selection::token! */
void command_arguments::append(const std::string& Name, const k3d::selection::record& Value)
{
	std::ostringstream buffer;
	buffer << Value.zmin << " " << Value.zmax << " " << Value.tokens.size();
	for(k3d::selection::record::tokens_t::const_iterator token = Value.tokens.begin(); token != Value.tokens.end(); ++token)
		buffer << " " << static_cast<size_t>(token->type) << " " << token->id;

	k3d::xml::element& xml_storage = m_storage->append(k3d::xml::element(Name, buffer.str()));
	if(k3d::inode* const node = k3d::selection::get_node(Value))
		xml_storage.append(k3d::xml::attribute("node", node->name()));
}

/** \note Be careful to keep this implementation in-sync with k3d::selection::records, k3d::selection::record and k3d::selection::token! */
void command_arguments::append(const std::string& Name, const k3d::selection::records& Value)
{
	k3d::xml::element& xml_storage = m_storage->append(k3d::xml::element(Name));
	for(k3d::selection::records::const_iterator record = Value.begin(); record != Value.end(); ++record)
	{
		std::ostringstream buffer;
		buffer << record->zmin << " " << record->zmax << " " << record->tokens.size();
		for(k3d::selection::record::tokens_t::const_iterator token = record->tokens.begin(); token != record->tokens.end(); ++token)
			buffer << " " << static_cast<size_t>(token->type) << " " << token->id;

		k3d::xml::element& xml_record = xml_storage.append(k3d::xml::element("r", buffer.str()));
		if(k3d::inode* const node = k3d::selection::get_node(*record))
			xml_record.append(k3d::xml::attribute("node", node->name()));
	}
}

void command_arguments::append(const std::string& Name, k3d::inode* const Value)
{
	m_storage->append(k3d::xml::element(Name, Value ? Value->document().unique_node_names().name(*Value) : ""));
}

void command_arguments::append(const k3d::xml::element& Value)
{
	m_storage->append(Value);
}

command_arguments::operator std::string() const
{
	std::stringstream buffer;
	buffer << k3d::xml::single_line() << *m_storage;
	return buffer.str();
}

viewport::control& command_arguments::get_viewport() const
{
	k3d::xml::element* const xml_storage = get_storage("viewport");

	k3d::icommand_node* const viewport_node = k3d::command_node::lookup(xml_storage->text);
	if(!viewport_node)
		throw std::runtime_error("viewport node not found");

	viewport::control* const viewport_control = dynamic_cast<viewport::control*>(viewport_node);
	if(!viewport_control)
		throw std::runtime_error("not a viewport control");

	/** \todo is this really necessary? */
	if(!viewport_control->gl_engine())
		throw std::runtime_error("viewport unattached");

	return *viewport_control;
}

const k3d::point2 command_arguments::get_viewport_point2(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	viewport::control& viewport = get_viewport();

	return ndc_to_widget(viewport, k3d::from_string<k3d::point2>(xml_storage->text, k3d::point2(0, 0)));
}

const k3d::rectangle command_arguments::get_viewport_rectangle(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	viewport::control& viewport = get_viewport();

	const k3d::rectangle temp = k3d::from_string<k3d::rectangle>(xml_storage->text, k3d::rectangle(0, 0, 0, 0));
	return k3d::rectangle(ndc_to_widget(viewport, temp.top_left()), ndc_to_widget(viewport, temp.bottom_right()));
}

const bool command_arguments::get_bool(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	return k3d::from_string<bool>(xml_storage->text, false);
}

const double command_arguments::get_double(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	return k3d::from_string<double>(xml_storage->text, 0.0);
}

const k3d::matrix4 command_arguments::get_matrix4(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	return k3d::from_string<k3d::matrix4>(xml_storage->text, k3d::identity3D());
}

const k3d::point2 command_arguments::get_point2(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	return k3d::from_string<k3d::point2>(xml_storage->text, k3d::point2(0, 0));
}

const k3d::point3 command_arguments::get_point3(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	return k3d::from_string<k3d::point3>(xml_storage->text, k3d::point3(0, 0, 0));
}

const k3d::vector2 command_arguments::get_vector2(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	return k3d::from_string<k3d::vector2>(xml_storage->text, k3d::vector2(0, 0));
}

const k3d::vector3 command_arguments::get_vector3(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	return k3d::from_string<k3d::vector3>(xml_storage->text, k3d::vector3(0, 0, 0));
}

const k3d::rectangle command_arguments::get_rectangle(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	return k3d::from_string<k3d::rectangle>(xml_storage->text, k3d::rectangle(0, 0, 0, 0));
}

const k3d::angle_axis command_arguments::get_angle_axis(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	return k3d::from_string<k3d::angle_axis>(xml_storage->text, k3d::angle_axis(0, 1, 0, 0));
}

const std::string command_arguments::get_string(const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	return xml_storage->text;
}

/** \note Be careful to keep this implementation in-sync with k3d::selection::record and k3d::selection::token! */
const k3d::selection::record command_arguments::get_selection_record(k3d::idocument& Document, const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	std::istringstream buffer(xml_storage->text);

	size_t count = 0;
	k3d::selection::record record;
	buffer >> record.zmin >> record.zmax >> count;

	size_t type;
	k3d::selection::token token;
	while(buffer && count)
	{
		buffer >> type >> token.id;
		token.type = static_cast<k3d::selection::type>(type);
		record.tokens.push_back(token);
		--count;
	}

	const std::string node_name = k3d::xml::attribute_text(*xml_storage, "node");
	if(!node_name.empty())
	{
		k3d::inode* const node = k3d::find_node(Document.nodes(), node_name);
		if(!node)
			throw std::runtime_error("couldn't find node [" + node_name + "]");

		for(k3d::selection::record::tokens_t::iterator token = record.tokens.begin(); token != record.tokens.end(); ++token)
		{
			if(token->type == k3d::selection::NODE)
			{
				token->id = k3d::selection::node_id(node);
				break;
			}
		}
	}

	return record;
}

/** \note Be careful to keep this implementation in-sync with k3d::selection::records, k3d::selection::record and k3d::selection::token! */
const k3d::selection::records command_arguments::get_selection_records(k3d::idocument& Document, const std::string& Name) const
{
	k3d::selection::records result;

	k3d::xml::element* const xml_root_storage = get_storage(Name);
	for(k3d::xml::element::elements_t::const_iterator xml_storage = xml_root_storage->children.begin(); xml_storage != xml_root_storage->children.end(); ++xml_storage)
	{
		if(xml_storage->name != "r")
			continue;

		std::istringstream buffer(xml_storage->text);

		size_t count = 0;
		k3d::selection::record record;
		buffer >> record.zmin >> record.zmax >> count;

		size_t type;
		k3d::selection::token token;
		while(buffer && count)
		{
			buffer >> type >> token.id;
			token.type = static_cast<k3d::selection::type>(type);
			record.tokens.push_back(token);
			--count;
		}

		const std::string node_name = k3d::xml::attribute_text(*xml_storage, "node");
		if(!node_name.empty())
		{
			k3d::inode* const node = k3d::find_node(Document.nodes(), node_name);
			if(!node)
				throw std::runtime_error("couldn't find node [" + node_name + "]");

			for(k3d::selection::record::tokens_t::iterator token = record.tokens.begin(); token != record.tokens.end(); ++token)
			{
				if(token->type == k3d::selection::NODE)
				{
					token->id = k3d::selection::node_id(node);
					break;
				}
			}
		}

		result.push_back(record);
	}

	return result;
}

k3d::inode* const command_arguments::get_node(k3d::idocument& Document, const std::string& Name) const
{
	k3d::xml::element* const xml_storage = get_storage(Name);
	if(xml_storage->text.empty())
		return 0;

	return Document.unique_node_names().node(xml_storage->text);
}

const k3d::xml::element command_arguments::get_element(const std::string& Name) const
{
	return *get_storage(Name);
}

k3d::xml::element* command_arguments::get_storage(const std::string& Name) const
{
	k3d::xml::element* const result = k3d::xml::find_element(*m_storage, Name);
	if(!result)
		throw std::runtime_error("missing storage for [" + Name + "]");

	return result;
}

} // namespace libk3dngui

