#ifndef K3DSDK_OBJECT_MODEL_H
#define K3DSDK_OBJECT_MODEL_H

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

#include <k3dsdk/iscript_engine.h>
namespace boost { namespace python { class dict; } }

extern "C" { void initk3d(); }


namespace k3d
{

namespace python
{

/// Inserts a script engine context into a Python dict
void set_context(const iscript_engine::context_t& Context, boost::python::dict& Dictionary);
/// Reflects changes to a script engine context back into C++ land
void get_context(boost::python::dict& Dictionary, iscript_engine::context_t& Context);

} // namespace python

} // namespace k3d

#endif // !K3DSDK_OBJECT_MODEL_H

