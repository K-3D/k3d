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

#include "aqsis_layer_chooser.h"
#include "entry.h"
#include "node_chooser.h"

#include <k3dsdk/iaqsis.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/result.h>

namespace libk3dngui
{

namespace aqsis_layer_chooser
{

/// Implementation of k3d::entry::idata_proxy for use with aqsis_layer_chooser
class source_proxy :
	public entry::idata_proxy
{
public:
	source_proxy(k3d::aqsis::ilayer_connection_property& Property, k3d::istate_recorder* const StateRecorder) :
		idata_proxy(StateRecorder, "source"),
		m_property(Property)
	{
	}

	const std::string value()
	{
		return m_property.get_source_variable();
	}

	void set_value(const std::string& Value)
	{
		m_property.set_source_variable(Value);
	}

	changed_signal_t& changed_signal()
	{
		return m_property.source_variable_changed_signal();
	}

private:
	k3d::aqsis::ilayer_connection_property& m_property;
};

/// Implementation of k3d::entry::idata_proxy for use with aqsis_layer_chooser
class target_proxy :
	public entry::idata_proxy
{
public:
	target_proxy(k3d::aqsis::ilayer_connection_property& Property, k3d::istate_recorder* const StateRecorder) :
		idata_proxy(StateRecorder, "target"),
		m_property(Property)
	{
	}

	const std::string value()
	{
		return m_property.get_target_variable();
	}

	void set_value(const std::string& Value)
	{
		m_property.set_target_variable(Value);
	}

	changed_signal_t& changed_signal()
	{
		return m_property.target_variable_changed_signal();
	}

private:
	k3d::aqsis::ilayer_connection_property& m_property;
};

/////////////////////////////////////////////////////////////////////////////
// control

control::control(document_state& DocumentState, k3d::aqsis::ilayer_connection_property& Property, k3d::icommand_node& Parent, const std::string& Name, k3d::istate_recorder* const StateRecorder) :
	base(false, 0),
	ui_component(Name, &Parent),
	m_property(Property)
{
	k3d::iproperty* const property = dynamic_cast<k3d::iproperty*>(&Property);
	return_if_fail(property);

	node_chooser::control* const chooser = new node_chooser::control(*this, "chooser", node_chooser::proxy(DocumentState, *property, StateRecorder, "chooser"), node_chooser::filter(*property));
	entry::control* const source_entry = new entry::control(*this, "source", std::auto_ptr<entry::idata_proxy>(new source_proxy(m_property, StateRecorder)));
	entry::control* const target_entry = new entry::control(*this, "target", std::auto_ptr<entry::idata_proxy>(new target_proxy(m_property, StateRecorder)));

	pack_start(*manage(chooser), Gtk::PACK_EXPAND_WIDGET);

//	Gtk::HBox* const hbox = new Gtk::HBox(true, 0);
//	pack_start(*Gtk::manage(hbox), Gtk::PACK_SHRINK);

	pack_start(*manage(source_entry), Gtk::PACK_SHRINK);
	pack_start(*manage(target_entry), Gtk::PACK_SHRINK);
}

} // namespace aqsis_layer_chooser

} // namespace libk3dngui


