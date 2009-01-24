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
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/triangulator.h>
#include <k3dsdk/user_property_changed_signal.h>

namespace module
{

namespace cgal
{

namespace detail
{
// taken from the triangulate_faces plugin. TODO: make this available in the SDK?
class create_triangles :
	public k3d::triangulator
{
	typedef k3d::triangulator base;

public:
	create_triangles() :
		m_input(0)
	{
	}

	void process(const k3d::mesh& Input, const k3d::mesh_selection& Selection, k3d::mesh& Output)
	{
		if(!k3d::validate_polyhedra(Input))
			return;

		m_input = &Input;

		// Allocate new data structures for our output ...
		m_polyhedra = &Output.polyhedra.create();
		m_first_faces = &m_polyhedra->first_faces.create();
		m_face_counts = &m_polyhedra->face_counts.create();
		m_types = &m_polyhedra->types.create();
		m_face_first_loops = &m_polyhedra->face_first_loops.create();
		m_face_loop_counts = &m_polyhedra->face_loop_counts.create();
		m_face_selection = &m_polyhedra->face_selection.create();
		m_face_materials = &m_polyhedra->face_materials.create();
		m_loop_first_edges = &m_polyhedra->loop_first_edges.create();
		m_edge_points = &m_polyhedra->edge_points.create();
		m_clockwise_edges = &m_polyhedra->clockwise_edges.create();
		m_edge_selection = &m_polyhedra->edge_selection.create();
		m_points = &Output.points.create(new k3d::mesh::points_t(*Input.points));
		m_point_selection = &Output.point_selection.create(new k3d::mesh::selection_t(Input.points->size(), 0.0));

		// Setup copying of attribute arrays ...
		m_polyhedra->constant_data = m_input->polyhedra->constant_data;

		m_polyhedra->uniform_data = m_input->polyhedra->uniform_data.clone_types();
		m_uniform_data_copier.reset(new k3d::attribute_array_copier(m_input->polyhedra->uniform_data, m_polyhedra->uniform_data));

		m_polyhedra->face_varying_data = m_input->polyhedra->face_varying_data.clone_types();
		m_face_varying_data_copier.reset(new k3d::attribute_array_copier(m_input->polyhedra->face_varying_data, m_polyhedra->face_varying_data));

		Output.vertex_data = m_input->vertex_data.clone();
		m_vertex_data_copier.reset(new k3d::attribute_array_copier(m_input->vertex_data, Output.vertex_data));

		// Setup the output mesh ...
		k3d::mesh::selection_t input_face_selection(*m_input->polyhedra->face_selection);
		k3d::mesh_selection::merge(Selection.faces, input_face_selection);

		const k3d::uint_t face_begin = 0;
		const k3d::uint_t face_end = face_begin + m_input->polyhedra->face_first_loops->size();
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
		{
			if(input_face_selection[face])
			{
				base::process(
					*m_input->points,
					*m_input->polyhedra->face_first_loops,
					*m_input->polyhedra->face_loop_counts,
					*m_input->polyhedra->loop_first_edges,
					*m_input->polyhedra->edge_points,
					*m_input->polyhedra->clockwise_edges,
					face);
			}
			else
			{
				add_existing_face(face);
			}
		}

		m_first_faces->push_back(0);
		m_face_counts->push_back(m_face_first_loops->size());
		m_types->push_back(k3d::mesh::polyhedra_t::POLYGONS);
	}

private:
	void start_face(const k3d::uint_t Face)
	{
		m_current_face = Face;
	}

	void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], double Weights[4], k3d::uint_t& NewVertex)
	{
		NewVertex = m_points->size();

		m_points->push_back(Coordinates);
		m_point_selection->push_back(0.0);

		m_vertex_data_copier->push_back(4, Vertices, Weights);

		m_new_face_varying_data[NewVertex] = new_face_varying_record(Edges, Weights);
	}

	void add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
	{
		m_face_first_loops->push_back(m_loop_first_edges->size());
		m_face_loop_counts->push_back(1);
		m_face_selection->push_back(1.0);
		m_face_materials->push_back((*m_input->polyhedra->face_materials)[m_current_face]);

		m_uniform_data_copier->push_back(m_current_face);

		m_loop_first_edges->push_back(m_edge_points->size());
		m_edge_points->push_back(Vertices[0]);
		m_edge_points->push_back(Vertices[1]);
		m_edge_points->push_back(Vertices[2]);
		m_clockwise_edges->push_back(m_edge_points->size() - 2);
		m_clockwise_edges->push_back(m_edge_points->size() - 1);
		m_clockwise_edges->push_back(m_edge_points->size() - 3);
		m_edge_selection->push_back(0.0);
		m_edge_selection->push_back(0.0);
		m_edge_selection->push_back(0.0);

		for(k3d::uint_t i = 0; i != 3; ++i)
		{
			if(m_new_face_varying_data.count(Vertices[i]))
				m_face_varying_data_copier->push_back(4, m_new_face_varying_data[Vertices[i]].edges, m_new_face_varying_data[Vertices[i]].weights);
			else
				m_face_varying_data_copier->push_back(Edges[i]);
		}
	}

