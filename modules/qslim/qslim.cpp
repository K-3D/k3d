// K-3D
// Copyright (c) 2004-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "MxQSlim.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/triangulator.h>
#include <k3dsdk/utility.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace qslim
{

/////////////////////////////////////////////////////////////////////////////
// qslim

class qslim :
	public k3d::material_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	qslim(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_face_count(init_owner(*this) + init_name("face_count") + init_label(_("Face Count")) + init_description(_("Desired number of outputs faces.")) + init_value(1000) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_contraction_type(init_owner(*this) + init_name("contraction_type") + init_label(_("Contraction type")) + init_description(_("Stop function")) + init_value(EDGE) + init_enumeration(contraction_values())),
		m_placement_policy(init_owner(*this) + init_name("placement_policy") + init_label(_("Placement policy")) + init_description(_("Placement policy (optimal, line, endpoint or midpoint, endpoints)")) + init_value(OPTIMAL) + init_enumeration(placement_values())),
		m_quadric_weighting(init_owner(*this) + init_name("quadric_weighting") + init_label(_("Quadric weighting")) + init_description(_("Quadric weighting policy (uniform, area, angle)")) + init_value(AREA) + init_enumeration(quadric_weighting_values())),
		m_boundary_weight(init_owner(*this) + init_name("boundary_weight") + init_label(_("Boundary weight")) + init_description(_("Use boundary preservation planes with given weight")) + init_value(1000.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_compactness_ratio(init_owner(*this) + init_name("compactness_ratio") + init_label(_("Compactness ratio")) + init_description(_("Compactness ratio")) + init_value(k3d::radians(0.0)) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::angle))),
		m_meshing_penalty(init_owner(*this) + init_name("meshing_penalty") + init_label(_("Meshing penalty")) + init_description(_("Penalty for bad meshes")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_face_count.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_contraction_type.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_placement_policy.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_quadric_weighting.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_boundary_weight.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_compactness_ratio.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
		m_meshing_penalty.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::mesh_topology_changed> >(make_reset_mesh_slot()));
	}

	class qslim_convert :
		public k3d::triangulator
	{
	public:
		qslim_convert(MxStdModel& Model) :
			model(Model)
		{
		}

	private:
		void add_vertex(const k3d::point3& Coordinates, k3d::uint_t Vertices[4], k3d::uint_t Edges[4], k3d::double_t Weights[4], k3d::uint_t NewVertex)
		{
			NewVertex = model.vert_count();
			model.add_vertex(-Coordinates[0], Coordinates[1], Coordinates[2]);
		}

		void add_triangle(k3d::uint_t Vertices[3], k3d::uint_t Edges[3])
		{
			model.add_face(Vertices[0], Vertices[1], Vertices[2]);
		}

		MxStdModel& model;
	};

	/// Converts a K-3D polyhedron to a QSlim model
	static MxStdModel* convert(const k3d::mesh& Mesh, const k3d::polyhedron::const_primitive& Primitive)
	{
		// Create a new QSlim model ...
		MxStdModel* const model = new MxStdModel(100, 100);

		// Add mesh points to the model ...
		return_val_if_fail(Mesh.points, model);
		const k3d::mesh::points_t& points = *Mesh.points;

		const k3d::uint_t point_begin = 0;
		const k3d::uint_t point_end = point_begin + points.size();
		for(k3d::uint_t point = point_begin; point != point_end; ++point)
			model->add_vertex(-points[point][0], points[point][1], points[point][2]);

		// Convert polygons to triangles ...
		qslim_convert(*model).process(Mesh, Primitive);
		
		return model;
	}

	/// Appends a QSlim model to a K-3D mesh as a polyhedron primitive.
	static void append(const k3d::polyhedron::const_primitive& InputPolyhedron, MxStdModel& Model, k3d::mesh& Mesh, k3d::imaterial* const Material)
	{
		k3d::mesh::points_t& points = Mesh.points.writable();
		k3d::mesh::selection_t& point_selection = Mesh.point_selection.writable();
		const k3d::uint_t point_offset = points.size();
		for(unsigned int v = 0; v < Model.vert_count(); ++v)
		{
		       k3d::point3 position(Model.vertex(v));
		       position[0] = -position[0];
		       points.push_back(position);
		}

		boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Mesh));
		polyhedron->constant_attributes = InputPolyhedron.constant_attributes;

		for(unsigned int f = 0; f != Model.face_count(); ++f)
		{
			if(!Model.face_is_valid(f))
				continue;

			polyhedron->face_shells.push_back(0);
			polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
			polyhedron->face_loop_counts.push_back(1);
			polyhedron->face_selections.push_back(0);
			polyhedron->face_materials.push_back(Material);

			polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

			polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
			polyhedron->edge_selections.push_back(0);
			polyhedron->vertex_points.push_back(point_offset + Model.face(f).v[0]);
			polyhedron->vertex_selections.push_back(0);

			polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
			polyhedron->edge_selections.push_back(0);
			polyhedron->vertex_points.push_back(point_offset + Model.face(f).v[1]);
			polyhedron->vertex_selections.push_back(0);

			polyhedron->clockwise_edges.push_back(polyhedron->loop_first_edges.back());
			polyhedron->edge_selections.push_back(0);
			polyhedron->vertex_points.push_back(point_offset + Model.face(f).v[2]);
			polyhedron->vertex_selections.push_back(0);
		}

		polyhedron->shell_types.push_back(k3d::polyhedron::POLYGONS);
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output.points = Input.points;
		Output.point_selection = Input.point_selection;

		k3d::imaterial* const material = m_material.pipeline_value();

		// For each primitive ...
		for(k3d::mesh::primitives_t::const_iterator primitive = Input.primitives.begin(); primitive != Input.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Input, **primitive));
			if(!polyhedron)
			{
				Output.primitives.push_back(*primitive);
				continue;
			}

			// Convert polyhedron to a QSlim model ... 
			boost::scoped_ptr<MxStdModel> model(convert(Output, *polyhedron));

			const k3d::int32_t face_number = std::max(0, std::min(static_cast<k3d::int32_t>(model->face_count()), m_face_count.pipeline_value()));
			
			boost::scoped_ptr<MxQSlim> slim;
			switch(m_contraction_type.pipeline_value())
			{
				case EDGE:
					slim.reset(new MxEdgeQSlim(*model));
					break;
				case FACE:
					slim.reset(new MxFaceQSlim(*model));
					break;
			}

			switch(m_placement_policy.pipeline_value())
			{
				case ENDPOINTS:
					slim->placement_policy = 0;
					break;
				case ENDORMID:
					slim->placement_policy = 1;
					break;
				case LINE:
					slim->placement_policy = 2;
					break;
				case OPTIMAL:
					slim->placement_policy = 3;
					break;
			}

			slim->boundary_weight = m_boundary_weight.pipeline_value();

			switch(m_quadric_weighting.pipeline_value())
			{
				case UNIFORM:
					slim->weighting_policy = 0;
					break;
				case AREA:
					slim->weighting_policy = 1;
					break;
				case ANGLE:
					slim->weighting_policy = 2;
					break;
			}

			slim->compactness_ratio = m_compactness_ratio.pipeline_value();
			slim->meshing_penalty = m_meshing_penalty.pipeline_value();
			slim->will_join_only = false;

			// Make it happen ...
			slim->initialize();
			slim->decimate(m_face_count.pipeline_value());

			append(*polyhedron, *model, Output, material);
		}

		k3d::mesh::bools_t unused_points;
		k3d::mesh::lookup_unused_points(Output, unused_points);
		k3d::mesh::delete_points(Output, unused_points);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<qslim,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9bfe8fce, 0x3c7c4b00, 0x9e050d1e, 0x062059fa),
				"QSlim",
				"Surface simplification using quadric error metrics",
				"Polyhedron",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	/// Enumerates contraction functions
	typedef enum
	{
		EDGE,
		FACE
	} contraction_t;

	friend std::ostream& operator << (std::ostream& Stream, const contraction_t& Value)
	{
		switch(Value)
		{
			case EDGE:
				Stream << "edge";
			break;
			case FACE:
				Stream << "face";
			break;
		}

		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, contraction_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "edge")
			Value = EDGE;
		else if(text == "face")
			Value = FACE;
		else
			k3d::log() << error << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& contraction_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Edge", "edge", "Use edge contraction"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Face", "face", "Use face contraction"));
		}

		return values;
	}

	/// Enumerates placement policies
	typedef enum
	{
		OPTIMAL,
		LINE,
		ENDORMID,
		ENDPOINTS
	} placement_policy_t;

	friend std::ostream& operator << (std::ostream& Stream, const placement_policy_t& Value)
	{
		switch(Value)
		{
			case OPTIMAL:
				Stream << "optimal";
				break;
			case LINE:
				Stream << "line";
				break;
			case ENDORMID:
				Stream << "endormid";
				break;
			case ENDPOINTS:
				Stream << "endpoints";
				break;
		}

		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, placement_policy_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "optimal")
			Value = OPTIMAL;
		else if(text == "line")
			Value = LINE;
		else if(text == "endormid")
			Value = ENDORMID;
		else if(text == "endpoints")
			Value = ENDPOINTS;
		else
			k3d::log() << error << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	const k3d::ienumeration_property::enumeration_values_t& placement_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Optimal", "optimal", "Use optimal placement policy"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Line", "line", "Use line placement policy"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Endormid", "endormid", "Use end-point or mid-point placement policy"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Endpoints", "endpoints", "Use end-points placement policy"));
		}

		return values;
	}

	/// Enumerates quadric weighting policies
	typedef enum
	{
		UNIFORM,
		AREA,
		ANGLE
	} quadric_weighting_t;

	friend std::ostream& operator << (std::ostream& Stream, const quadric_weighting_t& Value)
	{
		switch(Value)
		{
			case UNIFORM:
				Stream << "uniform";
				break;
			case AREA:
				Stream << "area";
				break;
			case ANGLE:
				Stream << "angle";
				break;
		}

		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, quadric_weighting_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "uniform")
			Value = UNIFORM;
		else if(text == "area")
			Value = AREA;
		else if(text == "angle")
			Value = ANGLE;
		else
			k3d::log() << error << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	const k3d::ienumeration_property::enumeration_values_t& quadric_weighting_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Uniform", "uniform", "Use uniform quadric weighting"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Area", "area", "Use area quadric weighting"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Angle", "angle", "Use angle quadric weighting"));
		}

		return values;
	}

	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_face_count;
	k3d_data(contraction_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_contraction_type;
	k3d_data(placement_policy_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_placement_policy;
	k3d_data(quadric_weighting_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_quadric_weighting;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_boundary_weight;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_compactness_ratio;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_meshing_penalty;
};

/////////////////////////////////////////////////////////////////////////////
// qslim_factory

k3d::iplugin_factory& qslim_factory()
{
	return qslim::get_factory();
}

} // namespace qslim

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qslim::qslim_factory());
K3D_MODULE_END

