// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\brief Implements the DocumentReader COLLADA plugin, which imports the COLLADA(.dae) file format
		\author Carlos Andres Dominguez Caballero (carlosadc@gmail.com)
*/

#include <dae.h>
#include <dom/domCOLLADA.h>
#include <dom/domConstants.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/data.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/gzstream.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/idocument_plugin_factory.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/log.h>
#include <k3dsdk/persistent_lookup.h>
#include <k3dsdk/serialization_xml.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/vectors.h>
#include <k3dsdk/xml.h>
#include "intElements.h"
#include "integration.h"

#include <boost/assign/list_of.hpp>

#include <iostream>

namespace module
{

namespace collada
{

namespace io
{

class document_importer :
	public k3d::idocument_importer
{
public:
	bool read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
	{
		// Instantiate the reference implementation
		DAE dae;

		k3d::log() << info << "Importing .dae file: " << FilePath.native_console_string() << std::endl;
		domCOLLADA* root = dae.open(k3d::string_cast<k3d::filesystem::path>(FilePath));
		if(!root) 
		{
            		k3d::log() << error << k3d_file_reference << ": error opening [" << FilePath.native_console_string() << "]" << std::endl;
			return false;
		}

		// Do the conversion. The conversion process throws an exception on error, so
		// we'll include a try/catch handler.
		//convertModel(*root, Mesh);
		daeParser dae_file(*root, Document);
	
		// destroy the objects we created during the conversion process
		freeConversionObjects<Node, domNode>(dae);
		freeConversionObjects<intGeometry, domGeometry>(dae);
		freeConversionObjects<intLight, domLight>(dae);
		freeConversionObjects<intCamera, domCamera>(dae);
		
		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<document_importer, k3d::interface_list<k3d::idocument_importer> > factory(
			k3d::classes::DocumentImporter(),
			"COLLADADocumentImporter",
			_("COLLADA ( .dae )"),
			"",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("k3d:mime-types", "application/x-k3d"));

		return factory;
	}
};

k3d::iplugin_factory& document_importer_factory()
{
	return document_importer::get_factory();
}

}
}
}
