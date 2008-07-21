#include "collada.h"

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
	
	std::string removeSpaces(std::string str)
	{
		for(int i=0; i<str.length(); i++)
			if(str[i]==' ')
				str[i]='_';
		return str;
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
		geom->setAttribute("id", removeSpaces(name).c_str());
		geom->setAttribute("name", removeSpaces(name).c_str());
		SafeAdd(geom, "mesh", gmesh);
	
		// Add the position data
	
		domFloat posArray[mesh->points->size()*3];
		for(int i=0; i<mesh->points->size(); i++)
			for(int j=0; j<3;j++)
				posArray[i*3+j] = mesh->points->at(i)[j];
	
		int count = sizeof(posArray)/sizeof(posArray[0]);
	
		addSource(gmesh, removeSpaces(name) + "-positions", "X Y Z", posArray, count);
	
		// Add the <vertices> element
		SafeAdd(gmesh, "vertices", vertices);
		vertices->setAttribute("id", (removeSpaces(name) + "-vertices").c_str());
		SafeAdd(vertices, "input", verticesInput);
		verticesInput->setAttribute("semantic", "POSITION");
		verticesInput->setAttribute("source", makeUriRef(removeSpaces(name) + "-positions").c_str());
	
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
					const size_t loop_begin = face_first_loops[face];
					const size_t loop_end = loop_begin + face_loop_counts[face];
					for(size_t loop = loop_begin; loop != loop_end; ++loop)
					{
						const size_t first_edge = loop_first_edges[loop];
						size_t face_size = 0;
						for(size_t edge = first_edge; ; )
						{
							ind.push_back(edge_points[edge]);
	
							edge = clockwise_edges[edge];
							face_size++;
							if(edge == first_edge)
							{
								vcou.push_back(face_size);
								break;
							}
						}
	
						/** \todo Support faces with holes */
						break;
					}
				}
	
				domPolylist* polylist = daeSafeCast<domPolylist>(gmesh->add("polylist"));
				polylist->setMaterial("mtl");
				polylist->setCount(face_counts[polyhedron]);
				domPolylist::domVcount *vcount = daeSafeCast<domPolylist::domVcount>(polylist->add("vcount"));
				
				addInput(polylist, "VERTEX",   removeSpaces(name) + "-vertices", 0);
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
		}
	}
	
	void addMeshInstance(daeElement * visualScene, k3d::inode *inode) 
	{
		// Add a <node> with a simple transformation
		SafeAdd(visualScene, "node", node);
		node->setAttribute("id", removeSpaces(inode->name()).c_str());
		node->setAttribute("name", removeSpaces(inode->name()).c_str());
	
		// Instantiate the <geometry>
		SafeAdd(node, "instance_geometry", instanceGeom);
	
		std::string ID = k3d::data::property_lookup(k3d::property::get(*inode, "input_mesh"))->property_node()->name();
	
		instanceGeom->setAttribute("url", makeUriRef(removeSpaces(ID)).c_str());
	
		// Bind material parameters
		//SafeAdd(instanceGeom, "bind_material technique_common instance_material", instanceMaterial);
		//instanceMaterial->setAttribute("symbol", "mtl");
		//instanceMaterial->setAttribute("target", makeUriRef("cubeMaterial").c_str());
	
		//SafeAdd(instanceMaterial, "bind_vertex_input", bindVertexInput);
		//bindVertexInput->setAttribute("semantic", "uv0");
		//bindVertexInput->setAttribute("input_semantic", "TEXCOORD");
		//bindVertexInput->setAttribute("input_set", "0");

		k3d::matrix4 matrix = boost::any_cast<k3d::matrix4>(k3d::property::get(*inode, "input_matrix")->property_pipeline_value());

		std::stringstream mat;
		for(int i=0; i<4; i++)
			for(int j=0; j<4; j++)
				mat << matrix[i][j] << " " << std::endl;
		node->add("matrix")->setCharData(mat.str());
	}

	void addCameraInstance(daeElement * visualScene, k3d::inode *inode)
	{
		// Add a <node> with a simple transformation
		SafeAdd(visualScene, "node", node);
		node->setAttribute("id", removeSpaces(inode->name()).c_str());
		node->setAttribute("name", removeSpaces(inode->name()).c_str());
	
		// Instantiate the <geometry>
		SafeAdd(node, "instance_camera", instanceCam);

		instanceCam->setAttribute("url", makeUriRef(removeSpaces(inode->name())).c_str());

		k3d::matrix4 matrix = boost::any_cast<k3d::matrix4>(k3d::property::get(*inode, "input_matrix")->property_pipeline_value());

		std::stringstream mat;
		for(int i=0; i<4; i++)
			for(int j=0; j<4; j++)
				mat << matrix[i][j] << " " << std::endl;
		node->add("matrix")->setCharData(mat.str());
	}

	k3d::mesh to_k3d_mesh(domGeometry &geom)
	{
		intGeometry result(geom,k3d::identity3D());
		return result.getMesh();
	}

}
}
}
