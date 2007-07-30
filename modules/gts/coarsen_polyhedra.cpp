// K-3D
// Copyright (c) 2004-2005, Romain Behar
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
		\brief Creates a coarsened version of the input
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/utility.h>

#include "gts_interface.h"

namespace libk3dgts
{

static gdouble cost_angle(GtsEdge* e)
{
	if(e->triangles && e->triangles->next)
	{
		GtsTriangle* t1 = static_cast<GtsTriangle*>(e->triangles->data);
		GtsTriangle* t2 = static_cast<GtsTriangle*>(e->triangles->next->data);
		return std::fabs(gts_triangles_angle(t1, t2));
	}

	return G_MAXDOUBLE;
}

static GtsVolumeOptimizedParams volume_params = { 0.5, 0.5, 0. };

/////////////////////////////////////////////////////////////////////////////
// coarsen_polyhedra

class coarsen_polyhedra :
	public k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	coarsen_polyhedra(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_cost_function(init_owner(*this) + init_name("cost_function") + init_label(_("Cost function")) + init_description(_("Cost function (length, optimized, angle)")) + init_value(LENGTH) + init_enumeration(cost_values())),
		m_midvertex_function(init_owner(*this) + init_name("midvertex_function") + init_label(_("Mid-vertex function")) + init_description(_("Mid-vertex  or volume optimized")) + init_value(VOLUMEOPTIMIZED) + init_enumeration(midvertex_values())),
		m_stop_function(init_owner(*this) + init_name("stop_function") + init_label(_("Stop function")) + init_description(_("Stop function (edge number or cost)")) + init_value(EDGENUMBER) + init_enumeration(stop_values())),
		m_max_fold_angle(init_owner(*this) + init_name("max_fold_angle") + init_label(_("Max fold angle")) + init_description(_("Maximum fold angle")) + init_value(k3d::radians(1.0)) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::angle))),
		m_edge_number(init_owner(*this) + init_name("edge_number") + init_label(_("Edge number")) + init_description(_("Final edge number")) + init_value(100) + init_constraint(constraint::minimum(1L)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_max_edge_cost(init_owner(*this) + init_name("max_edge_cost") + init_label(_("Max edge cost")) + init_description(_("Maximum edge cost")) + init_value(1.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::scalar))),
		m_volume_weight(init_owner(*this) + init_name("volume_weight") + init_label(_("Volume weight")) + init_description(_("Weight used for volume optimization")) + init_value(0.5) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_boundary_weight(init_owner(*this) + init_name("boundary_weight") + init_label(_("Boundary weight")) + init_description(_("Weight used for boundary optimization")) + init_value(0.5) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_shape_weight(init_owner(*this) + init_name("shape_weight") + init_label(_("Shape weight")) + init_description(_("Weight used for shape optimization")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_cost_function.changed_signal().connect(make_reset_mesh_slot());
		m_midvertex_function.changed_signal().connect(make_reset_mesh_slot());
		m_stop_function.changed_signal().connect(make_reset_mesh_slot());
		m_max_fold_angle.changed_signal().connect(make_reset_mesh_slot());
		m_edge_number.changed_signal().connect(make_reset_mesh_slot());
		m_max_edge_cost.changed_signal().connect(make_reset_mesh_slot());
		m_volume_weight.changed_signal().connect(make_reset_mesh_slot());
		m_boundary_weight.changed_signal().connect(make_reset_mesh_slot());
		m_shape_weight.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		/** \todo process each polyhedron (+ commit_mesh_changes) */

		const double fold = k3d::radians(m_max_fold_angle.pipeline_value());

		GtsSurface* surface = gts_surface(InputMesh);

		// Select the right coarsening process
		GtsKeyFunc cost_func = 0;
		gpointer cost_data = 0;
		switch(m_cost_function.pipeline_value())
		{
			case OPTIMIZED:
				cost_func = (GtsKeyFunc) gts_volume_optimized_cost;
				cost_data = &volume_params;

				volume_params.volume_weight = static_cast<gdouble>(m_volume_weight.pipeline_value());
				volume_params.boundary_weight = static_cast<gdouble>(m_boundary_weight.pipeline_value());
				volume_params.shape_weight = static_cast<gdouble>(m_shape_weight.pipeline_value());
				break;
			case LENGTH:
				cost_func = 0;
				break;
			case ANGLE:
				cost_func = (GtsKeyFunc) cost_angle;
				break;
		}

		GtsCoarsenFunc coarsen_func = 0;
		gpointer coarsen_data = 0;
		GtsStopFunc stop_func = 0;
		gpointer stop_data = 0;
		switch(m_midvertex_function.pipeline_value())
		{
			case MIDVERTEX:
				coarsen_func = 0;
				break;
			case VOLUMEOPTIMIZED:
				coarsen_func = (GtsCoarsenFunc) gts_volume_optimized_vertex;
				coarsen_data = &volume_params;
				volume_params.volume_weight = static_cast<gdouble>(m_volume_weight.pipeline_value());
				volume_params.boundary_weight = static_cast<gdouble>(m_boundary_weight.pipeline_value());
				volume_params.shape_weight = static_cast<gdouble>(m_shape_weight.pipeline_value());
				break;
		}

		guint edge_number = static_cast<guint>(m_edge_number.pipeline_value());
		gdouble max_edge_cost = static_cast<gdouble>(m_max_edge_cost.pipeline_value());
		switch(m_stop_function.pipeline_value())
		{
			case EDGENUMBER:
				stop_func = (GtsStopFunc) gts_coarsen_stop_number;
				stop_data = &edge_number;
				break;
			case COST:
				stop_func = (GtsStopFunc) gts_coarsen_stop_cost;
				stop_data = &max_edge_cost;
				break;
		}

		gts_surface_coarsen(surface, cost_func, cost_data, coarsen_func, coarsen_data, stop_func, stop_data, fold);

		copy_surface(surface, Mesh);
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<coarsen_polyhedra,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xdaae61bd, 0xd5b94f9b, 0x90a54f79, 0xf3f78729),
				"CoarsenPolyhedra",
				_("Coarsens polygonal surfaces"),
				"Polygons",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	/// Enumerates cost function types
	typedef enum
	{
		OPTIMIZED,
		LENGTH,
		ANGLE
	} cost_t;

	friend std::ostream& operator << (std::ostream& Stream, const cost_t& Value)
	{
		switch(Value)
			{
				case OPTIMIZED:
					Stream << "optimized";
					break;
				case LENGTH:
					Stream << "length";
					break;
				case ANGLE:
					Stream << "angle";
					break;
			}

		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, cost_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "optimized")
			Value = OPTIMIZED;
		else if(text == "length")
			Value = LENGTH;
		else if(text == "angle")
			Value = ANGLE;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& cost_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Optimized", "optimized", "Use optimized point cost"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Length", "length", "Use length^2 as cost function"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Angle", "angle", "Use angle as cost function"));
		}

		return values;
	}

	/// Enumerates mid-vertex functions
	typedef enum
	{
		MIDVERTEX,
		VOLUMEOPTIMIZED
	} midvertex_t;

	friend std::ostream& operator << (std::ostream& Stream, const midvertex_t& Value)
	{
		switch(Value)
			{
				case MIDVERTEX:
					Stream << "midvertex";
				break;
				case VOLUMEOPTIMIZED:
					Stream << "volumeoptimized";
				break;
			}

		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, midvertex_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "midvertex")
			Value = MIDVERTEX;
		else if(text == "volumeoptimized")
			Value = VOLUMEOPTIMIZED;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& midvertex_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Mid-vertex", "midvertex", "Use mid-vertex as replacement vertex"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Volume optimized", "volumeoptimized", "Use volume optimized point"));
		}

		return values;
	}

	/// Enumerates stop functions
	typedef enum
	{
		EDGENUMBER,
		COST
	} stop_t;

	friend std::ostream& operator << (std::ostream& Stream, const stop_t& Value)
	{
		switch(Value)
			{
				case EDGENUMBER:
					Stream << "number";
				break;
				case COST:
					Stream << "cost";
				break;
			}

		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, stop_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "number")
			Value = EDGENUMBER;
		else if(text == "cost")
			Value = COST;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& stop_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Number", "number", "Stop he coarsening process if the number of edges was to fall below"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Cost", "cost", "Stop the coarsening process if the cost of collapsing an edge is larger"));
		}

		return values;
	}

	k3d_data(cost_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_cost_function;
	k3d_data(midvertex_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_midvertex_function;
	k3d_data(stop_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_stop_function;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_max_fold_angle;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_edge_number;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_max_edge_cost;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_volume_weight;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_boundary_weight;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_shape_weight;
};

/////////////////////////////////////////////////////////////////////////////
// coarsen_polyhedra_factory

k3d::iplugin_factory& coarsen_polyhedra_factory()
{
	return coarsen_polyhedra::get_factory();
}

} // namespace libk3dgts

