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

/** \file
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/string_modifiers.h>

using namespace k3d::xml;

#include <iterator>

namespace libk3dmesh
{

/////////////////////////////////////////////////////////////////////////////
// frozen_parameter

class frozen_parameter :
	public k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	frozen_parameter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_parameter_name(init_owner(*this) + init_name("name") + init_label(_("Name")) + init_description(_("Parameter name")) + init_value<std::string>("parameter_name")),
		m_parameter_value(init_owner(*this) + init_name("value") + init_label(_("Value")) + init_description(_("Parameter value")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_parameter_name.changed_signal().connect(make_reset_mesh_slot());
		m_parameter_value.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		k3d::legacy::deep_copy(InputMesh, Mesh);
	}

	void on_update_mesh(const k3d::legacy::mesh& Source, k3d::legacy::mesh& Target)
	{
		update_selection(Source);

		size_t point_index = 0;

		const std::string name = m_parameter_name.pipeline_value();
		const double value = m_parameter_value.pipeline_value();
		for(k3d::legacy::mesh::points_t::iterator point = Target.points.begin(); point != Target.points.end(); ++point)
		{
			while(m_point_parameters.size() <= point_index)
				m_point_parameters.push_back(point_record());
			m_point_parameters[point_index++].apply(**point, name, value);
		}
	}

	bool empty() const
	{
		return
			m_point_parameters.empty();
	}

	void update_selection(const k3d::legacy::mesh& Source)
	{
		if(!empty())
			return;

		const std::string name = m_parameter_name.pipeline_value();
		for(k3d::legacy::mesh::points_t::const_iterator point = Source.points.begin(); point != Source.points.end(); ++point)
		{
			m_point_parameters.push_back(point_record((*point)->selection_weight));
		}
	}

	void save(element& Element, const k3d::ipersistent::save_context& Context)
	{
		base::save(Element, Context);

		if(empty())
			return;

		element& xml_selection = Element.append(element("parameters"));

		save_selection(xml_selection, m_point_parameters, "points");
	}

	void load(element& Element, const k3d::ipersistent::load_context& Context)
	{
		base::load(Element, Context);

		element* const xml_parameters = find_element(Element, "parameters");
		if(!xml_parameters)
			return;

		for(element::elements_t::const_iterator xml_selection = xml_parameters->children.begin(); xml_selection != xml_parameters->children.end(); ++xml_selection)
		{
			if(xml_selection->name == "points")
				load_selection(*xml_selection, m_point_parameters);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<frozen_parameter,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0x1e43c5b3, 0xb3db4dc0, 0x9f63027e, 0x6cd015fd),
				"FrozenParameter",
				_("Freezes the defined parameter"),
				"Mesh",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	struct point_record
	{
		point_record() :
			selection_weight(0)
		{
		}

		point_record(const double SelectionWeight) :
			selection_weight(SelectionWeight)
		{
		}

		/// Applies parameter to a point
		void apply(k3d::legacy::point& Point, const std::string& Name, const double Value)
		{
			Point.selection_weight = selection_weight;

			k3d::legacy::parameters_t::const_iterator p = Point.vertex_data.find(Name);
			double previous_value = 0;
			bool no_value = false;
			if(p != Point.vertex_data.end())
				previous_value = boost::any_cast<double>(p->second);
			else
				no_value = true;

			if(selection_weight > 0)
			{
				// Assign value if parameter equals default
				if(previous_value == 0)
					Point.vertex_data[Name] = Value;
			}
			else
			{
				// Parameter didn't exist, create it with default value
				const double default_value = 0;
				if(no_value)
					Point.vertex_data[Name] = default_value;
			}
		}

		bool operator != (const point_record& RHS) const
		{
			return selection_weight != RHS.selection_weight;
		}

		double selection_weight;
	};

	typedef std::vector<point_record> point_records_t;

	void save_selection(element& Element, const point_records_t& Records, const std::string& ElementName)
	{
		if(Records.empty())
			return;

		element& xml_records = Element.append(element(ElementName));

		// We do a simple run-length encoding of serialized selection data to keep file sizes reasonable
		size_t begin = 0;
		point_record last_record(0);

		for(size_t i = 0; i != Records.size(); ++i)
		{
			if(!i)
				last_record = Records[i];

			if(Records[i] != last_record)
			{
				xml_records.append(element("parameter", attribute("begin", begin), attribute("end", i), attribute("weight", last_record.selection_weight)));

				begin = i;
				last_record = Records[i];
			}
		}
		if(Records.size())
			xml_records.append(element("parameter", attribute("begin", begin), attribute("end", Records.size()), attribute("weight", last_record.selection_weight)));
	}

	void load_selection(const element& Element, point_records_t& Records)
	{
		for(element::elements_t::const_iterator xml_selection = Element.children.begin(); xml_selection != Element.children.end(); ++xml_selection)
		{
			if(xml_selection->name != "parameter")
				continue;

			// Selections are stored using a simple run-length encoding to keep things efficient
			const size_t begin = attribute_value<size_t>(*xml_selection, "begin", 0);
			const size_t end = attribute_value<size_t>(*xml_selection, "end", 0);
			const double weight = attribute_value<double>(*xml_selection, "weight", 0.0);

			if(end > Records.size())
				Records.resize(end);

			const point_record record(weight);
			for(size_t i = begin; i != end; ++i)
				Records[i] = record;
		}
	}

	point_records_t m_point_parameters;

private:
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_parameter_name;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_parameter_value;
};

/////////////////////////////////////////////////////////////////////////////
// frozen_parameter_factory

k3d::iplugin_factory& frozen_parameter_factory()
{
	return frozen_parameter::get_factory();
}

} // namespace libk3dmesh

