#ifndef MODULES_OPENCASCADE_OPENCASCADE_TO_K3D_H
#define MODULES_OPENCASCADE_OPENCASCADE_TO_K3D_H

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

/** \file Operations for reading file formats supported by OpenCascade
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

// forward declarations
namespace k3d
{ 
namespace filesystem { class path; }
class gprim_factory;
}

namespace module
{

namespace opencascade
{


namespace detail { class implementation; }

/// Processes all nodes of an OpenCascade document
class opencascade_document_processor
{
public:
	/// Initialise with path to OpenCascade-readable file.
	opencascade_document_processor(const k3d::filesystem::path& FilePath);
	~opencascade_document_processor();
	/// Process the current node in the document using the supplied gprim_factory. Name will contain the name of the node
	void process_current(k3d::gprim_factory& Factory, std::string& Name) const;
	/// True if the current node has children
	bool has_children() const;
	/// Increase the level, to explore the children of the current node. Silently fails if the level can not be increased
	void increase_level();
	/// Decrease the level, to continue exploring the parent hierarchy. Silently fails if we are already at the top level
	void decrease_level();
	/// Move on to the next node of the current level
	void next();
	/// True if there are more nodes to be processed at the current level
	bool more() const;
	
private:
	detail::implementation* m_implementation;
};

} // opencascade

} // module

#endif // !MODULES_OPENCASCADE_OPENCASCADE_TO_K3D_H
