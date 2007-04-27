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

#include "iapplication_python.h"

#include <k3dsdk/application.h>
#include <k3dsdk/auto_ptr.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/command_node.h>
#include <k3dsdk/create_plugins.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument_read_format.h>
#include <k3dsdk/path.h>

#include <boost/python.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

iapplication::iapplication() :
	base()
{
}

iapplication::iapplication(k3d::iapplication* Application) :
	base(Application)
{
}

void iapplication::exit()
{
	wrapped().exit();
}

idocument iapplication::new_document()
{
	return idocument(wrapped().create_document());
}

idocument iapplication::open_document(const std::string& Path)
{
	const filesystem::path document_path = filesystem::native_path(ustring::from_utf8(Path));

	k3d::auto_ptr<k3d::idocument_read_format> filter(k3d::create_plugin<k3d::idocument_read_format>(k3d::classes::DocumentReader()));
	if(!filter.get())
		throw std::runtime_error("no document loader plugin");

	k3d::idocument* const document = k3d::application().create_document();
	if(!document)
		throw std::runtime_error("couldn't create empty document");

	if(!filter->read_file(*document, document_path))
		throw std::runtime_error("error loading document");

	return idocument(document);
}

void iapplication::close_document(idocument& Document)
{
	wrapped().close_document(Document.wrapped());
}

icommand_node iapplication::get_command_node(const std::string& Path)
{
	return icommand_node(k3d::command_node::lookup(Path));
}

void export_iapplication()
{
	class_<iapplication>("iapplication")
		.def("exit", &iapplication::exit)
		.def("new_document", &iapplication::new_document)
		.def("open_document", &iapplication::open_document)
		.def("close_document", &iapplication::close_document)
		.def("get_command_node", &iapplication::get_command_node);
}

} // namespace python

} // namespace k3d