	void add_existing_face(const k3d::uint_t Face)
	{
		const k3d::mesh::indices_t& input_face_first_loops = *m_input->polyhedra->face_first_loops;
		const k3d::mesh::indices_t& input_face_loop_counts = *m_input->polyhedra->face_loop_counts;
		const k3d::mesh::materials_t& input_face_materials = *m_input->polyhedra->face_materials;

		const k3d::mesh::indices_t& input_loop_first_edges = *m_input->polyhedra->loop_first_edges;
		
		const k3d::mesh::indices_t& input_edge_points = *m_input->polyhedra->edge_points;
		const k3d::mesh::indices_t& input_clockwise_edges = *m_input->polyhedra->clockwise_edges;

		m_face_first_loops->push_back(m_loop_first_edges->size());
		m_face_loop_counts->push_back(input_face_loop_counts[Face]);
		m_face_selection->push_back(0.0);
		m_face_materials->push_back(input_face_materials[Face]);

		m_uniform_data_copier->push_back(Face);

		const k3d::uint_t loop_begin = input_face_first_loops[Face];
		const k3d::uint_t loop_end = loop_begin + input_face_loop_counts[Face];
		for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
		{
			m_loop_first_edges->push_back(m_edge_points->size());

			const k3d::uint_t first_edge = input_loop_first_edges[loop];
			const k3d::uint_t edge_offset = m_edge_points->size() - first_edge;
			for(k3d::uint_t edge = first_edge; ;)
			{
				m_edge_points->push_back(input_edge_points[edge]);
				m_clockwise_edges->push_back(input_clockwise_edges[edge] + edge_offset);
				m_edge_selection->push_back(0.0);
				m_face_varying_data_copier->push_back(edge);

				edge = input_clockwise_edges[edge];
				if(edge == first_edge)
					break;
			}
		}
	}

	const k3d::mesh* m_input;

	k3d::mesh::polyhedra_t* m_polyhedra;
	k3d::mesh::indices_t* m_first_faces;
	k3d::mesh::counts_t* m_face_counts;
	k3d::mesh::polyhedra_t::types_t* m_types;
	k3d::mesh::indices_t* m_face_first_loops;
	k3d::mesh::counts_t* m_face_loop_counts;
	k3d::mesh::selection_t* m_face_selection;
	k3d::mesh::materials_t* m_face_materials;
	k3d::mesh::indices_t* m_loop_first_edges;
	k3d::mesh::indices_t* m_edge_points;
	k3d::mesh::indices_t* m_clockwise_edges;
	k3d::mesh::selection_t* m_edge_selection;
	k3d::mesh::points_t* m_points;
	k3d::mesh::selection_t* m_point_selection;

	boost::shared_ptr<k3d::attribute_array_copier> m_uniform_data_copier;
	boost::shared_ptr<k3d::attribute_array_copier> m_face_varying_data_copier;
	boost::shared_ptr<k3d::attribute_array_copier> m_vertex_data_copier;

	k3d::uint_t m_current_face;

	struct new_face_varying_record
	{
		new_face_varying_record()
		{
		}

		new_face_varying_record(k3d::uint_t Edges[4], k3d::double_t Weights[4])
		{
			std::copy(Edges, Edges + 4, edges);
			std::copy(Weights, Weights + 4, weights);
		}

		k3d::uint_t edges[4];
		k3d::double_t weights[4];
	};

