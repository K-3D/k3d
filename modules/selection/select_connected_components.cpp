// K-3D
// Copyright (c) 2005-2008 Timothy M. Shead
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
#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/geometry.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh_selection_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/node.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <boost/scoped_ptr.hpp>
#include <boost/mpl/for_each.hpp>

namespace module
{

namespace selection
{

namespace detail
{
	/// Interface for an array wrapper, providing a means to compare data
struct iarray_wrapper
{
	virtual ~iarray_wrapper() {}
	
	/// Compare the data values at the two indices
	virtual k3d::bool_t compare(const k3d::uint_t a, const k3d::uint_t b) const = 0;
};
	
/// Wraps a named array of type T
template<typename T>
struct array_wrapper : iarray_wrapper
{
	array_wrapper(const k3d::typed_array<T>& Array) : m_array(Array)
	{
	}
	
	virtual k3d::bool_t compare(const k3d::uint_t a, const k3d::uint_t b) const
	{
		return m_array[a] == m_array[b];
	}
	
private:
	const k3d::typed_array<T>& m_array;
};

/// MPL functor to wrap an array
struct wrap_array_ftor
{
	wrap_array_ftor(const k3d::array* Array, boost::shared_ptr<iarray_wrapper>& Result) : m_array(Array), m_result(Result)
	{
	}
	
