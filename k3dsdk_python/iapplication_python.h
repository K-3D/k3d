#ifndef K3DSDK_IAPPLICATION_PYTHON_H
#define K3DSDK_IAPPLICATION_PYTHON_H

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

#include "icommand_node_python.h"
#include "idocument_python.h"

namespace k3d
{

class iapplication;

namespace python
{

class iapplication :
	public interface_wrapper<k3d::iapplication>
{
	typedef interface_wrapper<k3d::iapplication> base;
public:
	iapplication();
	iapplication(k3d::iapplication* Application);

	void exit();

	idocument new_document();
	idocument open_document(const std::string& Path);
	void close_document(idocument& Document);
	
	icommand_node get_command_node(const std::string& Path);
};

void export_iapplication();

} // namespace python

} // namespace k3d

#endif // !K3DSDK_IAPPLICATION_PYTHON_H

