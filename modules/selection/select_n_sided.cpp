// K-3D
// Copyright (c) 2005, Romain Behar
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

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/selection.h>

#include <map>
#include <set>

namespace module
{

namespace selection
{

namespace detail
{

/// Count face's edge number
const unsigned long edge_number(const k3d::legacy::face& Face)
{
	unsigned long edges = 0;

	k3d::legacy::split_edge* current_edge = Face.first_edge;
	do
	{
		++edges;
		current_edge = current_edge->face_clockwise;
	}
	while(current_edge != Face.first_edge);

	return edges;
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// select_n_sided

class select_n_sided :
	public k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::legacy::mesh_modifier<k3d::node > > base;

public:
	select_n_sided(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_sides(init_owner(*this) + init_name("sides") + init_label(_("Side number")) + init_description(_("Gives chosen side number")) + init_value(5L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
		m_operator(init_owner(*this) + init_name("operator") + init_label(_("Operator")) + init_description(_("Selection operator comparing with side number")) + init_value(EQUAL) + init_enumeration(operator_values())),
		m_component(init_owner(*this) + init_name("component") + init_label(_("Component type")) + init_description(_("Components to be selected (faces or edges)")) + init_value(FACES) + init_enumeration(component_values()))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());

		m_sides.changed_signal().connect(make_update_mesh_slot());
		m_operator.changed_signal().connect(make_update_mesh_slot());
		m_component.changed_signal().connect(make_update_mesh_slot());
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Mesh);
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		const unsigned long sides = m_sides.pipeline_value();
		const operator_t op = m_operator.pipeline_value();
		const component_t component = m_component.pipeline_value();

		k3d::legacy::for_each_component(Mesh, k3d::selection::set_weight(0.0));

		if(component == FACES)
		{
			for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
			{
				for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
				{
					const unsigned long edge_number = detail::edge_number(**face);
					if(op == LESS_OR_EQUAL && edge_number <= sides
						|| op == EQUAL && edge_number == sides
						|| op == GREATER_OR_EQUAL && edge_number >= sides)
					{
						(*face)->selection_weight = 1.0;
					}
				}
			}
		}
		else if(component == POINTS)
		{
			// Create a mapping of points to their array index
			typedef std::map<k3d::legacy::point*, unsigned long> point_map_t;
			point_map_t point_map;
			unsigned long index = 0;
			for(k3d::legacy::mesh::points_t::const_iterator point = Mesh.points.begin(); point != Mesh.points.end(); ++point)
				point_map[*point] = index++;

			// Build vertex valency array
			typedef std::vector<unsigned long> valencies_t;
			valencies_t valencies(point_map.size(), 0);

			for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
			{
				for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
				{
					k3d::legacy::split_edge* current_edge = (*face)->first_edge;
					do
					{
						k3d::legacy::point* vertex = current_edge->vertex;
						const unsigned long vertex_index = point_map[vertex];
						++valencies[vertex_index];
						// Edge without companion means two single edges that make one more
						if(!current_edge->companion)
							++valencies[vertex_index];

						current_edge = current_edge->face_clockwise;
					}
					while(current_edge != (*face)->first_edge);
				}
			}

			// Select points
			for(unsigned long n = 0; n < valencies.size(); ++ n)
			{
				const unsigned long edge_number = valencies[n];
				if(op == LESS_OR_EQUAL && edge_number <= sides
					|| op == EQUAL && edge_number == sides
					|| op == GREATER_OR_EQUAL && edge_number >= sides)
				{
					Mesh.points[n]->selection_weight = 1.0;
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_n_sided,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x129039f9, 0x7a6d49fc, 0x92c14587, 0x764a8cc3),
				"SelectNSided",
				_("Selects points or faces that have numbers of sides within a specified range"),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef enum
	{
		LESS_OR_EQUAL,
		EQUAL,
		GREATER_OR_EQUAL
	} operator_t;

	friend std::ostream& operator << (std::ostream& Stream, const operator_t& Value)
	{
		switch(Value)
		{
			case LESS_OR_EQUAL:
				Stream << "less_or_equal";
				break;
			case EQUAL:
				Stream << "equal";
				break;
			case GREATER_OR_EQUAL:
				Stream << "greater_or_equal";
				break;
		}
		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, operator_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "less_or_equal")
			Value = LESS_OR_EQUAL;
		else if(text == "equal")
			Value = EQUAL;
		else if(text == "greater_or_equal")
			Value = GREATER_OR_EQUAL;
		else
			k3d::log() << error << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	static const k3d::ienumeration_property::enumeration_values_t& operator_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Less_or_equal", "less_or_equal", "Selects components with N or less than N sides"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Equal", "equal", "Selects components with N sides "));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Greater_or_equal", "greater_or_equal", "Selects components with N or greater than N sides "));
		}

		return values;
	}

	typedef enum
	{
		FACES,
		POINTS
	} component_t;

	friend std::ostream& operator << (std::ostream& Stream, const component_t& Value)
	{
		switch(Value)
		{
			case FACES:
				Stream << "faces";
				break;
			case POINTS:
				Stream << "points";
				break;
		}
		return Stream;
	}

	friend std::istream& operator >> (std::istream& Stream, component_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "faces")
			Value = FACES;
		else if(text == "points")
			Value = POINTS;
		else
			k3d::log() << error << k3d_file_reference << ": unknown enumeration [" << text << "]" << std::endl;

		return Stream;
	}

	const k3d::ienumeration_property::enumeration_values_t& component_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Faces", "faces", "Selects faces"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Points", "points", "Selects points"));
		}

		return values;
	}

	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_sides;
	k3d_data(operator_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_operator;
	k3d_data(component_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_component;
};

/////////////////////////////////////////////////////////////////////////////
// select_n_sided_factory

k3d::iplugin_factory& select_n_sided_factory()
{
	return select_n_sided::get_factory();
}

} // namespace selection

} // namespace module

