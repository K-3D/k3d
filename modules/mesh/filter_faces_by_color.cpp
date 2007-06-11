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
#include <k3dsdk/i18n.h>
#include <k3dsdk/color.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_modifier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <iterator>

namespace k3d
{

inline bool operator < (const k3d::color& a, const k3d::color& b)
{
	if(a.red != b.red)
		return a.red < b.red;

	if(a.green != b.green)
		return a.green < b.green;

	return a.blue < b.blue;
}

} // namespace k3d

namespace libk3dmesh
{

/////////////////////////////////////////////////////////////////////////////
// filter_faces_by_color_implementation

class filter_faces_by_color_implementation :
	public k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> >
{
	typedef k3d::legacy::mesh_modifier<k3d::persistent<k3d::node> > base;

public:
	filter_faces_by_color_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_color_number(init_owner(*this) + init_name("color_number") + init_label(_("Color number")) + init_description(_("Color number in the virtual color list")) + init_value(1) + init_step_increment(1) + init_constraint(constraint::minimum(1L)) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_color_number.changed_signal().connect(make_reset_mesh_slot());
	}

	/** \todo Improve the implementation so we don't have to do this */
	k3d::iunknown* on_rewrite_hint(iunknown* const Hint)
	{
		// Force updates to re-allocate our mesh, for simplicity
		return 0;
	}

	void on_initialize_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
		// Find Nth color
		const unsigned long color_number = m_color_number.value();
		unsigned long current_color = 0;
		std::map<k3d::color, unsigned long> color_map;
		bool found = false;
		k3d::color selected_color;

		for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = InputMesh.polyhedra.begin(); polyhedron != InputMesh.polyhedra.end(); ++polyhedron)
		{
			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				k3d::legacy::parameters_t::const_iterator color = (*face)->uniform_data.find("Cs");
				if(color != (*face)->uniform_data.end())
				{
					k3d::color face_color = boost::any_cast<k3d::color>(color->second);
					if(color_map.find(face_color) != color_map.end())
						// Color was already indexed
						continue;
					else
					{
						// Save color
						color_map[face_color] = current_color++;
						if(current_color == color_number)
						{
							found = true;
							selected_color = face_color;
							break;
						}
					}
				}
			}

			if(found)
				break;
		}

		// If specified color wasn't found, return
		if(!found)
			return;

		// Copy faces of the same color
		k3d::legacy::polyhedron* output_polyhedron = new k3d::legacy::polyhedron();
		return_if_fail(output_polyhedron);
		Mesh.polyhedra.push_back(output_polyhedron);

		typedef std::map<k3d::legacy::point*, k3d::legacy::point*> point_map_t;
		point_map_t point_map;
		for(k3d::legacy::mesh::polyhedra_t::const_iterator polyhedron = InputMesh.polyhedra.begin(); polyhedron != InputMesh.polyhedra.end(); ++polyhedron)
		{
			for(k3d::legacy::polyhedron::faces_t::const_iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
			{
				k3d::legacy::parameters_t::const_iterator color = (*face)->uniform_data.find("Cs");
				if(color != (*face)->uniform_data.end())
				{
					k3d::color face_color = boost::any_cast<k3d::color>(color->second);
					if(face_color != selected_color)
						continue;

					// Copy face ...
					k3d::legacy::split_edge* first_edge = (*face)->first_edge;
					k3d::legacy::split_edge* current_edge = first_edge;

					std::vector<k3d::legacy::split_edge*> edges;
					do
					{
						k3d::legacy::point* point = current_edge->vertex;
						k3d::legacy::point* new_point;
						point_map_t::const_iterator point_equivalence = point_map.find(point);
						if(point_equivalence != point_map.end())
						{
							new_point = point_equivalence->second;
						}
						else
						{
							new_point = new k3d::legacy::point(point->position);
							Mesh.points.push_back(new_point);
							return_if_fail(new_point);
						}

						edges.push_back(new k3d::legacy::split_edge(new_point));
						current_edge = current_edge->face_clockwise;
					}
					while(current_edge != first_edge);

					k3d::legacy::loop_edges(edges.begin(), edges.end());

					k3d::legacy::face* const new_face = new k3d::legacy::face(**face);
					new_face->first_edge = *edges.begin();

					output_polyhedron->faces.push_back(new_face);
				}
			}
		}
	}

	void on_update_mesh(const k3d::legacy::mesh& InputMesh, k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<filter_faces_by_color_implementation,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink> > > factory(
				k3d::uuid(0x1efa74fc, 0x6e5d4ab8, 0x9110d288, 0xdbb975ed),
				"FilterFacesByColor",
				_("Filter faces by color"),
				"Yagotme",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_color_number;
};

/////////////////////////////////////////////////////////////////////////////
// filter_faces_by_color_factory

k3d::iplugin_factory& filter_faces_by_color_factory()
{
	return filter_faces_by_color_implementation::get_factory();
}

} // namespace libk3dmesh


