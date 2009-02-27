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
#include <k3dsdk/blobby.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>

#include <boost/scoped_ptr.hpp>

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
		boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(Input));
		if(!polyhedron)
			return;

		const k3d::double_t radius = m_radius.pipeline_value();
		const operation_t type = m_type.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		// Build a list of edges, eliminating duplicates (neighbors) as we go ...
		const k3d::mesh::points_t& points = *Input.points;

		typedef std::set<std::pair<k3d::uint_t, k3d::uint_t> > edges_t;
		edges_t edges;

		const k3d::uint_t edge_begin = 0;
		const k3d::uint_t edge_end = edge_begin + polyhedron->edge_points.size();
		for(k3d::uint_t edge = edge_begin; edge != edge_end; ++edge)
		{
			edges.insert(std::make_pair(
				std::min(polyhedron->edge_points[edge], polyhedron->edge_points[polyhedron->clockwise_edges[edge]]),
				std::max(polyhedron->edge_points[edge], polyhedron->edge_points[polyhedron->clockwise_edges[edge]])));
		}

		// Setup arrays to build a new blobby ...
		boost::scoped_ptr<k3d::blobby::primitive> blobby(k3d::blobby::create(Output));

		blobby->first_primitives.push_back(blobby->primitives.size());
		blobby->primitive_counts.push_back(edges.size());
		blobby->first_operators.push_back(blobby->operators.size());
		blobby->operator_counts.push_back(1);
		blobby->materials.push_back(material);

		// Add a blobby segment for each edge ...
		for(edges_t::const_iterator edge = edges.begin(); edge != edges.end(); ++edge)
		{
			blobby->primitives.push_back(k3d::blobby::SEGMENT);
			blobby->primitive_first_floats.push_back(blobby->floats.size());
			blobby->primitive_float_counts.push_back(23);

			blobby->floats.push_back(points[edge->first][0]);
			blobby->floats.push_back(points[edge->first][1]);
			blobby->floats.push_back(points[edge->first][2]);
			blobby->floats.push_back(points[edge->second][0]);
			blobby->floats.push_back(points[edge->second][1]);
			blobby->floats.push_back(points[edge->second][2]);
			blobby->floats.push_back(radius);

			k3d::matrix4 matrix = k3d::transpose(k3d::identity3());
			blobby->floats.insert(blobby->floats.end(), static_cast<double*>(matrix), static_cast<double*>(matrix) + 16);
		}

		// Merge the edges together ...
		switch(type)
		{
			case ADD:
				blobby->operators.push_back(k3d::blobby::ADD);
				break;
			case MULT:
				blobby->operators.push_back(k3d::blobby::MULTIPLY);
				break;
			case MIN:
				blobby->operators.push_back(k3d::blobby::MINIMUM);
				break;
			case MAX:
				blobby->operators.push_back(k3d::blobby::MAXIMUM);
				break;
		}
		blobby->operator_first_operands.push_back(blobby->operands.size());
		blobby->operator_operand_counts.push_back(edges.size() + 1);
		blobby->operands.push_back(edges.size());
		for(k3d::uint_t i = 0; i != edges.size(); ++i)
			blobby->operands.push_back(i);
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

	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
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

