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

#include "metadata.h"

namespace k3d
{

namespace metadata
{

////////////////////////////////////////////////////////////////////////////////////////////
// storage

void storage::set_metadata_value(const string_t& Name, const string_t& Value)
{
	m_storage[Name] = Value;
	m_changed_signal.emit();
}

void storage::set_metadata(const metadata_t& Values)
{
	// Note ... we don't use insert() here because we want to overwrite any existing values
	for(metadata_t::const_iterator pair = Values.begin(); pair != Values.end(); ++pair)
		m_storage[pair->first] = pair->second;
	m_changed_signal.emit();
}

storage::metadata_t storage::get_metadata()
{
	return m_storage;
}

const string_t storage::get_metadata_value(const string_t& Name)
{
	metadata_t::const_iterator pair = m_storage.find(Name);
	return pair != m_storage.end() ? pair->second : string_t();
}

void storage::erase_metadata_value(const string_t& Name)
{
	m_storage.erase(Name);
	m_changed_signal.emit();
}

sigc::connection storage::connect_metadata_changed_signal(const sigc::slot<void>& Slot)
{
	return m_changed_signal.connect(Slot);
}

} // namespace metadata

} // namespace k3d