	template<typename T>
	void operator()(const T&)
	{
		k3d::typed_array<T> const * const arr_ptr = dynamic_cast<k3d::typed_array<T> const *>(m_array);
		if(arr_ptr)
			m_result.reset(new array_wrapper<T>(*arr_ptr));
	}
	
private:
	const k3d::array* m_array;
	boost::shared_ptr<iarray_wrapper>& m_result;
};

boost::shared_ptr<iarray_wrapper> wrap_array(const k3d::array* Array)
{
	boost::shared_ptr<iarray_wrapper> result;
	boost::mpl::for_each<k3d::named_array_types>(wrap_array_ftor(Array, result));
	return result;
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// select_connected_components

class select_connected_components :
	public k3d::mesh_selection_modifier<k3d::mesh_selection_sink<k3d::node > >
{
	typedef k3d::mesh_selection_modifier<k3d::mesh_selection_sink<k3d::node > > base;

public:
	select_connected_components(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_select_faces(init_owner(*this) + init_name("select_faces") + init_label(_("Select Faces")) + init_description(_("Select connected faces")) + init_value(true)),
		m_select_edges(init_owner(*this) + init_name("select_edges") + init_label(_("Select Edges")) + init_description(_("Select connected edges")) + init_value(true)),
		m_select_vertices(init_owner(*this) + init_name("select_vertices") + init_label(_("Select Vertices")) + init_description(_("Select connected vertices")) + init_value(true)),
		m_select_points(init_owner(*this) + init_name("select_points") + init_label(_("Select Points")) + init_description(_("Select connected points")) + init_value(true)),
		m_same_attributes(init_owner(*this) + init_name("same_attributes") + init_label(_("Same Attributes")) + init_description(_("Only selects components where the face attributes in the supplied array are the same")) + init_value(false)),
		m_array(init_owner(*this) + init_name("array") + init_label(_("Array")) + init_description(_("Specifies the array name for the attributes that must match when growing the selection (if Same Attributes is on)")) + init_value(std::string("Cs")))
	{
		m_mesh_selection.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));

		m_select_faces.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_select_edges.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_select_vertices.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_select_points.changed_signal().connect(k3d::hint::converter<
					k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_same_attributes.changed_signal().connect(k3d::hint::converter<
					k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
		m_array.changed_signal().connect(k3d::hint::converter<
					k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(make_update_mesh_slot()));
	}

	void on_update_selection(const k3d::mesh& Input, k3d::mesh& Output)
	{
		// Shallow copy of the input (no data is copied, only shared pointers are)
		Output = Input;
		k3d::geometry::selection::merge(m_mesh_selection.pipeline_value(), Output); // Merges the current document selection with the mesh

		const k3d::bool_t select_faces = m_select_faces.pipeline_value();
		const k3d::bool_t select_edges = m_select_edges.pipeline_value();
		const k3d::bool_t select_vertices = m_select_vertices.pipeline_value();
		const k3d::bool_t select_points = m_select_points.pipeline_value();
		
		const k3d::string_t array_name = m_array.pipeline_value();

		k3d::mesh::selection_t& output_point_selection = Output.point_selection.writable();

		for(k3d::uint_t i = 0; i != Input.primitives.size(); ++i)
		{
			boost::scoped_ptr<k3d::polyhedron::primitive> polyhedron(k3d::polyhedron::validate(Output, Output.primitives[i]));
			if(!polyhedron)
				continue;

			const k3d::uint_t face_begin = 0;
			const k3d::uint_t face_end = polyhedron->face_selections.size();
			k3d::mesh::selection_t input_selected_faces(face_end - face_begin, 0.0);
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(polyhedron->face_selections[face])
				{
					input_selected_faces[face] = 1.;
					continue;
				}
				const k3d::uint_t loop_begin = polyhedron->face_first_loops[face];
				const k3d::uint_t loop_end = loop_begin + polyhedron->face_loop_counts[face];
				for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
				{
					const k3d::uint_t first_edge = polyhedron->loop_first_edges[loop];
					for(k3d::uint_t edge = first_edge; ;)
					{
						if(polyhedron->edge_selections[edge] || polyhedron->vertex_selections[edge] || output_point_selection[polyhedron->vertex_points[edge]])
						{
							input_selected_faces[face] = 1.;
							break;
						}
						edge = polyhedron->clockwise_edges[edge];
						if(edge == first_edge)
							break;
					}
				}
			}

			k3d::mesh::selection_t output_selection(face_end - face_begin, 0.0);
			connected_faces_selector selector(*polyhedron, output_selection, array_name, m_same_attributes.pipeline_value());
			for(k3d::uint_t face = face_begin; face != face_end; ++face)
			{
				if(input_selected_faces[face])
					selector.select_connected_faces(face);
			}

			if(select_faces)
				polyhedron->face_selections.assign(output_selection.begin(), output_selection.end());
			add_selections(output_selection, select_edges, select_vertices, select_points, *polyhedron, output_point_selection);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<select_connected_components,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xe54a08e3, 0x014445d3, 0x5395c8a6, 0xb3bfaae3),
				"SelectConnectedComponents",
				_("Select all components that are directly or indirectly connected to the selected component"),
				"Selection",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	/// Recurse through faces, selecting all connected faces
	struct connected_faces_selector
	{
		const k3d::polyhedron::const_primitive& m_polyhedron;
		k3d::mesh::selection_t& m_output_selection;
		k3d::mesh::bools_t m_boundary_edges;
		k3d::mesh::indices_t m_adjacent_edges;
		k3d::mesh::indices_t m_edge_faces;
		boost::shared_ptr<detail::iarray_wrapper> m_face_array_wrapper;
		const k3d::bool_t m_use_attributes;
		connected_faces_selector(const k3d::polyhedron::const_primitive& Polyhedron, k3d::mesh::selection_t& OutputFaceSelection, const std::string& ArrayName, const k3d::bool_t UseAttributes) : m_polyhedron(Polyhedron), m_output_selection(OutputFaceSelection), m_use_attributes(UseAttributes)
		{
			k3d::polyhedron::create_edge_adjacency_lookup(m_polyhedron.vertex_points, m_polyhedron.clockwise_edges, m_boundary_edges, m_adjacent_edges);
			k3d::polyhedron::create_edge_face_lookup(m_polyhedron, m_edge_faces);
			m_face_array_wrapper = detail::wrap_array(Polyhedron.face_attributes.lookup(ArrayName));
		}

		void select_connected_faces(const k3d::uint_t Face)
		{
			if(m_output_selection[Face])
				return;
			m_output_selection[Face] = 1.0;
			const k3d::uint_t loop_begin = m_polyhedron.face_first_loops[Face];
			const k3d::uint_t loop_end = m_polyhedron.face_loop_counts[Face] + loop_begin;
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = m_polyhedron.loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ;)
				{
					if(!m_boundary_edges[edge])
					{
						const k3d::uint_t companion_face = m_edge_faces[m_adjacent_edges[edge]];
						if(!m_output_selection[companion_face] && (!m_use_attributes || (m_face_array_wrapper && m_face_array_wrapper->compare(Face, companion_face))))
							select_connected_faces(companion_face);
					}

					edge = m_polyhedron.clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
	};

	void add_selections(const k3d::mesh::selection_t FaceSelection, const k3d::bool_t SelectEdges, const k3d::bool_t SelectVertices, const k3d::bool_t SelectPoints, k3d::polyhedron::primitive& Polyhedron, k3d::mesh::selection_t& PointSelection)
	{
		const k3d::uint_t face_begin = 0;
		const k3d::uint_t face_end = Polyhedron.face_selections.size();
		for(k3d::uint_t face = face_begin; face != face_end; ++face)
		{
			if(!FaceSelection[face])
				continue;

			const k3d::uint_t loop_begin = Polyhedron.face_first_loops[face];
			const k3d::uint_t loop_end = loop_begin + Polyhedron.face_loop_counts[face];
			for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const k3d::uint_t first_edge = Polyhedron.loop_first_edges[loop];
				for(k3d::uint_t edge = first_edge; ;)
				{
					Polyhedron.edge_selections[edge] = SelectEdges ? 1.0 : 0.0;
					Polyhedron.vertex_selections[edge] = SelectVertices ? 1.0 : 0.0;
					PointSelection[Polyhedron.vertex_points[edge]] = SelectPoints ? 1.0 : 0.0;

					edge = Polyhedron.clockwise_edges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
	}

	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_select_faces;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_select_edges;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_select_vertices;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_select_points;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_same_attributes;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_array;
};

/////////////////////////////////////////////////////////////////////////////
// select_connected_components_factory

k3d::iplugin_factory& select_connected_components_factory()
{
	return select_connected_components::get_factory();
}

} // namespace selection

} // namespace module

