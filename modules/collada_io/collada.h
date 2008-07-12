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
	\author Carlos Andres Dominguez Caballero (carlosadc@gmail.com)
*/

#ifndef COLLADA_H
#define COLLADA_H

#include <k3dsdk/mesh.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/ustring.h>
#include <dom/domGeometry.h>
#include "intElements.h"

namespace module
{
namespace collada
{
namespace io
{

	k3d::mesh to_k3d_mesh(domGeometry &geom)
	{
		intGeometry result(geom,k3d::identity3D());
		return result.getMesh();
	}

	class collada_obj
	{
	public:
		collada_obj(k3d::idocument &Document, domGeometry &geom)
		{
			id = geom.getAttribute("id");
			name = "COLLADA " + k3d::unique_name(Document.nodes(), geom.getAttribute("name"));
			type = "frozen_mesh";

			k3d::mesh *mesh = new k3d::mesh();
			//mesh = to_k3d_mesh(geom);
			k3d::inode *frozen_mesh = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup("FrozenMesh"), Document, name);
			k3d::imesh_storage* mesh_storage = dynamic_cast<k3d::imesh_storage*>(frozen_mesh);
			mesh_storage->reset_mesh(mesh);
			*mesh  = to_k3d_mesh(geom);

			k3d::imesh_source* const mesh_source = dynamic_cast<k3d::imesh_source*>(frozen_mesh);
			mesh_source_output = &mesh_source->mesh_source_output();
		}
		collada_obj(k3d::idocument &Document, domCamera &cam)
		{
			id = cam.getAttribute("id");
			name = "COLLADA " + k3d::unique_name(Document.nodes(), cam.getAttribute("name"));
			type = "camera";

			k3d::inode *camera = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup("Camera"), Document, name);

			domCamera::domOptics::domTechnique_common *technique_common = cam.getOptics()->getTechnique_common();
			domCamera::domOptics::domTechnique_common::domPerspective *perspective = technique_common->getPerspective();
			domCamera::domOptics::domTechnique_common::domOrthographic *orthographic = technique_common->getOrthographic();
			double znear, zfar;
			double aspect_ratio;// = perspective->getAspect_ratio()->getValue();
			if(orthographic!=NULL)
			{
				double xmag = orthographic->getXmag()->getValue();
				double ymag = orthographic->getYmag()->getValue();

				if(orthographic->getAspect_ratio()!=NULL)
					aspect_ratio = orthographic->getAspect_ratio()->getValue();
				else
					aspect_ratio = xmag/ymag;

				k3d::property::set_internal_value(*camera, "top", ymag);
				k3d::property::set_internal_value(*camera, "bottom", -ymag);
				k3d::property::set_internal_value(*camera, "right", xmag);
				k3d::property::set_internal_value(*camera, "left", -xmag);
				znear = orthographic->getZnear()->getValue();
				zfar = orthographic->getZfar()->getValue();
			}
			if(perspective!=NULL)
			{
				if(perspective->getAspect_ratio()!=NULL)
					aspect_ratio = perspective->getAspect_ratio()->getValue();
				else
					aspect_ratio = 1;
				double yfov = perspective->getYfov()->getValue();
				znear = perspective->getZnear()->getValue();
				zfar = perspective->getZfar()->getValue();
				double x = tan(k3d::radians(yfov/2))*znear;
				k3d::property::set_internal_value(*camera, "top", x);
				k3d::property::set_internal_value(*camera, "bottom", x);
				k3d::property::set_internal_value(*camera, "left", -x*aspect_ratio);
				k3d::property::set_internal_value(*camera, "right", x*aspect_ratio);
			}

			k3d::property::set_internal_value(*camera, "viewport_visible", false);
			k3d::property::set_internal_value(*camera, "near", znear);
			k3d::property::set_internal_value(*camera, "far", zfar);
			//k3d::property::set_internal_value(*camera, "

			//Collect properties
			top = k3d::property::get(*camera, "top");
			bottom = k3d::property::get(*camera, "bottom");
			left = k3d::property::get(*camera, "left");
			right = k3d::property::get(*camera, "right");
			near = k3d::property::get(*camera, "near");
			far = k3d::property::get(*camera, "far");
		}

		collada_obj(k3d::idocument &Document, domLight &li)
		{
			id = li.getAttribute("id");
			name = "COLLADA " + k3d::unique_name(Document.nodes(), li.getAttribute("name"));
			std::string shader_name = k3d::unique_name(Document.nodes(), name + " Shader");
			type = "light";

			domLight::domTechnique_common *technique_common = li.getTechnique_common();
			domLight::domTechnique_common::domPoint *point = technique_common->getPoint();
			domLight::domTechnique_common::domAmbient *ambient = technique_common->getAmbient();
			domLight::domTechnique_common::domDirectional *directional = technique_common->getDirectional();
			domLight::domTechnique_common::domSpot *spot = technique_common->getSpot();

			k3d::inode *light = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup("RenderManLight"), Document, name);
			light_shader = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup("RenderManLightShader"), Document, shader_name);

			k3d::property::set_internal_value(*light, "viewport_visible", false);
			k3d::property::set_internal_value(*light, "shader", light_shader);
			//k3d::filesystem::path shader_path;

			if(point!=NULL)
			{
				//shader_path = k3d::filesystem::generic_path("k3d_pointlight.sl");
			}

			//k3d::property::set_internal_value(*light_shader, "shader_path", shader_path);
		}

		collada_obj(k3d::idocument &Document, domImage &img)
		{
			id = img.getAttribute("id");
			name = "COLLADA " + k3d::unique_name(Document.nodes(), img.getAttribute("name"));
			type = "image";

			domImage::domInit_from *init_from = img.getInit_from();

			if(init_from != NULL)
			{
				std::string factory;
				std::string img_path = init_from->getValue().str().substr(5);
				std::string img_type = img_path.substr(img_path.length()-4);
				//k3d::log() << debug << img_path << std::endl;
				//k3d::log() << debug << img_type << std::endl;
				if(img_type==".jpg")
					factory = "JPEGBitmapReader";
				else
				if(img_type==".png")
					factory = "PNGBitmapReader";
				else
				if(img_type==".tif"||img_type=="tiff")
					factory = "TIFFBitmapReader";
				else
				{
					k3d::log() << error << "Image type required by " << img.getAttribute("name") << " not supported by K-3D yet!" << std::endl;
					return;
				}

				k3d::inode *image = k3d::plugin::create<k3d::inode>(*k3d::plugin::factory::lookup(factory),Document,name);
				k3d::property::set_internal_value(*image, "file", k3d::filesystem::generic_path(img_path));

			}
		}

		std::string get_name(){return name;}
		std::string get_type(){return type;}
		std::string get_id(){return id;}
		k3d::iproperty *get_mesh_source_output(){return mesh_source_output;}
		k3d::iproperty *get_top(){return top;}
		k3d::iproperty *get_bottom(){return bottom;}
		k3d::iproperty *get_left(){return left;}
		k3d::iproperty *get_right(){return right;}
		k3d::iproperty *get_near(){return near;}
		k3d::iproperty *get_far(){return far;}
		k3d::inode *get_light_shader(){return light_shader;}
	private:
		std::string name;
		std::string id;
		std::string type;
		k3d::iproperty *mesh_source_output;
		k3d::iproperty *matrix_source_output;
		k3d::iproperty *top, *bottom, *left, *right, *near, *far;
		k3d::inode *light_shader;
	};

}
}
}

#endif