	std::map<k3d::uint_t, new_face_varying_record> m_new_face_varying_data;
};
	
void merge_coplanar_faces(k3d::mesh& Mesh, const double_t Threshold)
{
	const k3d::mesh::points_t& points = *Mesh.points;
	const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
	const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
	const k3d::uint_t face_begin = 0;
	const k3d::uint_t face_end = face_first_loops.size();
	k3d::mesh::selection_t input_face_selection(face_end, 1.0);
	
	k3d::mesh::bools_t boundary_edges;
	k3d::mesh::indices_t companions;
	k3d::polyhedron::create_edge_adjacency_lookup(edge_points, clockwise_edges, boundary_edges, companions);
	
	// Calculate the face normals
	k3d::mesh::normals_t face_normals(face_first_loops.size());
	for(k3d::uint_t face = face_begin; face != face_end; ++face)
	{
		face_normals[face] = k3d::normalize(k3d::normal(edge_points, clockwise_edges, points, loop_first_edges[face_first_loops[face]]));
	}
	
	k3d::mesh::indices_t edge_faces;
	k3d::polyhedron::create_edge_face_lookup(face_first_loops, *Mesh.polyhedra->face_loop_counts, loop_first_edges, clockwise_edges, edge_faces);
	k3d::mesh::selection_t redundant_edges;
	k3d::polyhedron::mark_coplanar_edges(companions, boundary_edges, face_normals, edge_faces, input_face_selection, redundant_edges, Threshold);

	k3d::mesh::polyhedra_t& output_polyhedra = Mesh.polyhedra.writable();
	k3d::euler::kill_edge_make_loop(output_polyhedra, points, redundant_edges, boundary_edges, companions, face_normals);
}

void merge_collinear_edges(k3d::mesh& Mesh, const double_t Threshold)
{
	const k3d::mesh::points_t& points = *Mesh.points;
	const k3d::mesh::indices_t& edge_points = *Mesh.polyhedra->edge_points;
	const k3d::mesh::indices_t& clockwise_edges = *Mesh.polyhedra->clockwise_edges;
	const k3d::mesh::indices_t& loop_first_edges = *Mesh.polyhedra->loop_first_edges;
	const k3d::mesh::indices_t& face_first_loops = *Mesh.polyhedra->face_first_loops;
	const k3d::uint_t face_begin = 0;
	const k3d::uint_t face_end = face_first_loops.size();
	k3d::mesh::selection_t input_edge_selection(edge_points.size(), 1.0);
	
	k3d::mesh::bools_t boundary_edges;
	k3d::mesh::indices_t companions;
	k3d::polyhedron::create_edge_adjacency_lookup(edge_points, clockwise_edges, boundary_edges, companions);
	
	k3d::mesh::counts_t vertex_valences;
	k3d::polyhedron::create_vertex_valence_lookup(points.size(), edge_points, vertex_valences);
	k3d::mesh::selection_t redundant_edges;
	k3d::polyhedron::mark_collinear_edges(redundant_edges, input_edge_selection, points, edge_points, clockwise_edges, vertex_valences, boundary_edges, companions, Threshold);
	
	k3d::euler::kill_edge_and_vertex(Mesh.polyhedra.writable(), redundant_edges, boundary_edges, companions, points.size());
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
			do_boolean<exact_nef>(Output);
			detail::merge_coplanar_faces(Output, m_threshold.pipeline_value());
			detail::merge_collinear_edges(Output, m_threshold.pipeline_value());
			k3d::mesh::delete_unused_points(Output);
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
		boolean_functor(const boolean::boolean_t BooleanType, const k3d::int32_t Digits, nef_t& Result) : m_boolean_type(BooleanType), m_result(Result), m_started(false)
		{
			m_factor = 10.0;
			for(k3d::uint_t i = 0; i != Digits; ++i)
				m_factor *= 10.0;
		}
		void operator()(k3d::iproperty* const Property)
		{
			if(Property->property_type() == typeid(k3d::mesh*))
			{
				k3d::timer timer;
				const k3d::mesh* const mesh = boost::any_cast<k3d::mesh*>(k3d::property::pipeline_value(*Property));
				return_if_fail(mesh);
				return_if_fail(k3d::validate_polyhedra(*mesh));
				return_if_fail(k3d::is_solid(*mesh));
				
				// First triangulate inputs
				k3d::mesh triangulated_mesh;
				detail::create_triangles().process(*mesh, k3d::mesh_selection::select_all(), triangulated_mesh);
				k3d::log() << debug << "triangulation: " << timer.elapsed() << std::endl;
				
				timer.restart();
				boost::shared_ptr<nef_t> operand = to_nef<nef_t>(triangulated_mesh, m_factor);
				k3d::log() << debug << "convert to nef: " << timer.elapsed() << std::endl;
				return_if_fail(operand.get());
				if (!m_started)
				{
					m_result = *operand;
					m_started = true;
				}
				else
				{
					timer.restart();
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
					k3d::log() << debug << "boolean op: " << timer.elapsed() << std::endl;
				}
			}
		}
	private:
		const boolean::boolean_t m_boolean_type;
		k3d::double_t m_factor;
		nef_t& m_result;
		k3d::bool_t m_started;
	};
	
	/// Executes a boolean operation, using the given Nef polyhedron type
	template<typename nef_t>
	void do_boolean(k3d::mesh& Output)
	{
		Output = k3d::mesh();
		nef_t result;
		const boolean_t boolean_type = m_type.pipeline_value(); 
		
		const k3d::iproperty_collection::properties_t properties = k3d::property::user_properties(*static_cast<k3d::iproperty_collection*>(this));
		boolean_functor<nef_t> functor(boolean_type, 16, result);
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

