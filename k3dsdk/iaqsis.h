#ifndef K3DSDK_IAQSIS_H
#define K3DSDK_IAQSIS_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Declares interfaces for rendering within the Pixar RenderMan specification
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "signal_system.h"

#include <string>

namespace k3d
{

namespace aqsis
{

/// Abstract interface implemented by objects that encapsulate an Aqsis displacement layer
class idisplacement_layer :
	public virtual iunknown
{
protected:
	idisplacement_layer() {}
	idisplacement_layer(const idisplacement_layer&) {}
	idisplacement_layer& operator=(const idisplacement_layer&) { return *this; }
	virtual ~idisplacement_layer() {}
};

/// Abstract interface implemented by objects that encapsulate an Aqsis surface layer
class isurface_layer :
	public virtual iunknown
{
protected:
	isurface_layer() {}
	isurface_layer(const isurface_layer&) {}
	isurface_layer& operator=(const isurface_layer&) { return *this; }
	virtual ~isurface_layer() {}
};

/// Abstract interface implemented by properties that encapsulate Aqsis layer connections
class ilayer_connection_property :
	public virtual iunknown
{
public:
	virtual const std::string get_source_variable() = 0;
	virtual void set_source_variable(const std::string& Variable) = 0;
	typedef sigc::signal<void, iunknown*> source_variable_changed_signal_t;
	virtual source_variable_changed_signal_t& source_variable_changed_signal() = 0;


	virtual const std::string get_target_variable() = 0;
	virtual void set_target_variable(const std::string& Variable) = 0;
	typedef sigc::signal<void, iunknown*> target_variable_changed_signal_t;
	virtual target_variable_changed_signal_t& target_variable_changed_signal() = 0;

protected:
	ilayer_connection_property() {}
	ilayer_connection_property(const ilayer_connection_property&) {}
	ilayer_connection_property& operator=(const ilayer_connection_property&) { return *this; }
	virtual ~ilayer_connection_property() {}
};

} // namespace aqsis

} // namespace k3d

#endif // K3DSDK_IAQSIS_INTERFACE_H


