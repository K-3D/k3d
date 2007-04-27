#ifndef K3DSDK_IDOCUMENT_PYTHON_H
#define K3DSDK_IDOCUMENT_PYTHON_H

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

#include "iproperty_python.h"

#include <boost/python/list.hpp>

namespace k3d
{
	
class idocument;

namespace python
{

class idocument :
	public interface_wrapper<k3d::idocument>
{
	typedef interface_wrapper<k3d::idocument> base;
public:
	idocument();
	idocument(k3d::idocument* Document);
	idocument(k3d::idocument& Document);

	const bool save(const std::string& Path);
	
	void start_change_set();
	void cancel_change_set();
	void finish_change_set(const std::string& Label);
	
	void redraw_all();

	const boost::python::list nodes();
	const boost::python::object new_node(const boost::python::object& Type);
	const boost::python::object get_node(const std::string& Name);
	void delete_node(boost::python::object& Node);

	boost::python::object get_dependency(iproperty& Property);
	void set_dependency(iproperty& LHS, iproperty& RHS);
};

void export_idocument();

} // namespace python

} // namespace k3d

#endif // !K3DSDK_IDOCUMENT_PYTHON_H

