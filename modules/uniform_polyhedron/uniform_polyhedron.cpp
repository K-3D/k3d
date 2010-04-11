// K-3D
// Copyright (c) 2010, Timothy M. Shead
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

///////////////////////////////////////////////////////////////////////////////
//
// Polyhedron data files courtesy of http://polyhedra.org, which is based on
// the Netlib Polyhedra Database compiled by Andrew Hume at
// http://netlib.sandia.gov/polyhedra, with corrections pulished by
// George Hart at http://www.georgehart.com/virtual-polyhedra/netlib-info.html.
// See http://polyhedra.org/poly/about/ for details.
//
// The data files are licensed under the GNU Free Documentation License, see
// http://polyhedra.org/poly/about/terms for details.
//
///////////////////////////////////////////////////////////////////////////////


/** \file
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/resource/resource.h>

#include <boost/algorithm/string/find.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>

namespace module
{

namespace uniform_polyhedron
{

/////////////////////////////////////////////////////////////////////////////
// uniform_polyhedron

class uniform_polyhedron :
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	uniform_polyhedron(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_document(Document),
		m_type(init_owner(*this) + init_name("type") + init_label(_("Type")) + init_description(_("Polyhedron type.")) + init_value(k3d::string_t("tetrahedron")) + init_enumeration(polyhedron_values())),
		m_size(init_owner(*this) + init_name("size") + init_label(_("Size")) + init_description(_("Polyhedron size.")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_material.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_type.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_size.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();

		try
		{
			k3d::imaterial* const material = m_material.pipeline_value();
			const k3d::string_t type = m_type.pipeline_value();
			const k3d::double_t size = m_size.pipeline_value();

			static const k3d::mesh::colors_t colors = boost::assign::list_of
				(k3d::color(1, 1, 1))
				(k3d::color(1, 1, 1))
				(k3d::color(1, 1, 1))
				(k3d::color(1, 0, 0)) // triangles
				(k3d::color(0, 1, 0)) // quads
				(k3d::color(0, 0, 1)) // pentagons
				(k3d::color(1, 1, 0)) // hexagons
				(k3d::color(1, 1, 1))
				(k3d::color(1, 0, 1)) // octagons
				(k3d::color(1, 1, 1))
				(k3d::color(1, 1, 1))
				(k3d::color(1, 1, 1))
				(k3d::color(0, 1, 1)); // decagons

			const k3d::string_t resource_path = "/module/uniform_polyhedron/data/" + type + ".xml";
			std::istringstream resource(k3d::resource::get_string(resource_path));
			k3d::xml::element xml_document;
			resource >> xml_document;
			if(xml_document.name != "m")
				throw std::runtime_error("Unexpected root element: " + xml_document.name);

			k3d::mesh::points_t& points = Output.points.create();
			k3d::mesh::selection_t& point_selection = Output.point_selection.create();

			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::create(Output));
			k3d::mesh::colors_t& face_colors = polyhedron->face_attributes.create<k3d::mesh::colors_t>("Cs");
			polyhedron->shell_types.push_back(k3d::polyhedron::POLYGONS);

			for(k3d::xml::element::elements_t::const_iterator xml_component = xml_document.children.begin(); xml_component != xml_document.children.end(); ++xml_component)
			{
				if(xml_component->name == "p")
				{
					points.push_back(
						size * k3d::point3(
							k3d::xml::attribute_value<k3d::double_t>(*xml_component, "x", 0.0),
							k3d::xml::attribute_value<k3d::double_t>(*xml_component, "y", 0.0),
							k3d::xml::attribute_value<k3d::double_t>(*xml_component, "z", 0.0)));
					point_selection.push_back(0.0);
				}
				else if(xml_component->name == "f")
				{
					polyhedron->face_shells.push_back(0);
					polyhedron->face_first_loops.push_back(polyhedron->loop_first_edges.size());
					polyhedron->face_loop_counts.push_back(1);
					polyhedron->face_selections.push_back(0);
					polyhedron->face_materials.push_back(material);
					polyhedron->loop_first_edges.push_back(polyhedron->clockwise_edges.size());

					k3d::uint_t edge_count = 0;
					for(k3d::xml::element::elements_t::const_iterator xml_vertex = xml_component->children.begin(); xml_vertex != xml_component->children.end(); ++xml_vertex)
					{
						if(xml_vertex->name != "v")
							throw std::runtime_error("Unexpected element: " + xml_vertex->name);

						++edge_count;				
	
						polyhedron->clockwise_edges.push_back(polyhedron->clockwise_edges.size() + 1);
						polyhedron->edge_selections.push_back(0);

						polyhedron->vertex_points.push_back(boost::lexical_cast<k3d::uint_t>(xml_vertex->text));
						polyhedron->vertex_selections.push_back(0);
					}

					polyhedron->clockwise_edges.back() = polyhedron->loop_first_edges.back();

					face_colors.push_back(edge_count < colors.size() ? colors[edge_count] : k3d::color(1, 1, 1));
				}
				else
				{
					throw std::runtime_error("Unexpected element: " + xml_component->name);
				}
			}
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
			return;
		}
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<uniform_polyhedron, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x916c98dd, 0x74f24362, 0x98364c94, 0x2a9f9ae7),
			"UniformPolyhedron",
			_("Generates a uniform polyhedron"),
			"Polyhedron",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d::idocument& m_document;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_size;

	static const k3d::ienumeration_property::enumeration_values_t& polyhedron_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			k3d::string_t polyhedron;
			std::istringstream polyhedra(k3d::resource::get_string("/module/uniform_polyhedron/data/polyhedra.txt"));
			for(std::getline(polyhedra, polyhedron); polyhedra; std::getline(polyhedra, polyhedron))
			{
				boost::iterator_range<k3d::string_t::iterator> result = boost::algorithm::find_first(polyhedron, " ");
				if(result)
				{
					values.push_back(
						k3d::ienumeration_property::enumeration_value_t(
							"(" + k3d::string_cast(values.size()) + ") " + k3d::string_t(result.end(), polyhedron.end()),
							k3d::string_t(polyhedron.begin(), result.begin()),
							""));
				}
			}
		}

		return values;
	}
};

/////////////////////////////////////////////////////////////////////////////
// uniform_polyhedron_factory

k3d::iplugin_factory& uniform_polyhedron_factory()
{
	return uniform_polyhedron::get_factory();
}

} // namespace uniform_polyhedron

} // namespace module

