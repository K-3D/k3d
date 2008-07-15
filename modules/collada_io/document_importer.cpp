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
#include <dom/domLibrary_geometries.h>
#include <dom/domLibrary_cameras.h>
#include <dom/domLibrary_visual_scenes.h>
#include <dom/domGeometry.h>
#include <dom/domMatrix.h>
#include <dom/domNode.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/itransform_sink.h>

#include <k3dsdk/plugins.h>

#include "intElements.h"
#include "integration.h"
#include "collada.h"

#include <boost/assign/list_of.hpp>

#include <iostream>
#include <vector>

namespace module
{

namespace collada
{

namespace io
{

k3d::matrix4 getTransformation(domNode& node)
{
	k3d::matrix4 result = k3d::identity3D();

	// Look for Translations
	domTranslate_Array translate_array = node.getTranslate_array();
	for(int i=0; i<translate_array.getCount(); i++)
	{
		domTranslate* translate = translate_array[i];
		domFloat3 trans = translate->getValue();
		k3d::matrix4 tmp_matrix = k3d::identity3D();
		tmp_matrix[0][3] = trans[0];
		tmp_matrix[1][3] = trans[1];
		tmp_matrix[2][3] = trans[2];
		result = result * tmp_matrix;
	}

	// Look for Scaling
	domScale_Array scale_array = node.getScale_array();
	for(int i=0; i<scale_array.getCount(); i++)
	{
		domScale* scale = scale_array[i];
		domFloat3 sc = scale->getValue();
		k3d::matrix4 tmp_matrix = k3d::identity3D();
		tmp_matrix[0][0] = sc[0];
		tmp_matrix[1][1] = sc[1];
		tmp_matrix[2][2] = sc[2];
		result = result * tmp_matrix;
	}

	// Look for Rotations
	domRotate_Array rotate_array = node.getRotate_array();
	for(int i=0; i<rotate_array.getCount(); i++)
	{
		domRotate* rotate = rotate_array[i];
		domFloat4 rot = rotate->getValue();
		k3d::matrix4 tmp_matrix = k3d::identity3D();
		float c = cos(k3d::radians(rot[3])), s = sin(k3d::radians(rot[3])), C = 1-c;
		float x = rot[0], y = rot[1], z = rot[2];
		float xs = x*s, ys = y*s, zs = z*s;
		float xC = x*C, yC = y*C, zC = z*C;
		float xyC=x*y*C,yzC=y*z*C,zxC=z*x*C;
		tmp_matrix[0][0] = x*xC+c;
		tmp_matrix[0][1] = xyC-zs;
		tmp_matrix[0][2] = zxC+ys;
		tmp_matrix[1][0] = xyC+zs;
		tmp_matrix[1][1] = y*yC+c;
		tmp_matrix[1][2] = yzC-xs;
		tmp_matrix[2][0] = zxC-ys;
		tmp_matrix[2][1] = yzC+xs;
		tmp_matrix[2][2] = z*zC+c;
		result = result * tmp_matrix;
	}

	// Look for complete transformation matrix
	domMatrix_Array matrix_array = node.getMatrix_array();
	for(int k=0; k<matrix_array.getCount(); k++)
	{
		domFloat4x4 mat = matrix_array[k]->getValue();
		k3d::matrix4 tmp_matrix = k3d::identity3D();
		for(int i=0; i<4; i++)
			for(int j=0; j<4; j++)
				tmp_matrix[i][j] = mat[4*i+j];
		result = result * tmp_matrix;
	}

	return result;
}


collada_obj lookcollada(std::vector<collada_obj> &collada_objs, std::string id)
{
	for(int i=0; i<collada_objs.size(); i++)
		if(id==collada_objs[i].get_id())
			return collada_objs[i];
	k3d::log() << error << "Could not connect collada object with instace!" << std::endl;
	return collada_objs[0];
}

class document_importer :
	public k3d::idocument_importer
{
public:
	bool read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
	{
		// Instantiate the reference implementation
		DAE dae;
		std::vector<collada_obj> collada_objs;

		k3d::log() << info << "Importing .dae file: " << FilePath.native_console_string() << std::endl;
		domCOLLADA* root = dae.open(k3d::string_cast<k3d::filesystem::path>(FilePath));
		if(!root) 
		{
            		k3d::log() << error << k3d_file_reference << ": error opening [" << FilePath.native_console_string() << "]" << std::endl;
			return false;
		}

		// First we add all possible objects from COLLADA libraries to Pipeline even if they aren't used. This is done
		// so a correct exporting is performed too.

		//Add all possible Geometries to pipeline
		domLibrary_geometries_Array library_geometries = root->getLibrary_geometries_array();
		for(int j=0; j<library_geometries.getCount(); j++)
		{
			domGeometry_Array geometries = library_geometries[j]->getGeometry_array();
			for(int i=0; i<geometries.getCount(); i++)
			{
				//k3d::log() << debug << "A geometry" << std::endl;
				collada_objs.push_back(collada_obj(Document,*geometries[i]));
			}
		}

		//Add all cameras to pipeline
		domLibrary_cameras_Array library_cameras = root->getLibrary_cameras_array();
		for(int j=0; j<library_cameras.getCount(); j++)
		{
			domCamera_Array cameras = library_cameras[j]->getCamera_array();
			for(int i=0; i<cameras.getCount(); i++)
				collada_objs.push_back(collada_obj(Document,*cameras[i]));
		}

		//Add all lights to pipeline
		domLibrary_lights_Array library_lights = root->getLibrary_lights_array();
		for(int j=0; j<library_lights.getCount(); j++)
		{
			domLight_Array lights = library_lights[j]->getLight_array();
			for(int i=0; i<lights.getCount(); i++)
				collada_objs.push_back(collada_obj(Document,*lights[i]));
		}

		//Add all images to pipeline
		domLibrary_images_Array library_images = root->getLibrary_images_array();
		for(int j=0; j<library_images.getCount(); j++)
		{
			domImage_Array images = library_images[j]->getImage_array();
			for(int i=0; i<images.getCount(); i++)
				collada_objs.push_back(collada_obj(Document,*images[i]));
		}

		domLibrary_visual_scenes_Array library_visual_scenes = root->getLibrary_visual_scenes_array();
		for(int j=0; j<library_visual_scenes.getCount(); j++)
		{
			domVisual_scene_Array visual_scenes = library_visual_scenes[j]->getVisual_scene_array();
			for(int k=0; k<visual_scenes.getCount(); k++)
			{
				//k3d::log() << debug << "A visual_scene" << std::endl;
				domNode_Array scene_nodes = visual_scenes[k]->getNode_array();
				for(int i=0; i<scene_nodes.getCount(); i++)
				{
					k3d::matrix4 mcurrent = getTransformation(*scene_nodes[i]);

					std::stringstream trans_name;
					trans_name << "COLLADA " << scene_nodes[i]->getName() << " Transformation";
					k3d::inode *frozen_trans = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup("FrozenTransformation"), Document, k3d::unique_name(Document.nodes(),trans_name.str()));
					k3d::property::set_internal_value(*frozen_trans, "matrix", mcurrent);
					k3d::itransform_source* const transform_source = dynamic_cast<k3d::itransform_source*>(frozen_trans);

					k3d::ipipeline::dependencies_t dependencies;

					for (size_t l = 0; l < scene_nodes[i]->getInstance_geometry_array().getCount(); l++)
					{
						domInstance_geometry* instanceGeom = scene_nodes[i]->getInstance_geometry_array()[l];
	
						std::stringstream instance_name;
						instance_name << "COLLADA " << scene_nodes[i]->getName() << " Instance";
						k3d::inode *mesh_instance = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup("MeshInstance"),Document,k3d::unique_name(Document.nodes(),instance_name.str()));

						// Set painters
						const k3d::nodes_t gl_nodes = k3d::find_nodes(Document.nodes(), "GL Default Painter");
						k3d::inode* gl_painter = (1 == gl_nodes.size()) ? *gl_nodes.begin() : 0;
						const k3d::nodes_t ri_nodes = k3d::find_nodes(Document.nodes(), "RenderMan Default Painter");
						k3d::inode* ri_painter = (1 == ri_nodes.size()) ? *ri_nodes.begin() : 0;
						k3d::property::set_internal_value(*mesh_instance, "gl_painter", gl_painter);
						k3d::property::set_internal_value(*mesh_instance, "ri_painter", ri_painter);

						// Connect the MeshInstance
						k3d::imesh_sink* const mesh_sink = dynamic_cast<k3d::imesh_sink*>(mesh_instance);

						dependencies.insert(std::make_pair(&mesh_sink->mesh_sink_input(), lookcollada(collada_objs,instanceGeom->getUrl().getElement()->getAttribute("id")).get_mesh_source_output()));

						k3d::itransform_sink* const transform_sink = dynamic_cast<k3d::itransform_sink*>(mesh_instance);

						dependencies.insert(std::make_pair(&transform_sink->transform_sink_input(),&transform_source->transform_source_output()));
					}

					for (size_t l = 0; l < scene_nodes[i]->getInstance_camera_array().getCount(); l++)
					{
						domInstance_camera* instanceCam = scene_nodes[i]->getInstance_camera_array()[l];

						std::stringstream cam_name;
						cam_name << "COLLADA " << scene_nodes[i]->getName();

						k3d::inode *camera = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup("Camera"), Document, cam_name.str());
						k3d::itransform_sink* const cam_transform_sink = dynamic_cast<k3d::itransform_sink*>(camera);

						dependencies.insert(std::make_pair(&cam_transform_sink->transform_sink_input(),&transform_source->transform_source_output()));

						dependencies.insert(std::make_pair(k3d::property::get(*camera,"top"),lookcollada(collada_objs,instanceCam->getUrl().getElement()->getAttribute("id")).get_top()));
						dependencies.insert(std::make_pair(k3d::property::get(*camera,"bottom"),lookcollada(collada_objs,instanceCam->getUrl().getElement()->getAttribute("id")).get_bottom()));
						dependencies.insert(std::make_pair(k3d::property::get(*camera,"left"),lookcollada(collada_objs,instanceCam->getUrl().getElement()->getAttribute("id")).get_left()));
						dependencies.insert(std::make_pair(k3d::property::get(*camera,"right"),lookcollada(collada_objs,instanceCam->getUrl().getElement()->getAttribute("id")).get_right()));
						dependencies.insert(std::make_pair(k3d::property::get(*camera,"near"),lookcollada(collada_objs,instanceCam->getUrl().getElement()->getAttribute("id")).get_near()));
						dependencies.insert(std::make_pair(k3d::property::get(*camera,"far"),lookcollada(collada_objs,instanceCam->getUrl().getElement()->getAttribute("id")).get_far()));
					}

					for (size_t l = 0; l < scene_nodes[i]->getInstance_light_array().getCount(); l++)
					{
						domInstance_light* instanceLight = scene_nodes[i]->getInstance_light_array()[l];

						std::stringstream light_name;
						light_name << "COLLADA " << scene_nodes[i]->getName();

						k3d::inode *light = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup("RenderManLight"), Document, light_name.str());
						k3d::itransform_sink* const light_transform_sink = dynamic_cast<k3d::itransform_sink*>(light);

						k3d::property::set_internal_value(*light, "shader", lookcollada(collada_objs, instanceLight->getUrl().getElement()->getAttribute("id")).get_light_shader());

						dependencies.insert(std::make_pair(&light_transform_sink->transform_sink_input(),&transform_source->transform_source_output()));
					}
					Document.pipeline().set_dependencies(dependencies);
				}
			}
		}

/*
		// Do the conversion. The conversion process throws an exception on error, so
		// we'll include a try/catch handler.
		//daeParser dae_file(*root, Document);

		//k3d::inode* const mesh_node = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup("Scale"), Document, "MyScale");
		//mesh_node->set_property("name", "Bla");
		k3d::iplugin_factory *factory = k3d::plugin::factory::lookup("Scale");
		if(!factory)
			k3d::log() << debug << "No factory found" << std::endl;
		else
			k3d::log() << debug << "Factory found" << std::endl;
*/

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
			k3d::uuid(0xd473c2a6, 0x45447d4c, 0x7fb4bfbc, 0xf0feda54),
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
