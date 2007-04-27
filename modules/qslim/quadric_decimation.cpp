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
		\brief Decimation tool using QSlim's quadric error metrics
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property.h>
#include <k3dsdk/utility.h>

#include "MxQSlim.h"

namespace libk3dqslim
{

namespace detail
{

MxStdModel* triangulate_mesh(const k3d::legacy::mesh& Mesh)
{
	// Output polyhedra as triangles
	k3d::legacy::polyhedron::faces_t new_faces;
	k3d::legacy::mesh::points_t new_points;

	for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		k3d::legacy::triangulate((*polyhedron)->faces, new_faces, new_points);

	k3d::legacy::mesh::points_t all_points;
	all_points.insert(all_points.end(), Mesh.points.begin(), Mesh.points.end());
	all_points.insert(all_points.end(), new_points.begin(), new_points.end());

	// New Mx-mesh
	MxStdModel* model = new MxStdModel(100, 100);
	return_val_if_fail(model, 0);

	std::map<k3d::legacy::point*, unsigned long> point_map;
	for(k3d::legacy::mesh::points_t::const_iterator point = all_points.begin(); point != all_points.end(); ++point)
	{
		point_map.insert(std::make_pair(*point, point_map.size()));
		const k3d::point3 position = (*point)->position;
		Vec3 pos(-position[0], position[1], position[2]);
		model->add_vertex(pos);
	}

	for(k3d::legacy::polyhedron::faces_t::const_iterator face = new_faces.begin(); face != new_faces.end(); ++face)
	{
		k3d::legacy::split_edge* const e0 = (*face)->first_edge;
		k3d::legacy::split_edge* const e1 = e0 ? e0->face_clockwise : 0;
		k3d::legacy::split_edge* const e2 = e1 ? e1->face_clockwise : 0;

		model->add_face(point_map[e2->vertex], point_map[e1->vertex], point_map[e0->vertex]);
	}

	std::for_each(new_faces.begin(), new_faces.end(), k3d::delete_object());
	std::for_each(new_points.begin(), new_points.end(), k3d::delete_object());

	return model;
}

}

/////////////////////////////////////////////////////////////////////////////
// quadric_decimation

class quadric_decimation :
	public k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	quadric_decimation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_face_number(init_owner(*this) + init_name("face_number") + init_label(_("Face number")) + init_description(_("Target face number")) + init_value(100) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_contraction_type(init_owner(*this) + init_name("contraction_type") + init_label(_("Contraction type")) + init_description(_("Stop function")) + init_value(EDGE) + init_enumeration(contraction_values())),
		m_placement_policy(init_owner(*this) + init_name("placement_policy") + init_label(_("Placement policy")) + init_description(_("Placement policy (optimal, line, endpoint or midpoint, endpoints)")) + init_value(OPTIMAL) + init_enumeration(placement_values())),
		m_quadric_weighting(init_owner(*this) + init_name("quadric_weighting") + init_label(_("Quadric weighting")) + init_description(_("Quadric weighting policy (uniform, area, angle)")) + init_value(AREA) + init_enumeration(quadric_weighting_values())),
		m_boundary_weight(init_owner(*this) + init_name("boundary_weight") + init_label(_("Boundary weight")) + init_description(_("Use boundary preservation planes with given weight")) + init_value(1000.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_compactness_ratio(init_owner(*this) + init_name("compactness_ratio") + init_label(_("Compactness ratio")) + init_description(_("Compactness ratio")) + init_value(k3d::radians(0.0)) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::angle))),
		m_meshing_penalty(init_owner(*this) + init_name("meshing_penalty") + init_label(_("Meshing penalty")) + init_description(_("Penalty for bad meshes")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_face_number.changed_signal().connect(make_reset_mesh_slot());
		m_contraction_type.changed_signal().connect(make_reset_mesh_slot());
		m_placement_policy.changed_signal().connect(make_reset_mesh_slot());
		m_quadric_weighting.changed_signal().connect(make_reset_mesh_slot());
		m_boundary_weight.changed_signal().connect(make_reset_mesh_slot());
		m_compactness_ratio.changed_signal().connect(make_reset_mesh_slot());
		m_meshing_penalty.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_create_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		if(!InputMesh.polyhedra.size() || !InputMesh.polyhedra.front()->faces.size())
			return;

		// Load mesh into MxStdModel
		MxStdModel* m = detail::triangulate_mesh(InputMesh);
		return_if_fail(m);

		const unsigned long face_number = m->face_count();
		k3d::set_value(m_face_number, face_number);

		// Init
		MxQSlim* slim = 0;
		MxEdgeQSlim* eslim = 0;
		MxFaceQSlim* fslim = 0;
		std::vector<MxEdge> target_edges;

		const bool will_use_fslim = m_contraction_type.value() == FACE;
		if(!slim)
		{
			if(will_use_fslim)
				slim = fslim = new MxFaceQSlim(*m);
			else
				slim = eslim = new MxEdgeQSlim(*m);
		}
		else
		{
			if(will_use_fslim)
				fslim = (MxFaceQSlim*)slim;
			else
				eslim = (MxEdgeQSlim*)slim;
		}

		switch(m_placement_policy.value())
		{
			case LINE: slim->placement_policy = 2; break;
			case ENDORMID: slim->placement_policy = 1; break;
			case ENDPOINTS: slim->placement_policy = 0; break;
			default: slim->placement_policy = 3;
		}
		slim->boundary_weight = m_boundary_weight.value();
		switch(m_quadric_weighting.value())
		{
			case UNIFORM: slim->weighting_policy = 0; break;
			case ANGLE: slim->weighting_policy = 2; break;
			default: slim->weighting_policy = 1;
		}
		slim->compactness_ratio = m_compactness_ratio.value();
		slim->meshing_penalty = m_meshing_penalty.value();
		slim->will_join_only = false;

		if(eslim && target_edges.size())
			eslim->initialize(target_edges, target_edges.size());
		else
			slim->initialize();

		// Decimation
		slim->decimate(m_face_number.value());

		// Output cleanup ...

		// First, mark stray vertices for removal
		for(unsigned long i = 0; i < m->vert_count(); ++i)
			if(m->vertex_is_valid(i) && m->neighbors(i).size() == 0)
				m->vertex_mark_invalid(i);

		// Compact vertex array so only valid vertices remain
		m->compact_vertices();

		std::map<unsigned long, k3d::legacy::point*> point_map;
		for(unsigned int v = 0; v < m->vert_count(); v++)
		{
			k3d::point3 position(m->vertex(v));
			position[0] = -position[0];

			k3d::legacy::point* point = new k3d::legacy::point(position);
			Mesh.points.push_back(point);

			point_map[v] = point;
		}

		k3d::legacy::polyhedron* polyhedron = new k3d::legacy::polyhedron();
		Mesh.polyhedra.push_back(polyhedron);
		k3d::imaterial* material = InputMesh.polyhedra.front()->faces.front()->material;
		for(unsigned int f = 0; f < m->face_count(); f++)
		{
			if(m->face_is_valid(f))
			{
				unsigned long i1 = m->face(f).v[0];
				unsigned long i2 = m->face(f).v[1];
				unsigned long i3 = m->face(f).v[2];

				// Create a triangle ...
				k3d::legacy::split_edge* edge1 = new k3d::legacy::split_edge(point_map[i1]);
				k3d::legacy::split_edge* edge2 = new k3d::legacy::split_edge(point_map[i2]);
				k3d::legacy::split_edge* edge3 = new k3d::legacy::split_edge(point_map[i3]);

				// Invert face (because of coordinate system switch)
				edge1->face_clockwise = edge3;
				edge3->face_clockwise = edge2;
				edge2->face_clockwise = edge1;

				k3d::legacy::face* const face = new k3d::legacy::face(edge1, material);
				polyhedron->faces.push_back(face);
			}
		}

		// Set edge companions
		k3d::legacy::set_companions(*polyhedron);
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<quadric_decimation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x9bfe8fce, 0x3c7c4b00, 0x9e050d1e, 0x062059fa),
				"QuadricDecimation",
				"Surface simplification using quadric error metrics",
				"Polygons",
				k3d::iplugin_factory::STABLE);

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

	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_face_number;
	k3d_data(contraction_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_contraction_type;
	k3d_data(placement_policy_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_placement_policy;
	k3d_data(quadric_weighting_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_quadric_weighting;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_boundary_weight;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_compactness_ratio;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_meshing_penalty;
};

/////////////////////////////////////////////////////////////////////////////
// quadric_decimation_factory

k3d::iplugin_factory& quadric_decimation_factory()
{
	return quadric_decimation::get_factory();
}

} // namespace libk3dqslim

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dqslim::quadric_decimation::get_factory());
K3D_MODULE_END

