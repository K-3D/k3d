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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "k3d-i18n-config.h"
#include "iplugin_factory.h"
#include "node.h"

#include <algorithm>
#include <iostream>

namespace k3d
{

/////////////////////////////////////////////////////////////////////////////
// node

node::node(iplugin_factory& Factory, idocument& Document) :
	property_collection(),
	m_factory(Factory),
	m_document(Document),
	m_name(init_owner(*this) + init_name("name") + init_label(_("Name")) + init_description(_("Assign a human-readable name to identify this node.")) + init_value<std::string>("")),
	m_selection_weight(init_owner(*this) + init_name("selection_weight") + init_label(_("Selection Weight")) + init_description(_("Node selection state, 1 = selected, 0 = unselected.")) + init_value(0.0))
{
	m_deleted_signal.connect(sigc::mem_fun(*this, &node::on_deleted));
	m_name.changed_signal().connect(sigc::hide(m_name_changed_signal.make_slot()));
}

node::~node()
{
}

void node::set_name(const std::string Name)
{
	m_name.set_value(Name);
}

const std::string node::name()
{
	return m_name.internal_value();
}

void node::on_deleted()
{
	// Signal that our properties are going away ...
	const property_collection::properties_t props = properties();
	for(property_collection::properties_t::const_iterator property = props.begin(); property != props.end(); ++property)
		(**property).property_deleted_signal().emit();
}

iplugin_factory& node::factory()
{
	return m_factory;
}

idocument& node::document()
{
	return m_document;
}

inode::deleted_signal_t& node::deleted_signal()
{
	return m_deleted_signal;
}

inode::name_changed_signal_t& node::name_changed_signal()
{
	return m_name_changed_signal;
}

void node::save(xml::element& Element, const ipersistent::save_context& Context)
{
	persistent_property_collection::save(Element, Context);
}

void node::load(xml::element& Element, const ipersistent::load_context& Context)
{
	// Load object name ...
	set_name(xml::attribute_text(Element, "name"));

	// Load Variables
	persistent_property_collection::load(Element, Context);
}

double node::get_selection_weight()
{
	return m_selection_weight.pipeline_value();
}

void node::set_selection_weight(const double Weight)
{
	m_selection_weight.set_value(Weight);
}

} // namespace k3d

