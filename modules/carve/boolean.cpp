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
//
// As a special exception, you have permission to link this program
// with the CARVE (http://www.carve.org) library and distribute executables, as long as you
// follow the requirements of the GNU GPL in regard to all of the
// software in the executable aside from CARVE.

/** \file
		\brief Plugin to perform 3D boolean operations using CARVE
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/table_copier.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/euler_operations.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/property.h>
#include <k3dsdk/triangulator.h>
#include <k3dsdk/user_property_changed_signal.h>

#include <boost/mpl/vector.hpp>
#include <boost/scoped_ptr.hpp>

#include <carve/csg.hpp>
#include <carve/poly.hpp>

using namespace carve;

namespace module
{

namespace carve
{

namespace detail
{
	
void merge_coplanar_faces(const k3d::mesh::points_t& Points, k3d::polyhedron::primitive& Polyhedron, const k3d::double_t Threshold)
{
	const k3d::uint_t face_begin = 0;
	const k3d::uint_t face_end = Polyhedron.face_first_loops.size();
	k3d::mesh::selection_t input_face_selection(face_end, 1.0);
	
	k3d::mesh::bools_t boundary_edges;
	k3d::mesh::indices_t companions;
	k3d::polyhedron::create_edge_adjacency_lookup(Polyhedron.vertex_points, Polyhedron.clockwise_edges, boundary_edges, companions);
	
	// Calculate the face normals
	k3d::mesh::normals_t face_normals(Polyhedron.face_first_loops.size());
	for(k3d::uint_t face = face_begin; face != face_end; ++face)
	{
		face_normals[face] = k3d::normalize(k3d::polyhedron::normal(Polyhedron.vertex_points, Polyhedron.clockwise_edges, Points, Polyhedron.loop_first_edges[Polyhedron.face_first_loops[face]]));
	}
	
	k3d::mesh::indices_t edge_faces;
	k3d::polyhedron::create_edge_face_lookup(Polyhedron, edge_faces);
	k3d::mesh::indices_t redundant_edges;
	k3d::polyhedron::mark_coplanar_edges(companions, boundary_edges, face_normals, edge_faces, input_face_selection, redundant_edges, Threshold);

	k3d::euler::kill_edge_make_loop(Polyhedron, redundant_edges, boundary_edges, companions, Points, face_normals);
}

void merge_collinear_edges(const k3d::mesh::points_t& Points, k3d::polyhedron::primitive& Polyhedron, const k3d::double_t Threshold)
{
	const k3d::uint_t face_begin = 0;
	const k3d::uint_t face_end = Polyhedron.face_first_loops.size();
	k3d::mesh::selection_t input_edge_selection(Polyhedron.vertex_points.size(), 1.0);
	
	k3d::mesh::bools_t boundary_edges;
	k3d::mesh::indices_t companions;
	k3d::polyhedron::create_edge_adjacency_lookup(Polyhedron.vertex_points, Polyhedron.clockwise_edges, boundary_edges, companions);
	
	k3d::mesh::counts_t vertex_valences;
	k3d::polyhedron::create_point_valence_lookup(Points.size(), Polyhedron.vertex_points, vertex_valences);
	k3d::mesh::indices_t redundant_edges;
	k3d::polyhedron::mark_collinear_edges(redundant_edges, input_edge_selection, Points, Polyhedron.vertex_points, Polyhedron.clockwise_edges, vertex_valences, boundary_edges, companions, Threshold);

	k3d::euler::kill_edge_and_vertex(Polyhedron, redundant_edges, boundary_edges, companions, Points.size());
}

/// Types supported for interpolation
typedef boost::mpl::vector<k3d::color, k3d::double_t, k3d::texture3, k3d::int32_t, k3d::uint_t> interpolation_types;

/// Stores attribute values at a single index
struct table_value
{

	typedef std::map<k3d::string_t, boost::any> values_t;
	values_t values;

private:
	/// Extracts single values from a table, and puts them into the supplied map
	struct value_extractor
	{
		value_extractor(values_t& Values, const k3d::table& Table, const k3d::uint_t Index) : m_values(Values), m_table(Table), m_idx(Index) {}

		template<typename T> void operator()(T v)
		{
			typedef k3d::typed_array<T> array_t;
			for(k3d::table::const_iterator it = m_table.begin(); it != m_table.end(); ++it)
			{
				const k3d::string_t& array_name = it->first;
				if(const array_t* array = dynamic_cast<const array_t*>(it->second))
				{

				}
			}
		}

	private:
		values_t& m_values;
		const k3d::table& m_table;
		const k3d::uint_t m_idx;
	};
};
	
} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// boolean

class boolean :
	public k3d::imulti_mesh_sink,
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;
	typedef poly::Vertex<3> vertex_t;
	typedef poly::Face<3> face_t;
public:
	boolean(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_type(init_owner(*this) + init_name("type") + init_label(_("Type")) + init_description(_("Boolean operation (intersection, union, difference, reverse difference)")) + init_value(BOOLEAN_INTERSECTION) + init_enumeration(boolean_values())),
		m_threshold(init_owner(*this) + init_name("threshold") + init_label(_("Threshold")) + init_description(_("Controls the sensitivity for deciding when to simplify coplanar faces or collinear edges.")) + init_value(1e-8) + init_step_increment(1e-8) + init_units(typeid(k3d::measurement::scalar))),
		m_user_property_changed_signal(*this)	
	{
		m_type.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_threshold.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_user_property_changed_signal.connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}
	
	void on_update_mesh_topology(k3d::mesh& Output)
	{
		try
		{
			do_boolean(Output, *this);

//			boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron;
//			for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
//			{
//				// We only get the first polyhedron
//				output_polyhedron.reset(k3d::polyhedron::validate(Output, primitive->writable()));
//				if(output_polyhedron.get())
//					break;
//			}
//			if(Output.points && output_polyhedron.get())
//			{
//				document().pipeline_profiler().start_execution(*this, "Simplify output");
//				detail::merge_coplanar_faces(*Output.points, *output_polyhedron, m_threshold.pipeline_value());
//				detail::merge_collinear_edges(*Output.points, *output_polyhedron, m_threshold.pipeline_value());
//				k3d::mesh::bools_t unused_points;
//				k3d::mesh::lookup_unused_points(Output, unused_points);
//				k3d::mesh::delete_points(Output, unused_points);
//				document().pipeline_profiler().finish_execution(*this, "Simplify output");
//			}
		}
		catch (std::exception& E)
		{
			k3d::log() << error << "CARVEBoolean: error executing boolean operation: " << E.what() << std::endl;
			Output = k3d::mesh();
			return;
		}
		
	}
	
	void on_update_mesh_geometry(k3d::mesh& Output) {}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<boolean, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imulti_mesh_sink> > > factory(
			k3d::uuid(0x1f463505, 0x2c4bd104, 0xde32b98f, 0xb2da1ed1),
			"CARVEBoolean",
			_("3D mesh boolean operations"),
			"CSG",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
private:
	
	typedef enum
	{
		BOOLEAN_INTERSECTION,
		BOOLEAN_UNION,
		BOOLEAN_DIFFERENCE,
		BOOLEAN_REVERSE_DIFFERENCE
	} boolean_t;
	
	class boolean_functor
	{
	public:
		boolean_functor(const boolean::boolean_t BooleanType, k3d::inode& Node) : m_boolean_type(BooleanType), m_node(Node), m_sequence(1)
		{
		}

		void operator()(k3d::iproperty* const Property)
		{
			if(Property->property_type() == typeid(k3d::mesh*))
			{
				const k3d::mesh* const input_mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(*Property));
				if(!input_mesh)
					throw std::runtime_error("No mesh found in property " + Property->property_name());
				// make a copy of the mesh, where we can alter the face selection so everything is selected
				k3d::mesh mesh_all_faces_selected(*input_mesh);
				boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron;
				for(k3d::mesh::primitives_t::iterator primitive = mesh_all_faces_selected.primitives.begin(); primitive != mesh_all_faces_selected.primitives.end(); ++primitive)
				{
					// We only get the first polyhedron
					polyhedron.reset(k3d::polyhedron::validate(mesh_all_faces_selected, primitive->writable()));
					if(polyhedron.get())
						break;
				}
				if(!polyhedron.get())
					return;

				const k3d::uint_t input_face_count = polyhedron->face_selections.size();
				for(k3d::uint_t face = 0; face != input_face_count; ++face)
				{
					polyhedron->face_selections[face] = polyhedron->face_loop_counts[face] > 1 ? 1.0 : 0.0;
				}
				
				// We triangulate the holes
				k3d::mesh triangulated_mesh;
				
				k3d::string_t boolean_op = k3d::string_cast(m_sequence - 1);
				k3d::string_t sequence_string = k3d::string_cast(m_sequence++); 
				
				m_node.document().pipeline_profiler().start_execution(m_node, "Triangulate input " + sequence_string);
				const k3d::mesh::primitive* triangulated_prim = k3d::polyhedron::triangulate(mesh_all_faces_selected, *polyhedron, triangulated_mesh);
				boost::scoped_ptr<k3d::polyhedron::const_primitive> triangulated_polyhedron(k3d::polyhedron::validate(triangulated_mesh, *triangulated_prim));
				m_node.document().pipeline_profiler().finish_execution(m_node, "Triangulate input " + sequence_string);

				std::vector<vertex_t > vertices;
				std::vector<face_t > faces;
				const k3d::uint_t point_count = triangulated_mesh.points->size();
				vertices.reserve(point_count);
				for(k3d::uint_t p_idx = 0; p_idx != point_count; ++p_idx)
				{
					const k3d::point3& p = triangulated_mesh.points->at(p_idx);
					vertices.push_back(vertex_t(geom::VECTOR(p[0],p[1],p[2])));
				}
				const k3d::uint_t face_count = triangulated_polyhedron->face_first_loops.size();
				faces.reserve(face_count);
				for(k3d::uint_t face = 0; face != face_count; ++face)
				{
					assert_error(triangulated_polyhedron->face_loop_counts[face] == 1);
					const k3d::uint_t loop = triangulated_polyhedron->face_first_loops[face];
					const k3d::uint_t first_edge = triangulated_polyhedron->loop_first_edges[loop];
					std::vector<const vertex_t *> corners;
					for(k3d::uint_t edge = first_edge; ;)
					{
						corners.push_back(&vertices[triangulated_polyhedron->vertex_points[edge]]);

						edge = triangulated_polyhedron->clockwise_edges[edge];
						if(edge == first_edge)
							break;
					}
					faces.push_back(face_t(corners));
				}
				if (!m_result)
				{
					m_result.reset(new poly::Polyhedron(faces));
				}
				else
				{
					m_node.document().pipeline_profiler().start_execution(m_node, "Execute boolean operation " + boolean_op);
					csg::CSG::OP op = csg::CSG::UNION;
					switch(m_boolean_type)
					{
						case BOOLEAN_INTERSECTION:
							op = csg::CSG::INTERSECTION;
							break;
			
						case BOOLEAN_UNION:
							op = csg::CSG::UNION;
							break;
			
						case BOOLEAN_DIFFERENCE:
							op = csg::CSG::A_MINUS_B;
							break;
							
						case BOOLEAN_REVERSE_DIFFERENCE:
							op = csg::CSG::B_MINUS_A;
							break;
					}
					csg::CSG csg;
					boost::scoped_ptr<poly::Polyhedron> operand(new poly::Polyhedron(faces));
					m_result.reset(csg.compute(m_result.get(), operand.get(), op));
					m_node.document().pipeline_profiler().finish_execution(m_node, "Execute boolean operation " + boolean_op);
				}
			}
		}

		poly::Polyhedron& result()
		{
			return *m_result;
		}

	private:
		const boolean::boolean_t m_boolean_type;
		k3d::inode& m_node;
		k3d::uint_t m_sequence;
		boost::scoped_ptr<poly::Polyhedron> m_result;
	};
	
	/// Executes a boolean operation
	void do_boolean(k3d::mesh& Output, k3d::inode& Node)
	{
		Output = k3d::mesh();
		const boolean_t boolean_type = m_type.pipeline_value();
		
		const k3d::iproperty_collection::properties_t properties = k3d::property::user_properties(*static_cast<k3d::iproperty_collection*>(this));
		boolean_functor functor(boolean_type, Node);
		for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
			functor(*property);

		document().pipeline_profiler().start_execution(*this, "Canonicalize");
		functor.result().canonicalize();
		document().pipeline_profiler().finish_execution(*this, "Canonicalize");
			
		k3d::mesh::points_t vertices;
		k3d::mesh::counts_t vertex_counts;
		k3d::mesh::indices_t vertex_indices;

		std::map<const vertex_t*, k3d::uint_t> vertex_map;

		const k3d::uint_t vertex_count = functor.result().vertices.size();
		vertices.reserve(vertex_count);
		for(k3d::uint_t v_idx = 0; v_idx != vertex_count; ++v_idx)
		{
			const vertex_t& v = functor.result().vertices[v_idx];
			vertex_map[&v] = vertices.size();
			vertices.push_back(k3d::point3(v.v[0], v.v[1], v.v[2]));
		}

		const k3d::uint_t face_count = functor.result().faces.size();
		for(k3d::uint_t f_idx = 0; f_idx != face_count; ++f_idx)
		{
			const face_t& f = functor.result().faces[f_idx];
			const k3d::uint_t f_v_count = f.vertices.size();
			vertex_counts.push_back(f_v_count);
			for(k3d::uint_t v_idx = 0; v_idx != f_v_count; ++v_idx)
			{
				vertex_indices.push_back(vertex_map[f.vertices[v_idx]]);
			}
		}
		k3d::polyhedron::create(Output, vertices, vertex_counts, vertex_indices, 0);
	}

	static const k3d::ienumeration_property::enumeration_values_t& boolean_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Intersection"), "intersection", _("Render intersecting volumes")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Union"), "union", _("Render the union of two volumnes")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Difference"), "difference", _("Render the difference of two volumes")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Reverse Difference"), "reverse_difference", _("Render the difference of two volumes")));
		}

		return values;
	}

	friend std::ostream& operator<<(std::ostream& Stream, const boolean_t& Value)
	{
		switch(Value)
		{
			case BOOLEAN_UNION:
				Stream << "union";
				break;
			case BOOLEAN_INTERSECTION:
				Stream << "intersection";
				break;
			case BOOLEAN_DIFFERENCE:
				Stream << "difference";
				break;
			case BOOLEAN_REVERSE_DIFFERENCE:
				Stream << "reverse_difference";
				break;
		}

		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, boolean_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "union")
			Value = BOOLEAN_UNION;
		else if(text == "intersection")
			Value = BOOLEAN_INTERSECTION;
		else if(text == "difference")
			Value = BOOLEAN_DIFFERENCE;
		else if(text == "reverse_difference")
			Value = BOOLEAN_REVERSE_DIFFERENCE;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

		return Stream;
	}

	k3d_data(boolean_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_threshold;
	k3d::user_property_changed_signal m_user_property_changed_signal;
};

k3d::iplugin_factory& boolean_factory()
{
	return boolean::get_factory();
}

} // namespace carve

} // namespace module

