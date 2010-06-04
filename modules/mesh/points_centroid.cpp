// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/value_demand_storage.h>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// points_centroid

class points_centroid :
	public k3d::mesh_selection_sink<k3d::node>,
	public k3d::imesh_sink
{
	typedef k3d::mesh_selection_sink<k3d::node> base;

public:
	points_centroid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_mesh(init_owner(*this) + init_name("input_mesh") + init_label(_("Input Mesh")) + init_description(_("Input mesh")) + init_value<k3d::mesh*>(0)),
		m_centroid(init_owner(*this) + init_name("centroid") + init_label(_("Centroid")) + init_description(_("Calculated centroid")) + init_value(k3d::point3(0, 0, 0)))
	{
		m_centroid.set_update_slot(sigc::mem_fun(*this, &points_centroid::execute));

		m_input_mesh.changed_signal().connect(k3d::hint::converter<
				k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_centroid.make_slot()));

		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
				k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_centroid.make_slot()));
	}

	void execute(const std::vector<k3d::ihint*>& Hints, k3d::point3& Output)
	{
		Output = k3d::point3(0.,0.,0.);
		if(const k3d::mesh* const input = m_input_mesh.pipeline_value())
		{
			if(!input->points)
				return;
			k3d::mesh selected_mesh = *input;
			k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), selected_mesh);
			const k3d::uint_t points_begin = 0;
			const k3d::uint_t points_end = selected_mesh.points->size();
			k3d::double_t weightsum = 0.;
			for(k3d::uint_t point = points_begin; point != points_end; ++point)
			{
				const k3d::double_t weight = selected_mesh.point_selection->at(point);
				weightsum += weight;
				Output += weight * k3d::to_vector(selected_mesh.points->at(point));
			}
			Output /= weightsum;
		}
	}

	k3d::iproperty& mesh_sink_input()
	{
		return m_input_mesh;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<points_centroid,
			k3d::interface_list<k3d::imesh_sink> > factory(
				k3d::uuid(0x30c540a0, 0x544f37c4, 0x1f4a2d8b, 0xb0fa06a2),
				"PointsCentroid",
				_("Calculates the average position (centroid) of the points, weighted by the selection value"),
				"Mesh",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

	k3d_data(k3d::mesh*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::local_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_input_mesh;
	k3d_data(k3d::point3, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::value_demand_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_centroid;
};

/////////////////////////////////////////////////////////////////////////////
// points_centroid_factory

k3d::iplugin_factory& points_centroid_factory()
{
	return points_centroid::get_factory();
}

} // namespace mesh

} // namespace module
