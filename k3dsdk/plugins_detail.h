#ifndef K3DSDK_PLUGINS_DETAIL_H
#define K3DSDK_PLUGINS_DETAIL_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include "data.h"
#include "idocument.h"
#include "inode_collection.h"
#include "iplugin_factory.h"

namespace k3d
{

namespace plugin
{

namespace detail
{

iunknown* create_application_plugin(const k3d::string_t& FactoryName);
iunknown* create_application_plugin(iplugin_factory& Factory);
iunknown* create_application_plugin(const uuid& FactoryID);
inode* create_document_plugin(const k3d::string_t& FactoryName, idocument& Document, const k3d::string_t& Name);
inode* create_document_plugin(iplugin_factory& Factory, idocument& Document, const k3d::string_t& Name);
inode* create_document_plugin(const uuid& FactoryID, idocument& Document, const k3d::string_t& Name);

} // namespace detail

} // namespace plugin

} // namespace k3d

#endif // !K3DSDK_PLUGINS_DETAIL_H

