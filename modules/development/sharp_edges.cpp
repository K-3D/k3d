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
		\author Bart Janssens (bart.janssens@lid.kviv.be)
		\brief Sets the selected edges as "sharp"
 */

#include <k3d-i18n-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/array.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/xml.h>

#include <list>

namespace module
{

namespace development
{

	/////////////////////////////////////////////////////////////////////////////
// sharp_edges

class sharp_edges :
	public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
	typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
	typedef std::vector<size_t> sharp_edges_t;
	typedef k3d::typed_array<bool> sharpness_array_t;
public:
	sharp_edges(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_sharp_edges(init_owner(*this) + init_name("sharp_edge_list") + init_label(_("Sharp edges")) + init_description(_("List with edge indices that are marked as sharp by this node")) + init_value(sharp_edges_t()))
	{
		m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
		m_sharp_edges.changed_signal().connect(make_update_mesh_slot());
	}

	/// Mesh modifier implementation
	void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		Output = Input;
		k3d::make_unique(Output.points);
		
		k3d::merge_selection(m_mesh_selection.pipeline_value(), Output);
		
		return_if_fail(Output.polyhedra);
		return_if_fail(Output.polyhedra->edge_selection);
		return_if_fail(Output.polyhedra->edge_points);
		
		sharp_edges_t sharp_edge_list;
		
		size_t edgecount = Output.polyhedra->edge_selection->size();
		for (size_t edge = 0; edge != edgecount; ++edge)
		{
			if (Output.polyhedra->edge_selection->at(edge))
			{
				sharp_edge_list.push_back(edge);
			}
		}
		
		k3d::mesh::polyhedra_t* writable_polyhedra = const_cast<k3d::mesh::polyhedra_t*>(Output.polyhedra.get());
		if (Output.polyhedra->face_varying_data.find("N") == Output.polyhedra->face_varying_data.end())
			writable_polyhedra->face_varying_data["N"] = boost::shared_ptr<sharpness_array_t>(new sharpness_array_t(edgecount, false));
		m_sharp_edges.set_value(sharp_edge_list);
	}
	
	/// Mesh modifier implementation
	void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
	{
		return_if_fail(Output.polyhedra);
		k3d::mesh::named_arrays::const_iterator array_it = Output.polyhedra->face_varying_data.find("N");
		return_if_fail(array_it !=  Output.polyhedra->face_varying_data.end());
		return_if_fail(dynamic_cast<sharpness_array_t*>(array_it->second.get()));
		
		sharpness_array_t& sharpness_array = *dynamic_cast<sharpness_array_t*>(array_it->second.get());
		const sharp_edges_t& sharp_edge_list = m_sharp_edges.pipeline_value();
		size_t sharpcount = sharp_edge_list.size();
		for (size_t edge = 0; edge != sharpcount; ++edge)
		{
			sharpness_array[sharp_edge_list[edge]] = true;
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<sharp_edges,
		k3d::interface_list<k3d::imesh_source,
		k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xd5e31efa, 0xae024102, 0x88797645, 0x897c9592),
		"MakeSharpEdges",
		_("Make selected edges sharp"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
	
	k3d_data(sharp_edges_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_sharp_edges; // TODO: implement serialization

	// protected:
// 	virtual void emit_hint()
// 	{
// 		base::emit_hint(&m_hint);
// 	}
};

/////////////////////////////////////////////////////////////////////////////
// sharp_edges_factory

k3d::iplugin_factory& sharp_edges_factory()
{
	return sharp_edges::get_factory();
}

} // namespace development

} // namespace module

