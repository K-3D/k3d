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

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>

#include <iterator>
#include <set>

namespace module
{

namespace blobby
{

namespace detail
{

/// std::pair equivalent that maintains the order of its members
template<typename T1, typename T2>
class ordered_pair;

template<typename T1, typename T2>
bool operator<(const ordered_pair<T1,T2>& lhs, const ordered_pair<T1,T2>& rhs);

template<typename T1, typename T2>
class ordered_pair :
	public std::pair<T1, T2>
{
public:
	typedef T1 first_type;
	typedef T2 second_type;

	T1 first;
	T2 second;

	explicit ordered_pair()
	{
	}

	explicit ordered_pair(const T1& First, const T2& Second) :
		first(First < Second ? First : Second),
		second(First < Second ? Second : First)
	{
	}

	explicit ordered_pair(const k3d::legacy::split_edge* Edge) :
		first(Edge->vertex < Edge->face_clockwise->vertex ? Edge->vertex : Edge->face_clockwise->vertex),
		second(Edge->vertex < Edge->face_clockwise->vertex ? Edge->face_clockwise->vertex : Edge->vertex)
	{
	}

	friend bool operator< <>(const ordered_pair& lhs, const ordered_pair& rhs);
};

template<typename T1, typename T2>
bool operator<(const ordered_pair<T1,T2>& lhs, const ordered_pair<T1,T2>& rhs)
{
	if(lhs.first != rhs.first)
		return lhs.first < rhs.first;

	return lhs.second < rhs.second;
}

typedef ordered_pair<k3d::legacy::point*, k3d::legacy::point*> ordered_edge_t;
typedef std::set<ordered_edge_t> ordered_edges_t;

struct get_edges
{
	get_edges(const k3d::legacy::mesh& Input, k3d::legacy::mesh& Output, ordered_edges_t& Edges) :
		edges(Edges)
	{
		for(k3d::legacy::mesh::points_t::const_iterator point = Input.points.begin(); point != Input.points.end(); ++point)
		{
			k3d::legacy::point* const new_point = new k3d::legacy::point(**point);
			Output.points.push_back(new_point);
			point_map[*point] = new_point;
		}
	}

	void operator()(k3d::legacy::split_edge& Edge)
	{
		k3d::legacy::point* p1 = Edge.vertex;
		assert_warning(Edge.face_clockwise);
		k3d::legacy::point* p2 = Edge.face_clockwise->vertex;
		assert_warning(p1 && p2);
		edges.insert(ordered_edge_t(point_map[p1], point_map[p2]));
	}

	ordered_edges_t& edges;
	std::map<k3d::legacy::point*, k3d::legacy::point*> point_map;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// edges_to_blobby_implementation

class edges_to_blobby_implementation :
	public k3d::legacy::mesh_modifier<k3d::node >
{
	typedef k3d::legacy::mesh_modifier<k3d::node > base;

public:
	edges_to_blobby_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_radius(init_owner(*this) + init_name("radius") + init_label(_("Radius")) + init_description(_("Segments radius")) + init_value(1.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_type(init_owner(*this) + init_name("type") + init_label(_("Operator type")) + init_description(_("Addition, multiplication, minimum or maximum")) + init_enumeration(operation_values()) + init_value(MAX))
	{
		m_radius.changed_signal().connect(make_reset_mesh_slot());
		m_type.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		// Collect edges ...
		detail::ordered_edges_t edges;
		k3d::legacy::for_each_edge(const_cast<k3d::legacy::mesh&>(InputMesh), detail::get_edges(InputMesh, Mesh, edges));

		const double radius = m_radius.pipeline_value();
		k3d::legacy::blobby::variable_operands* new_blobby = 0;
		switch(m_type.pipeline_value())
		{
			case ADD:
				new_blobby = new k3d::legacy::blobby::add();
				break;
			case MULT:
				new_blobby = new k3d::legacy::blobby::multiply();
				break;
			case MIN:
				new_blobby = new k3d::legacy::blobby::min();
				break;
			default:
				new_blobby = new k3d::legacy::blobby::max();
				break;
		}

		for(detail::ordered_edges_t::const_iterator edge = edges.begin(); edge != edges.end(); ++edge)
			new_blobby->add_operand(new k3d::legacy::blobby::segment(edge->first, edge->second, radius, k3d::identity3D()));

		Mesh.blobbies.push_back(new k3d::legacy::blobby(new_blobby));
	}

	void on_update_mesh(const k3d::legacy::mesh& Input, k3d::legacy::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<edges_to_blobby_implementation,
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

	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_radius;
	k3d_data(operation_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
};

/////////////////////////////////////////////////////////////////////////////
// edges_to_blobby_factory

k3d::iplugin_factory& edges_to_blobby_factory()
{
	return edges_to_blobby_implementation::get_factory();
}

} // namespace blobby

} // namespace module

