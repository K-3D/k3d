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

#include "dynamic_cast_python.h"
#include "icommand_node_python.h"
#include "idocument_exporter_python.h"
#include "idocument_importer_python.h"
#include "idocument_python.h"
#include "ifile_change_notifier_python.h"
#include "imaterial_python.h"
#include "imesh_storage_python.h"
#include "imetadata_python.h"
#include "inode_python.h"
#include "inode_selection_python.h"
#include "iproperty_python.h"
#include "iproperty_collection_python.h"
#include "isnappable_python.h"
#include "iunknown_python.h"
#include "node_python.h"

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

namespace detail
{

template<typename InterfaceT, typename WrapperT>
object do_dynamic_cast(k3d::iunknown* Unknown)
{
	if(InterfaceT* interface = dynamic_cast<InterfaceT*>(Unknown))
		return object(WrapperT(interface));

	return object();
}

template<typename InterfaceT>
object do_dynamic_cast(k3d::iunknown* Unknown)
{
	return wrap(dynamic_cast<InterfaceT*>(Unknown));
}

} // namespace detail
	
object do_dynamic_cast(k3d::iunknown* const Source, const string_t& Type)
{
	if(Type == "icommand_node")
		return detail::do_dynamic_cast<k3d::icommand_node>(Source);
	if(Type == "idocument_exporter")
		return detail::do_dynamic_cast<k3d::idocument_exporter>(Source);
	if(Type == "idocument_importer")
		return detail::do_dynamic_cast<k3d::idocument_importer>(Source);
	if(Type == "ifile_change_notifier")
		return detail::do_dynamic_cast<k3d::ifile_change_notifier>(Source);
	if(Type == "imaterial")
		return detail::do_dynamic_cast<k3d::imaterial>(Source);
	if(Type == "imesh_storage")
		return detail::do_dynamic_cast<k3d::imesh_storage>(Source);
	if(Type == "imetadata")
		return detail::do_dynamic_cast<k3d::imetadata>(Source);
	if(Type == "inode")
		return detail::do_dynamic_cast<k3d::inode>(Source);
	if(Type == "inode_selection")
		return detail::do_dynamic_cast<k3d::inode_selection>(Source);
	if(Type == "iproperty_collection")
		return detail::do_dynamic_cast<k3d::iproperty_collection>(Source);
	if(Type == "isnappable")
		return detail::do_dynamic_cast<k3d::isnappable>(Source);
	if(Type == "iunknown")
		return detail::do_dynamic_cast<k3d::iunknown>(Source);

	throw std::invalid_argument("unknown cast type: " + Type);
}

object do_dynamic_cast(const object& Source, const string_t& Type)
{
	extract<icommand_node_wrapper> icommand_node(Source);
	if(icommand_node.check())
		return do_dynamic_cast(icommand_node().wrapped_ptr(), Type);

	extract<idocument_exporter_wrapper> idocument_exporter(Source);
	if(idocument_exporter.check())
		return do_dynamic_cast(idocument_exporter().wrapped_ptr(), Type);

	extract<idocument_importer_wrapper> idocument_importer(Source);
	if(idocument_importer.check())
		return do_dynamic_cast(idocument_importer().wrapped_ptr(), Type);

	extract<idocument_wrapper> idocument(Source);
	if(idocument.check())
		return do_dynamic_cast(idocument().wrapped_ptr(), Type);

	extract<ifile_change_notifier_wrapper> ifile_change_notifier(Source);
	if(ifile_change_notifier.check())
		return do_dynamic_cast(ifile_change_notifier().wrapped_ptr(), Type);

	extract<imaterial_wrapper> imaterial(Source);
	if(imaterial.check())
		return do_dynamic_cast(imaterial().wrapped_ptr(), Type);

	extract<imesh_storage_wrapper> imesh_storage(Source);
	if(imesh_storage.check())
		return do_dynamic_cast(imesh_storage().wrapped_ptr(), Type);

	extract<imetadata_wrapper> imetadata(Source);
	if(imetadata.check())
		return do_dynamic_cast(imetadata().wrapped_ptr(), Type);

	extract<inode_wrapper> inode(Source);
	if(inode.check())
		return do_dynamic_cast(inode().wrapped_ptr(), Type);
	
	extract<inode_selection_wrapper> inode_selection(Source);
	if(inode_selection.check())
		return do_dynamic_cast(inode_selection().wrapped_ptr(), Type);

	extract<iproperty_wrapper> iproperty(Source);
	if(iproperty.check())
		return do_dynamic_cast(iproperty().wrapped_ptr(), Type);

	extract<iproperty_collection_wrapper> iproperty_collection(Source);
	if(iproperty_collection.check())
		return do_dynamic_cast(iproperty_collection().wrapped_ptr(), Type);

	extract<isnappable_wrapper> isnappable(Source);
	if(isnappable.check())
		return do_dynamic_cast(isnappable().wrapped_ptr(), Type);

	extract<k3d::python::node> node(Source);
	if(node.check())
		return do_dynamic_cast(node().inode_wrapper::wrapped_ptr(), Type);
	
	extract<iunknown_wrapper> iunknown(Source);
	if(iunknown.check())
		return do_dynamic_cast(iunknown().wrapped_ptr(), Type);

	throw std::invalid_argument("unknown source type for dynamic cast");
}

} // namespace python

} // namespace k3d

