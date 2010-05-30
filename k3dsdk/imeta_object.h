#ifndef K3DSDK_IMETA_OBJECT_H
#define K3DSDK_IMETA_OBJECT_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3dsdk/iunknown.h>
#include <k3dsdk/types.h>
#include <boost/any.hpp>
#include <vector>

namespace k3d
{

/// Abstract interface for objects that can accept commands from external entities
class imeta_object :
	public virtual iunknown
{
public:
	/// Executes an object method that takes zero arguments.
	const boost::any execute(const string_t& Method);
	/// Executes an object method that takes one argument.
	const boost::any execute(const string_t& Method, const boost::any& Argument1);
	/// Executes an object method that takes two arguments.
	const boost::any execute(const string_t& Method, const boost::any& Argument1, const boost::any& Argument2);
	/// Executes an object method that takes three arguments.
	const boost::any execute(const string_t& Method, const boost::any& Argument1, const boost::any& Argument2, const boost::any& Argument3);
	/// Executes an object method with an arbitrary number of arguments.
	virtual const boost::any execute(const string_t& Method, const std::vector<boost::any>& Arguments) = 0;

protected:
	imeta_object() {}
	imeta_object(const imeta_object& Other) : iunknown(Other) {}
	imeta_object& operator=(const imeta_object&) { return *this; }
	virtual ~imeta_object() {}
};

} // namespace k3d

#endif // !K3DSDK_IMETA_OBJECT_H

