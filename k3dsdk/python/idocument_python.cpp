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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <boost/python.hpp>

#include <k3dsdk/python/idocument_python.h>
#include <k3dsdk/python/iproperty_python.h>
#include <k3dsdk/python/iunknown_python.h>

#include <k3dsdk/classes.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/idocument_exporter.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/iplugin_factory_collection.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nodes.h>
#include <k3dsdk/state_change_set.h>
#include <k3dsdk/utility_gl.h>

using namespace boost::python;

#include <boost/scoped_ptr.hpp>

namespace k3d
{

namespace python
{

static const bool save(idocument_wrapper& Self, const string_t& Path)
{
	boost::scoped_ptr<k3d::idocument_exporter> exporter(k3d::plugin::create<k3d::idocument_exporter>(k3d::classes::DocumentExporter()));
	if(!exporter)
		throw std::runtime_error("no exporter plugin available");

	return exporter->write_file(Self.wrapped(), filesystem::native_path(ustring::from_utf8(Path)));
}

static void start_change_set(idocument_wrapper& Self)
{
	k3d::start_state_change_set(Self.wrapped(), K3D_CHANGE_SET_CONTEXT);
}

static void cancel_change_set(idocument_wrapper& Self)
{
	k3d::cancel_state_change_set(Self.wrapped(), K3D_CHANGE_SET_CONTEXT);
}

static void finish_change_set(idocument_wrapper& Self, const string_t& Label)
{
	k3d::finish_state_change_set(Self.wrapped(), Label, K3D_CHANGE_SET_CONTEXT);
}

static void redraw_all(idocument_wrapper& Self)
{
	k3d::gl::redraw_all(Self.wrapped(), k3d::gl::irender_viewport::ASYNCHRONOUS);
}

static void delete_node(idocument_wrapper& Self, object& Node)
{
	extract<iunknown_wrapper> node(Node);
	if(!node.check())
		throw std::invalid_argument("argument isn't a node");

	k3d::delete_nodes(Self.wrapped(), k3d::make_collection<k3d::nodes_t>(node().wrapped_ptr<k3d::inode>()));
}

void define_class_idocument()
{
	class_<idocument_wrapper>("idocument")
		.def("save", &save)
		.def("start_change_set", &start_change_set)
		.def("cancel_change_set", &cancel_change_set)
		.def("finish_change_set", &finish_change_set)
		.def("redraw_all", &redraw_all)
		.def("delete_node", &delete_node)
		;
}

} // namespace python

} // namespace k3d

