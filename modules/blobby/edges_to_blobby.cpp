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
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>

#include <iterator>
#include <set>

namespace module
{

namespace blobby
{

/////////////////////////////////////////////////////////////////////////////
// edges_to_blobby

class edges_to_blobby :
	public k3d::material_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_modifier<k3d::node > > base;

public:
	edges_to_blobby(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Controls the radius of each blobby segment.")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_type(init_owner(*this) + init_name("type") + init_label(_("Operator")) + init_description(_("Specify the mathematical operator to merge segments: addition, multiplication, minimum or maximum.")) + init_enumeration(operation_values()) + init_value(MAX))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_type.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		if(!k3d::validate_polyhedra(Input))
			return;

		const double radius = m_radius.pipeline_value();
		const operation_t type = m_type.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		// Build a list of edges, eliminating duplicates (neighbors) as we go ...
		const k3d::mesh::points_t& points = *Input.points;
		const k3d::mesh::indices_t& edge_points = *Input.polyhedra->edge_points;
		const k3d::mesh::indices_t& clockwise_edges = *Input.polyhedra->clockwise_edges;

		typedef std::set<std::pair<k3d::uint_t, k3d::uint_t> > edges_t;
		edges_t edges;

		const k3d::uint_t edge_begin = 0;
		const k3d::uint_t edge_end = edge_begin + edge_points.size();
		for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
		{
			edges.insert(std::make_pair(
				std::min(edge_points[edge], edge_points[clockwise_edges[edge]]),
				std::max(edge_points[edge], edge_points[clockwise_edges[edge]])));
		}

		// Setup arrays to build a new blobby ...
		k3d::mesh::blobbies_t& blobbies = Output.blobbies.create();
		k3d::mesh::indices_t& first_primitives = blobbies.first_primitives.create();
		k3d::mesh::counts_t& primitive_counts = blobbies.primitive_counts.create();
		k3d::mesh::indices_t& first_operators = blobbies.first_operators.create();
		k3d::mesh::counts_t& operator_counts = blobbies.operator_counts.create();
		k3d::mesh::materials_t& materials = blobbies.materials.create();
		k3d::mesh::blobbies_t::primitives_t& primitives = blobbies.primitives.create();
		k3d::mesh::indices_t& primitive_first_floats = blobbies.primitive_first_floats.create();
		k3d::mesh::counts_t& primitive_float_counts = blobbies.primitive_float_counts.create();
		k3d::mesh::blobbies_t::operators_t& operators = blobbies.operators.create();
		k3d::mesh::indices_t& operator_first_operands = blobbies.operator_first_operands.create();
		k3d::mesh::counts_t& operator_operand_counts = blobbies.operator_operand_counts.create();
		k3d::mesh::blobbies_t::floats_t& floats = blobbies.floats.create();
		k3d::mesh::blobbies_t::operands_t& operands = blobbies.operands.create();

		first_primitives.push_back(primitives.size());
		primitive_counts.push_back(edges.size());
		first_operators.push_back(operators.size());
		operator_counts.push_back(1);
		materials.push_back(material);

		// Add a blobby segment for each edge ...
		for(edges_t::const_iterator edge = edges.begin(); edge != edges.end(); ++edge)
		{
			primitives.push_back(k3d::mesh::blobbies_t::SEGMENT);
			primitive_first_floats.push_back(floats.size());
			primitive_float_counts.push_back(23);

			floats.push_back(points[edge->first][0]);
			floats.push_back(points[edge->first][1]);
			floats.push_back(points[edge->first][2]);
			floats.push_back(points[edge->second][0]);
			floats.push_back(points[edge->second][1]);
			floats.push_back(points[edge->second][2]);
			floats.push_back(radius);

			k3d::matrix4 matrix = k3d::transpose(k3d::identity3D());
			floats.insert(floats.end(), static_cast<double*>(matrix), static_cast<double*>(matrix) + 16);
		}

		// Merge the edges together ...
		switch(type)
		{
			case ADD:
				operators.push_back(k3d::mesh::blobbies_t::ADD);
				break;
			case MULT:
				operators.push_back(k3d::mesh::blobbies_t::MULTIPLY);
				break;
			case MIN:
				operators.push_back(k3d::mesh::blobbies_t::MINIMUM);
				break;
			case MAX:
				operators.push_back(k3d::mesh::blobbies_t::MAXIMUM);
				break;
		}
		operator_first_operands.push_back(operands.size());
		operator_operand_counts.push_back(edges.size() + 1);
		operands.push_back(edges.size());
		for(k3d::uint_t i = 0; i != edges.size(); ++i)
			operands.push_back(i);
	}

	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<edges_to_blobby,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0xc6a00316, 0x72a54b1a, 0xb9ac478e, 0x00fdfc6c),
				"EdgesToBlobby",
				"Converts input edges to segment blobbies",
				"Blobby",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef enum
	{
		ADD,
		MULT,
		MIN,
		MAX
	} operation_t;

	friend std::ostream& operator << (std::ostream& Stream, const operation_t& Value)
	{
		switch(Value)
		{
			case ADD:
				Stream << "addition";
				break;
			case MULT:
				Stream << "multiplication";
				break;
			case MIN:
				Stream << "minimum";
				break;
			case MAX:
				Stream << "maximum";
				break;
		}

		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, operation_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "addition")
			Value = ADD;
		else if(text == "multiplication")
			Value = MULT;
		else if(text == "minimum")
			Value = MIN;
		else if(text == "maximum")
			Value = MAX;
		else
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& operation_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Addition", "addition", "Combine blobby segments with BlobbyAdd"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Multiplication", "multiplication", "Combine blobby segments with BlobbyMult"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Minimum", "minimum", "Combine blobby segments with BlobbyMin"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Maximum", "maximum", "Combine blobby segments with BlobbyMax"));
		}

		return values;
	}

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(operation_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
};

/////////////////////////////////////////////////////////////////////////////
// edges_to_blobby_factory

k3d::iplugin_factory& edges_to_blobby_factory()
{
	return edges_to_blobby::get_factory();
}

} // namespace blobby

} // namespace module

