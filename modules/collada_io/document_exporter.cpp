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
#include <dae/daeUtils.h>
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
#include <k3dsdk/plugins.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_operations.h>
#include <string>
#include <list>

#define SafeAdd(elt, name, var) \
	daeElement* var = elt->add(name);

namespace module
{
namespace collada
{
namespace io
{

// "myGeom" --> "#myGeom"
std::string makeUriRef(const std::string& id) 
{
	return std::string("#") + id;
}

template<typename T>
daeTArray<T> rawArrayToDaeArray(T rawArray[], size_t count) 
{
	daeTArray<T> result;
	for (size_t i = 0; i < count; i++)
		result.append(rawArray[i]);
	return result;
}

void addSource(daeElement* mesh, const std::string& srcID, const std::string& paramNames, domFloat values[], int valueCount)
{
	SafeAdd(mesh, "source", src);
	src->setAttribute("id", srcID.c_str());

	domFloat_array* fa = daeSafeCast<domFloat_array>(src->add("float_array"));
	fa->setId((src->getAttribute("id") + "-array").c_str());
	fa->setCount(valueCount);
	fa->getValue() = rawArrayToDaeArray(values, valueCount);

	domAccessor* acc = daeSafeCast<domAccessor>(src->add("technique_common accessor"));
	acc->setSource(makeUriRef(fa->getId()).c_str());

	std::list<std::string> params = cdom::tokenize(paramNames, " ");
	acc->setStride(params.size());
	acc->setCount(valueCount/params.size());
	for (cdom::tokenIter iter = params.begin(); iter != params.end(); iter++) 
	{
		SafeAdd(acc, "param", p);
		p->setAttribute("name", iter->c_str());
		p->setAttribute("type", "float");
	}
}

void addInput(daeElement* triangles, const std::string& semantic, const std::string& srcID, int offset) 
{
	domInputLocalOffset* input = daeSafeCast<domInputLocalOffset>(triangles->add("input"));
	input->setSemantic(semantic.c_str());
	input->setOffset(offset);
	input->setSource(makeUriRef(srcID).c_str());
	if (semantic == "TEXCOORD")
		input->setSet(0);
}

void addGeometry(daeElement* library_geometries, const std::string &name, k3d::mesh *mesh) 
{
	SafeAdd(library_geometries, "geometry", geom);
	std::string geomID = name;
	for(int i=0; i<geomID.length(); i++)
		if(geomID[i]==' ')
			geomID[i]='_';
	geom->setAttribute("id", geomID.c_str());
	geom->setAttribute("name", geomID.c_str());
	SafeAdd(geom, "mesh", gmesh);

	// Add the position data

	domFloat posArray[mesh->points->size()*3];
	for(int i=0; i<mesh->points->size(); i++)
		for(int j=0; j<3;j++)
			posArray[i*3+j] = mesh->points->at(i)[j];

	int count = sizeof(posArray)/sizeof(posArray[0]);

	addSource(gmesh, geomID + "-positions", "X Y Z", posArray, count);

	// Add the <vertices> element
	SafeAdd(gmesh, "vertices", vertices);
	vertices->setAttribute("id", (geomID + "-vertices").c_str());
	SafeAdd(vertices, "input", verticesInput);
	verticesInput->setAttribute("semantic", "POSITION");
	verticesInput->setAttribute("source", makeUriRef(geomID + "-positions").c_str());

	// Store polyhedra ...
	std::vector<domUint> ind;
	std::vector<domUint> vcou;
	if(k3d::validate_polyhedra(*mesh))
	{
		const k3d::mesh::indices_t& first_faces = *mesh->polyhedra->first_faces;
		const k3d::mesh::counts_t& face_counts = *mesh->polyhedra->face_counts;
		const k3d::mesh::polyhedra_t::types_t& types = *mesh->polyhedra->types;
		const k3d::mesh::indices_t& face_first_loops = *mesh->polyhedra->face_first_loops;
		const k3d::mesh::counts_t& face_loop_counts = *mesh->polyhedra->face_loop_counts;
		const k3d::mesh::indices_t& loop_first_edges = *mesh->polyhedra->loop_first_edges;
		const k3d::mesh::indices_t& edge_points = *mesh->polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *mesh->polyhedra->clockwise_edges;
		const size_t polyhedron_begin = 0;
		const size_t polyhedron_end = polyhedron_begin + first_faces.size();
		for(size_t polyhedron = polyhedron_begin; polyhedron != polyhedron_end; ++polyhedron)
		{
			const size_t face_begin = first_faces[polyhedron];
			const size_t face_end = face_begin + face_counts[polyhedron];
			for(size_t face = face_begin; face != face_end; ++face)
			{
				//file << "f";
				const size_t loop_begin = face_first_loops[face];
				const size_t loop_end = loop_begin + face_loop_counts[face];
				for(size_t loop = loop_begin; loop != loop_end; ++loop)
				{
					const size_t first_edge = loop_first_edges[loop];
					size_t face_size = 0;
					for(size_t edge = first_edge; ; )
					{
						//file << " " << edge_points[edge] + 1; //specs want indices starting with 1
						//k3d::log() << debug << edge_points[edge]+1 << std::endl;
						ind.push_back(edge_points[edge]+1);

						edge = clockwise_edges[edge];
						face_size++;
						if(edge == first_edge)
						{
							//k3d::log() << debug << face_size << std::endl;
							vcou.push_back(face_size);
							break;
						}
					}

					/** \todo Support faces with holes */
					break;
				}

				//file << "\n";
			}
		}
	}

	domPolylist* polylist = daeSafeCast<domPolylist>(gmesh->add("polylist"));
	polylist->setMaterial("mtl");
	domPolylist::domVcount *vcount = daeSafeCast<domPolylist::domVcount>(polylist->add("vcount"));

	addInput(polylist, "VERTEX",   geomID + "-vertices", 0);
	//addInput(polylist, "NORMAL",   geomID + "-normals",  1);
	//addInput(polylist, "TEXCOORD", geomID + "-uv",       2);

	domP* p = daeSafeCast<domP>(polylist->add("p"));
	domUint index[ind.size()];
	for(int i=0; i<ind.size(); i++)
		index[i] = ind[i];
	domUint vcount_tmp[vcou.size()];
	for(int i=0; i<vcou.size(); i++)
		vcount_tmp[i] = vcou[i];

	p->getValue() = rawArrayToDaeArray(index, ind.size());
	vcount->getValue() = rawArrayToDaeArray(vcount_tmp, vcou.size());
}

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
		daeElement *library_geometries = root->add("library_geometries");

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

			if((*node)->factory().factory_id() == k3d::plugin::factory::lookup("PolyCube")->factory_id())
			{
				k3d::mesh *mesh = boost::any_cast<k3d::mesh*>(k3d::property::get(**node, "output_mesh")->property_internal_value());
				addGeometry(library_geometries, (*node)->name(), mesh);
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
