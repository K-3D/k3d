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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/mesh_selection_modifier.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// select_n_sided

class select_n_sided :
	public k3d::mesh_selection_modifier<k3d::mesh_selection_sink<k3d::node > >
{
	typedef k3d::mesh_selection_modifier<k3d::mesh_selection_sink<k3d::node > > base;

public:
	select_n_sided(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_sides(init_owner(*this) + init_name("sides") + init_label(_("Side number")) + init_description(_("Gives chosen side number")) + init_value(5L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
		m_operation(init_owner(*this) + init_name("operator") + init_label(_("Operator")) + init_description(_("Selection operator comparing with side number")) + init_value(EQUAL) + init_enumeration(operator_values()))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));
		m_sides.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));
		m_operation.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::selection_changed> >(make_update_mesh_slot()));
	}

	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		const k3d::int32_t sides = m_sides.pipeline_value();
		const operation_t operation = m_operation.pipeline_value();

		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output);

		for(k3d::mesh::primitives_t::iterator primitive = Output.primitives.begin(); primitive != Output.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, *primitive));
			if(!polyhedron)
				continue;

			k3d::mesh::counts_t counts;
			k3d::polyhedron::create_edge_count_lookup(polyhedron->loop_first_edges, polyhedron->clockwise_edges, counts);

			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = face_begin + polyhedron->face_first_loops.size();

			switch(operation)
			{
				case LESS_OR_EQUAL:
				{
					for(k3d::uint_t face = face_begin; face != face_end; ++face)
					{
						if(counts[polyhedron->face_first_loops[face]] <= sides)
							polyhedron->face_selections[face] = 1;
					}
					break;
				}
				case EQUAL:
				{
					for(k3d::uint_t face = face_begin; face != face_end; ++face)
					{
						if(counts[polyhedron->face_first_loops[face]] == sides)
							polyhedron->face_selections[face] = 1;
					}
					break;
				}
				case GREATER_OR_EQUAL:
				{
					for(k3d::uint_t face = face_begin; face != face_end; ++face)
					{
						if(counts[polyhedron->face_first_loops[face]] >= sides)
							polyhedron->face_selections[face] = 1;
					}
					break;
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
	} operation_t;

	friend std::ostream& operator<<(std::ostream& Stream, const operation_t& Value)
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

	friend std::istream& operator>>(std::istream& Stream, operation_t& Value)
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
			values.push_back(k3d::ienumeration_property::enumeration_value_t("<= Less-than-or-equal", "less_or_equal", "Selects components with N or less than N sides"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("= Equal", "equal", "Selects components with N sides "));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(">= Greater-than-or-equal", "greater_or_equal", "Selects components with N or greater than N sides "));
		}

		return values;
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
	k3d_data(operation_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_operation;
};

/////////////////////////////////////////////////////////////////////////////
// select_n_sided_factory

k3d::iplugin_factory& select_n_sided_factory()
{
	return select_n_sided::get_factory();
}

} // namespace selection

} // namespace module

