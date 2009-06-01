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
// with the CGAL (http://www.cgal.org) library and distribute executables, as long as you
// follow the requirements of the GNU GPL in regard to all of the
// software in the executable aside from CGAL.

/** \file
		\brief Plugin to perform 3D boolean operations using CGAL
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "conversion.h"

#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/euler_operations.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_selection.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/triangulator.h>
#include <k3dsdk/user_property_changed_signal.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace cgal
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
	k3d::polyhedron::create_edge_adjacency_lookup(Polyhedron.edge_points, Polyhedron.clockwise_edges, boundary_edges, companions);
	
	// Calculate the face normals
	k3d::mesh::normals_t face_normals(Polyhedron.face_first_loops.size());
	for(k3d::uint_t face = face_begin; face != face_end; ++face)
	{
		face_normals[face] = k3d::normalize(k3d::polyhedron::normal(Polyhedron.edge_points, Polyhedron.clockwise_edges, Points, Polyhedron.loop_first_edges[Polyhedron.face_first_loops[face]]));
	}
	
	k3d::mesh::indices_t edge_faces;
	k3d::polyhedron::create_edge_face_lookup(Polyhedron.face_first_loops, Polyhedron.face_loop_counts, Polyhedron.loop_first_edges, Polyhedron.clockwise_edges, edge_faces);
	k3d::mesh::indices_t redundant_edges;
	k3d::polyhedron::mark_coplanar_edges(companions, boundary_edges, face_normals, edge_faces, input_face_selection, redundant_edges, Threshold);

	k3d::euler::kill_edge_make_loop(Polyhedron, redundant_edges, boundary_edges, companions, Points, face_normals);
}

void merge_collinear_edges(const k3d::mesh::points_t& Points, k3d::polyhedron::primitive& Polyhedron, const k3d::double_t Threshold)
{
	const k3d::uint_t face_begin = 0;
	const k3d::uint_t face_end = Polyhedron.face_first_loops.size();
	k3d::mesh::selection_t input_edge_selection(Polyhedron.edge_points.size(), 1.0);
	
	k3d::mesh::bools_t boundary_edges;
	k3d::mesh::indices_t companions;
	k3d::polyhedron::create_edge_adjacency_lookup(Polyhedron.edge_points, Polyhedron.clockwise_edges, boundary_edges, companions);
	
	k3d::mesh::counts_t vertex_valences;
	k3d::polyhedron::create_vertex_valence_lookup(Points.size(), Polyhedron.edge_points, vertex_valences);
	k3d::mesh::indices_t redundant_edges;
	k3d::polyhedron::mark_collinear_edges(redundant_edges, input_edge_selection, Points, Polyhedron.edge_points, Polyhedron.clockwise_edges, vertex_valences, boundary_edges, companions, Threshold);

	k3d::euler::kill_edge_and_vertex(Polyhedron, redundant_edges, boundary_edges, companions, Points.size());
}
	
} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// boolean

class boolean :
	public k3d::imulti_mesh_sink,
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

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
			do_boolean<exact_nef>(Output, *this);

			boost::scoped_ptr<k3d::polyhedron::primitive> output_polyhedron;
			for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
			{
				// We only get the first polyhedron
				output_polyhedron.reset(k3d::polyhedron::validate(primitive->writable()));
				if(output_polyhedron.get())
					break;
			}
			if(Output.points && output_polyhedron.get())
			{
				document().pipeline_profiler().start_execution(*this, "Simplify output");
				detail::merge_coplanar_faces(*Output.points, *output_polyhedron, m_threshold.pipeline_value());
				detail::merge_collinear_edges(*Output.points, *output_polyhedron, m_threshold.pipeline_value());
				k3d::mesh::delete_unused_points(Output);
				document().pipeline_profiler().finish_execution(*this, "Simplify output");
			}
		}
		catch (std::exception& E)
		{
			k3d::log() << error << "CGALBoolean: error executing boolean operation: " << E.what() << std::endl;
			Output = k3d::mesh();
			return;
		}
		
	}
	
	void on_update_mesh_geometry(k3d::mesh& Output) {}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<boolean, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imulti_mesh_sink> > > factory(
			k3d::uuid(0x0d88fe28, 0xea4fdf20, 0xae858297, 0x487f857b),
			"CGALBoolean",
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
	
	template<typename nef_t>
	class boolean_functor
	{
	public:
		boolean_functor(const boolean::boolean_t BooleanType, nef_t& Result, k3d::inode& Node) : m_boolean_type(BooleanType), m_result(Result), m_started(false), m_node(Node), m_sequence(1)
		{
		}
		void operator()(k3d::iproperty* const Property)
		{
			if(Property->property_type() == typeid(k3d::mesh*))
			{
				const k3d::mesh* const input_mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(*Property));
				return_if_fail(input_mesh);
				// make a copy of the mesh, where we can alter the face selection so everything is selected
				k3d::mesh mesh_all_faces_selected(*input_mesh);
				boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron;
				for(k3d::mesh::primitives_t::iterator primitive = mesh_all_faces_selected.primitives.begin(); primitive != mesh_all_faces_selected.primitives.end(); ++primitive)
				{
					// We only get the first polyhedron
					polyhedron.reset(k3d::polyhedron::validate(primitive->writable()));
					if(polyhedron.get())
						break;
				}
				if(!polyhedron.get())
					return;
				return_if_fail(k3d::polyhedron::is_solid(*polyhedron));
				polyhedron->face_selections.assign(polyhedron->face_selections.size(), 1.0);
				
				// First triangulate inputs
				k3d::mesh triangulated_mesh;
				
				k3d::string_t boolean_op = k3d::string_cast(m_sequence - 1);
				k3d::string_t sequence_string = k3d::string_cast(m_sequence++); 
				
				m_node.document().pipeline_profiler().start_execution(m_node, "Triangulate input " + sequence_string);
				const k3d::mesh::primitive* triangulated_prim = k3d::polyhedron::triangulate(mesh_all_faces_selected, *polyhedron, triangulated_mesh);
				boost::scoped_ptr<k3d::polyhedron::const_primitive> triangulated_polyhedron(k3d::polyhedron::validate(*triangulated_prim));
				m_node.document().pipeline_profiler().finish_execution(m_node, "Triangulate input " + sequence_string);
				
				m_node.document().pipeline_profiler().start_execution(m_node, "Convert input " + sequence_string + " to Nef");
				boost::shared_ptr<nef_t> operand = to_nef<nef_t>(*triangulated_mesh.points, *triangulated_polyhedron);
				m_node.document().pipeline_profiler().finish_execution(m_node, "Convert input " + sequence_string + " to Nef");
				return_if_fail(operand.get());
				if (!m_started)
				{
					m_result = *operand;
					m_started = true;
				}
				else
				{
					m_node.document().pipeline_profiler().start_execution(m_node, "Execute boolean operation " + boolean_op);
					switch(m_boolean_type)
					{
						case BOOLEAN_INTERSECTION:
							m_result *= *operand;
							break;
			
						case BOOLEAN_UNION:
							m_result += *operand;
							break;
			
						case BOOLEAN_DIFFERENCE:
							m_result -= *operand;
							break;
							
						case BOOLEAN_REVERSE_DIFFERENCE:
							m_result -= *operand;
							break;
					}
					m_node.document().pipeline_profiler().finish_execution(m_node, "Execute boolean operation " + boolean_op);
				}
			}
		}
	private:
		const boolean::boolean_t m_boolean_type;
		nef_t& m_result;
		k3d::bool_t m_started;
		k3d::inode& m_node;
		k3d::uint_t m_sequence;
	};
	
	/// Executes a boolean operation, using the given Nef polyhedron type
	template<typename nef_t>
	void do_boolean(k3d::mesh& Output, k3d::inode& Node)
	{
		Output = k3d::mesh();
		nef_t result;
		const boolean_t boolean_type = m_type.pipeline_value();
		
		const k3d::iproperty_collection::properties_t properties = k3d::property::user_properties(*static_cast<k3d::iproperty_collection*>(this));
		boolean_functor<nef_t> functor(boolean_type, result, Node);
		if (boolean_type == BOOLEAN_REVERSE_DIFFERENCE)
		{
			std::for_each(properties.rbegin(), properties.rend(), functor);
		}
		else
		{
			std::for_each(properties.begin(), properties.end(), functor);
		}
			
		to_mesh(result, Output, static_cast<k3d::imaterial*>(0));
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

} // namespace cgal

} // namespace module

