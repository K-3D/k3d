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
	\author Carlos Andres Dominguez Caballero (carlos@gmail.com)
*/


#include <libxml/parser.h>
#include <libxml/tree.h>

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gprim_factory.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include "gprim_factory.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/data.h>
#include <k3dsdk/point3.h>

namespace module
{

namespace svg
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_reader_implementation

class mesh_reader_implementation :
	public k3d::mesh_source<k3d::persistent<k3d::node> >,
	public k3d::imesh_storage
{
	typedef k3d::mesh_source<k3d::persistent<k3d::node> > base;

public:
	mesh_reader_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("svg_files"))
	{
		m_file.changed_signal().connect(make_topology_changed_slot());
		count = 0;
	}

	void reset_mesh(k3d::mesh* const Mesh)
	{
		m_output_mesh.reset(Mesh);
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		const k3d::filesystem::path path = m_file.pipeline_value();
		if(path.empty())
			return;

		k3d::log() << info << "Loading .svg file: " << path.native_console_string() << std::endl;
		k3d::filesystem::ifstream file(path);
		if(!file)
		{
			k3d::log() << error << k3d_file_reference << ": error opening [" << path.native_console_string() << "]" << std::endl;
			return;
		}

		LIBXML_TEST_VERSION
		xmlDocPtr doc = NULL; 
		xmlNodePtr root_element = NULL;
		xmlNodePtr current = NULL;

		doc = xmlParseFile("/home/karma/drawing.svg");
		if (doc == NULL) {
		    k3d::log() << error << "Failed to parse" << std::endl;
		return;
		}
		else
			k3d::log() << debug << "SVG file parsed succesfully" << std::endl;

		factory = new gprim_factory(Mesh);

		root_element = xmlDocGetRootElement(doc);


		current = xmlDocGetRootElement(doc);
		current = current->xmlChildrenNode;
		while(current!=NULL)
		{
			if(!xmlStrcmp(current->name, (const xmlChar *)"g"))
				parse_graphics(doc, current);
			current = current->next;
		}

		xmlFreeDoc(doc);

		xmlCleanupParser();

		xmlMemoryDump();

	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_reader_implementation,
                k3d::interface_list<k3d::imesh_source,
                k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0xa0149217, 0xf64844bb, 0x4ba01e8f, 0x21d7f651),
			"SVGMeshReader",
			_("Reader that loads external Scalable Vector Graphics (.svg) files into the document by reference"),
			"MeshReader");

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
	gprim_factory *factory;
	int count;


	void parse_graphics(xmlDocPtr doc, xmlNodePtr current)
	{
		current = current->xmlChildrenNode;
		while(current!=NULL)
		{
			if(!xmlStrcmp(current->name, (const xmlChar *)"path"))
				parse_path(doc, current);
			if(!xmlStrcmp(current->name, (const xmlChar *)"rect"))
				parse_rect(doc, current);
			current = current->next;
		}
	}

	void parse_rect(xmlDocPtr doc, xmlNodePtr current)
	{
		xmlChar *xx, *yy, *width, *height;
		double x, y, w, h;
		k3d::mesh::indices_t rect;

		xx = xmlGetProp(current, (const xmlChar *)"x");
		yy = xmlGetProp(current, (const xmlChar *)"y");
		width = xmlGetProp(current, (const xmlChar *)"width");
		height = xmlGetProp(current, (const xmlChar *)"height");
		
		x = atof((const char *)xx);
		y = atof((const char *)yy);
		w = atof((const char *)width);
		h = atof((const char *)height);

		rect.push_back(factory->add_point(k3d::point4(x,y,0,1))-1);
		rect.push_back(factory->add_point(k3d::point4(x+w,y,0,1))-1);
		rect.push_back(factory->add_point(k3d::point4(x+w,y+h,0,1))-1);
		rect.push_back(factory->add_point(k3d::point4(x,y+h,0,1))-1);
		rect.push_back(rect.front());
		
		factory->add_curve(rect,2);
		//factory->add_polygon(rect);		
	
	}

	void parse_path(xmlDocPtr doc, xmlNodePtr current)
	{
		xmlChar *id;
		xmlChar *curve;
		k3d::mesh::indices_t path;
		k3d::mesh::indices_t path2;
		k3d::mesh::indices_t path3;

		id = xmlGetProp(current, (const xmlChar *)"id");
		curve = xmlGetProp(current, (const xmlChar *)"d");

		for(int i=0; i<4; i++)
			path.push_back(i);

		//xmlFree(id);
		//xmlFree(curve);
	}

};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader_implementation::get_factory();
}

} // namespace io

} // namespace svg

} // namespace module

