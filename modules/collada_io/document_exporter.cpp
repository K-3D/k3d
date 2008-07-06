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
		\author Carlos Andres Dominguez Caballero (carlosadc@gmail.com)
*/

#include <dae.h>
#include <dom/domCOLLADA.h>
#include <dom/domLight.h>
#include <dom/domCamera.h>

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/dependencies.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_exporter.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/xml.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/data.h>
#include <k3dsdk/algebra.h>
#include <string>

namespace module
{
namespace collada
{
namespace io
{

template <class T>
std::string to_string(T & t)
{
        std::stringstream s;
        s << t;
        return s.str();
}

class document_exporter :
	public k3d::idocument_exporter
{
public:
	bool write_file(k3d::idocument& Document, const k3d::filesystem::path& Path)
	{
		k3d::log() << info << "Writing " << Path.native_console_string() << " using " << get_factory().name() << std::endl;
		DAE dae;
		daeElement *root = dae.add(k3d::string_cast<k3d::filesystem::path>(Path));

		daeElement *asset = root->add("asset");
		daeElement *library_cameras = root->add("library_cameras");
		daeElement *library_lights = root->add("library_lights");

		daeElement* contributor = asset->add("contributor");
		daeElement* created = asset->add("created");
		daeElement* modified = asset->add("modified");

		std::stringstream author;
		author << "K-3D " << K3D_PACKAGE << " , " << K3D_VERSION << " , " << K3D_HOST;
		contributor->setCharData(author.str().c_str());
		const char* date = "2008-04-08T13:07:52-08:00";
		created->setCharData(date);
		modified->setCharData(date);

		// Sort objects by ID before saving them ...
		const k3d::inode_collection::nodes_t nodes = Document.nodes().collection();

		for(k3d::inode_collection::nodes_t::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if((*node)->factory().factory_id() == k3d::classes::Camera())
			{
				domCamera *camera = daeSafeCast<domCamera>(library_cameras->add("camera"));
				domCamera::domOptics::domTechnique_common *technique = daeSafeCast<domCamera::domOptics::domTechnique_common>(camera->add("optics technique_common"));
				camera->setName((*node)->name().c_str());
				camera->setId((*node)->name().c_str());

				double zfar = boost::any_cast<double>(k3d::property::get(**node, "far")->property_internal_value());
				double znear = boost::any_cast<double>(k3d::property::get(**node, "near")->property_internal_value());
				double left = boost::any_cast<double>(k3d::property::get(**node, "left")->property_internal_value());
				double right = boost::any_cast<double>(k3d::property::get(**node, "right")->property_internal_value());
				double top = boost::any_cast<double>(k3d::property::get(**node, "top")->property_internal_value());
				double bottom = boost::any_cast<double>(k3d::property::get(**node, "bottom")->property_internal_value());
				double aspect_ratio = (right-left)/(top-bottom);
				

				bool ortho = boost::any_cast<bool>(k3d::property::get(**node, "orthographic")->property_internal_value());

				domElement *cam_type;
				if(ortho)
				{
					cam_type = technique->add("orthographic");
					double xmag = (right-left)/2;
					domElement *xm = cam_type->add("xmag");
					xm->setCharData(to_string(xmag).c_str());
					double ymag = (top-bottom)/2;
				}
				else
				{
					cam_type = technique->add("perspective");
					double yfov = k3d::degrees(2*atan((top-bottom)/(2*znear)));
					domElement *yf = cam_type->add("yfov");
					yf->setCharData(to_string(yfov).c_str());
				}

				domElement *near = cam_type->add("znear");
				near->setCharData(to_string(znear).c_str());
				domElement *far = cam_type->add("zfar");
				far->setCharData(to_string(zfar).c_str());
				domElement *aspect = cam_type->add("aspect_ratio");
				aspect->setCharData(to_string(aspect_ratio).c_str());
			}
			if((*node)->factory().factory_id() == k3d::classes::FrozenMesh())
			{
				
			}
		}

		dae.writeAll();
		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<document_exporter, k3d::interface_list<k3d::idocument_exporter> > factory(
			k3d::uuid(0x33f1ba2c, 0x5b41392d, 0xe9dc9d99, 0x66a8129d),
			"COLLADADocumentExporter",
			_("COLLADA ( .dae )"),
			"");

		return factory;
	}
};

k3d::iplugin_factory& document_exporter_factory()
{
	return document_exporter::get_factory();
}

}
}
}
