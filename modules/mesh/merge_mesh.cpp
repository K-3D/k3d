// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
	\author Romain Behar (romainbehar@yahoo.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/attribute_array_copier.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imulti_mesh_sink.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/metadata_keys.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_patch.h>
#include <k3dsdk/parallel/blocked_range.h>
#include <k3dsdk/parallel/parallel_for.h>
#include <k3dsdk/parallel/threads.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/user_property_changed_signal.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace mesh
{

/////////////////////////////////////////////////////////////////////////////
// merge_mesh

class merge_mesh :
	public k3d::imulti_mesh_sink,
	public k3d::material_sink<k3d::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::mesh_source<k3d::node > > base;

public:
	merge_mesh(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_user_property_changed_signal(*this)
	{
		m_user_property_changed_signal.connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	class offset_point_indices
	{
	public:
		offset_point_indices(const k3d::uint_t Offset) :
			offset(Offset)
		{
		}

		void operator()(const k3d::string_t& Name, k3d::pipeline_data<k3d::array>& Array)
		{
			if(Array->get_metadata_value(k3d::metadata::key::domain()) != k3d::metadata::value::mesh_point_indices_domain())
				return;

			k3d::uint_t_array* const array = dynamic_cast<k3d::uint_t_array*>(&Array.writable());
			if(!array)
			{
				k3d::log() << error << "array [" << Name << "] must be a k3d::uint_t_array." << std::endl;
				return;
			}

			std::transform(array->begin(), array->end(), array->begin(), std::bind2nd(std::plus<k3d::uint_t>(), offset));
		}

	private:
		const k3d::uint_t offset;	
	};

	void on_update_mesh_topology(k3d::mesh& Output)
	{
		Output = k3d::mesh();
		k3d::mesh::points_t& output_points = Output.points.create();
		k3d::mesh::selection_t& output_point_selection = Output.point_selection.create();

		// For each input mesh ...
		const k3d::iproperty_collection::properties_t properties = k3d::property::user_properties(*static_cast<k3d::iproperty_collection*>(this));
		for(k3d::iproperty_collection::properties_t::const_iterator p = properties.begin(); p != properties.end(); ++p)
		{
			k3d::iproperty& property = **p;
			if(property.property_type() != typeid(k3d::mesh*))
				continue;

			const k3d::mesh* const mesh = k3d::property::pipeline_value<k3d::mesh*>(property);
			if(!mesh)
				continue;

			// Merge points into the output ...
			const k3d::uint_t point_offset = output_points.size();
			if(mesh->points && mesh->point_selection)
			{
				output_points.insert(output_points.end(), mesh->points->begin(), mesh->points->end());
				output_point_selection.insert(output_point_selection.end(), mesh->point_selection->begin(), mesh->point_selection->end());
			}

			// Merge primitives into the output ...
			for(k3d::mesh::primitives_t::const_iterator primitive = mesh->primitives.begin(); primitive != mesh->primitives.end(); ++primitive)
			{
				Output.primitives.push_back(*primitive);
				k3d::mesh::primitive& new_primitive = Output.primitives.back().writable();
				k3d::mesh::visit_arrays(new_primitive, offset_point_indices(point_offset));
			}
		}
	}

	void on_update_mesh_geometry(k3d::mesh& Output)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<merge_mesh, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imulti_mesh_sink> > > factory(
			k3d::uuid(0x50aef311, 0xf5264da2, 0x9c5995e8, 0xdc2e4ddf),
			"MergeMesh",
			_("Merges two meshes into one"),
			"Mesh",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d::user_property_changed_signal m_user_property_changed_signal;
};

/////////////////////////////////////////////////////////////////////////////
// merge_mesh_factory

k3d::iplugin_factory& merge_mesh_factory()
{
	return merge_mesh::get_factory();
}

} // namespace mesh

} // namespace module

