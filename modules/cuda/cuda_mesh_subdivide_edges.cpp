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
		\author Romain Behar (romainbehar@yahoo.com)
		\author Bart Janssens (bart.janssens@lid.kviv.be)
        \author Evan Lezar (evanlezar@gmail.com)
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/ipipeline_profiler.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/mesh_topology_data.h>
#include <k3dsdk/named_array_copier.h>
#include <k3dsdk/node.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/shared_pointer.h>
#include <k3dsdk/utility.h>
#include <k3dsdk/vectors.h>

#include <k3d-platform-config.h>

#include "cuda_device_mesh.h"
#include "cuda_mesh_topology_data.h"

namespace module
{

namespace cuda
{

namespace detail
{

typedef std::vector<k3d::uint32_t> indices_t;

/// Gets the edges to subdivide, and creates a mapping between the old edge index and the new index
class edge_index_calculator
{
public:
    edge_index_calculator(const k3d::mesh::polyhedra_t& Polyhedra,
            const indices_t& Companions,
            const k3d::mesh::bools_t& BoundaryEdges,
            const k3d::uint_t SplitPointCount,
            const k3d::uint_t FirstNewPoint,
            indices_t& EdgeList,
            k3d::mesh::indices_t& IndexMap,
            indices_t& FirstMidPoint,
            k3d::mesh::bools_t& HasMidPoint) :
                edge_count(0),
                m_polyhedra(Polyhedra),
                m_companions(Companions),
                m_boundary_edges(BoundaryEdges),
                m_split_point_count(SplitPointCount),
                m_first_new_point(FirstNewPoint),
                m_edge_list(EdgeList),
                m_index_map(IndexMap),
                m_first_midpoint(FirstMidPoint),
                m_has_midpoint(HasMidPoint)
            {}

    void operator()(const k3d::uint_t Face)
    {
        const k3d::mesh::indices_t& face_first_loops = *m_polyhedra.face_first_loops;
        const k3d::mesh::counts_t& face_loop_counts = *m_polyhedra.face_loop_counts;
        const k3d::mesh::selection_t& edge_selection = *m_polyhedra.edge_selection;
        const k3d::mesh::indices_t& loop_first_edges = *m_polyhedra.loop_first_edges;
        const k3d::mesh::indices_t& clockwise_edges = *m_polyhedra.clockwise_edges;

        const k3d::uint_t loop_begin = face_first_loops[Face];
        const k3d::uint_t loop_end = loop_begin + face_loop_counts[Face];
        for(k3d::uint_t loop = loop_begin; loop != loop_end; ++loop)
        {
            const k3d::uint_t first_edge = loop_first_edges[loop];
            for(k3d::uint_t edge = first_edge; ; )
            {
                m_index_map[edge] = edge_count;
                ++edge_count;
                if(!m_boundary_edges[edge] && edge_selection[m_companions[edge]] && !edge_selection[edge])
                {
                    edge_count += m_split_point_count;
                    m_first_midpoint[edge] = m_first_midpoint[m_companions[edge]];
                }

                if(edge_selection[edge])
                {
                    edge_count += m_split_point_count;
                    if(!m_boundary_edges[edge] && m_has_midpoint[m_companions[edge]])
                    {
                        m_first_midpoint[edge] = m_first_midpoint[m_companions[edge]];
                    }
                    else
                    {
                        m_first_midpoint[edge] = m_first_new_point + m_split_point_count * m_edge_list.size();
                        m_edge_list.push_back(edge);
                        m_has_midpoint[edge] = true;
                    }
                }

                edge = clockwise_edges[edge];
                if(edge == first_edge)
                    break;
            }
        }
    }

    /// The new total edge count
    k3d::uint_t edge_count;

private:
    const k3d::mesh::polyhedra_t& m_polyhedra;
    const indices_t& m_companions;
    const k3d::mesh::bools_t& m_boundary_edges;
    const k3d::uint_t m_split_point_count;
    const k3d::uint_t m_first_new_point;
    indices_t& m_edge_list;
    k3d::mesh::indices_t& m_index_map;
    indices_t& m_first_midpoint;
    k3d::mesh::bools_t& m_has_midpoint;
};

/// Copies and interpolates the varying data as needed (serial usage only)
class varying_data_copier
{
public:
    varying_data_copier(const k3d::mesh::points_t Points,
            const k3d::mesh::indices_t& EdgePoints,
            const k3d::mesh::indices_t& ClockwiseEdges,
            const indices_t& Companions,
            const k3d::mesh::bools_t& BoundaryEdges,
            const k3d::mesh::bools_t& HasMidpoint,
            const k3d::uint_t SplitPointCount,
            k3d::named_array_copier& Copier) :
                m_points(Points),
                m_edge_points(EdgePoints),
                m_clockwise_edges(ClockwiseEdges),
                m_companions(Companions),
                m_boundary_edges(BoundaryEdges),
                m_has_midpoint(HasMidpoint),
                m_split_point_count(SplitPointCount),
                m_copier(Copier)
            {}

    void operator()(const k3d::uint_t Edge)
    {
        m_copier.push_back(Edge);
        if(m_has_midpoint[Edge] || (!m_boundary_edges[Edge] && m_has_midpoint[m_companions[Edge]]))
        {
            const k3d::uint_t clockwise = m_clockwise_edges[Edge];
            const k3d::uint_t indices[] = {Edge, clockwise};
            const k3d::point3& start_point = m_points[m_edge_points[Edge]];
            const k3d::point3& end_point = m_points[m_edge_points[Edge]];
            const k3d::double_t weight_step = 1.0 / static_cast<double>(m_split_point_count + 1);
            for(k3d::uint_t i = 1; i <= m_split_point_count; ++i)
            {
                const k3d::double_t last_weight = weight_step * static_cast<k3d::double_t>(i);
                const k3d::double_t weights[] = {1 - last_weight, last_weight};
                m_copier.push_back(2, indices, weights);
            }
        }
    }

private:
    const k3d::mesh::points_t m_points;
    const k3d::mesh::indices_t& m_edge_points;
    const k3d::mesh::indices_t& m_clockwise_edges;
    const indices_t& m_companions;
    const k3d::mesh::bools_t& m_boundary_edges;
    const k3d::mesh::bools_t& m_has_midpoint;
    const k3d::uint_t m_split_point_count;
    k3d::named_array_copier& m_copier;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// cuda_mesh_subdivide_edges

class cuda_mesh_subdivide_edges :
    public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
{
    typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;

public:
    cuda_mesh_subdivide_edges(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
        base(Factory, Document),
        m_vertices(init_owner(*this) + init_name("vertices") + init_label(_("Vertices")) + init_description(_("Number of vertices to insert in each selected edge")) + init_value(1L) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum<k3d::int32_t>(1))),
        m_p_output_device_mesh(),
        m_p_input_device_mesh(),
        m_pdev_edge_list(0)
    {
        m_vertices.changed_signal().connect(make_reset_mesh_slot());
        m_mesh_selection.changed_signal().connect(make_reset_mesh_slot());
    }

    ~cuda_mesh_subdivide_edges()
    {
    	if ( m_pdev_edge_list )
    	{
    		free_device_memory ( (void*) m_pdev_edge_list );
    	}
    }

    void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
    {
        if ( m_pdev_edge_list )
        {
        	free_device_memory( (void*) m_pdev_edge_list );
        	synchronize_threads();
        	m_pdev_edge_list = 0;
        }

        // If there are no valid polyhedra, we give up
        document().pipeline_profiler().start_execution(*this, "Validate input");
        if(!k3d::validate_polyhedra(Input))
        {
            document().pipeline_profiler().finish_execution(*this, "Validate input");
            return;
        }
        // should move up
        m_p_input_device_mesh.reset ( new cuda_device_mesh ( Input) );
        m_p_input_device_mesh->copy_to_device( );

        document().pipeline_profiler().finish_execution(*this, "Validate input");

        // Shallow copy of the input (no data is copied, only shared pointers are)
        document().pipeline_profiler().start_execution(*this, "Merge selection");
        Output = Input;
        k3d::merge_selection(m_mesh_selection.pipeline_value(), Output); // Merges the current document selection with the mesh
        document().pipeline_profiler().finish_execution(*this, "Merge selection");

        k3d::mesh::polyhedra_t& polyhedra = *k3d::make_unique(Output.polyhedra);

        document().pipeline_profiler().start_execution(*this, "Calculate companions");
        k3d::mesh::bools_t boundary_edges;
        detail::indices_t companions;
#ifndef K3D_UINT_T_64_BITS
        k3d::create_edge_adjacency_lookup(*polyhedra.edge_points, *polyhedra.clockwise_edges, boundary_edges, static_cast<k3d::mesh::indices_t&>(companions));
#else
        k3d::mesh::indices_t companions_64;
        k3d::create_edge_adjacency_lookup(*polyhedra.edge_points, *polyhedra.clockwise_edges, boundary_edges, companions_64);
        companions.resize(companions_64.size());
        std::copy(companions_64.begin(), companions_64.end(), companions.begin());
#endif
        document().pipeline_profiler().finish_execution(*this, "Calculate companions");

        document().pipeline_profiler().start_execution(*this, "Convert Boundary");
		k3d::typed_array<unsigned char> tmp_boundary (boundary_edges.size());

		// need to convert the bit references to chars
		for ( int k = 0; k < tmp_boundary.size() ; k++ )
		{
			if ( boundary_edges[k] )
				tmp_boundary[k] = 1;
			else
				tmp_boundary[k] = 0;
		}
		document().pipeline_profiler().finish_execution(*this, "Convert Boundary");

		document().pipeline_profiler().start_execution(*this, "Calculate indices");

        const k3d::uint_t split_point_count = m_vertices.pipeline_value();
        const k3d::uint_t old_edge_count = polyhedra.edge_points->size();

        k3d::uint32_t* pdev_index_map;
        k3d::uint32_t* pdev_first_midpoint;
        k3d::uint32_t* pdev_companions;
        unsigned char* pdev_has_midpoint;
        unsigned char* pdev_boundary_edges;


        // allocate the device memory
        allocate_device_memory((void**)&pdev_index_map, old_edge_count*sizeof(k3d::uint32_t));
        allocate_device_memory((void**)&pdev_first_midpoint, old_edge_count*sizeof(k3d::uint32_t));
        allocate_device_memory((void**)&pdev_has_midpoint, old_edge_count*sizeof(unsigned char));
        set_device_memory((void*)pdev_has_midpoint, 0, old_edge_count*sizeof(unsigned char));


        // the edge list can maximally contain all the edges
        allocate_device_memory((void**)&m_pdev_edge_list, (1+split_point_count)*tmp_boundary.size()*sizeof(k3d::uint32_t));

        allocate_device_memory((void**)&pdev_companions, companions.size()*sizeof(k3d::uint32_t));
        allocate_device_memory((void**)&pdev_boundary_edges, tmp_boundary.size()*sizeof(unsigned char));


        copy_from_host_to_device((void*)pdev_companions, (const void*)&(companions.front()), companions.size()*sizeof(k3d::uint32_t));
        copy_from_host_to_device((void*)pdev_boundary_edges, (const void*)&(tmp_boundary.front()), tmp_boundary.size()*sizeof(unsigned char));

        unsigned int edge_index_calculator_edge_count;

        m_p_output_device_mesh.reset( new cuda_device_mesh ( Output ) );
        m_p_output_device_mesh->copy_to_device(POLYHEDRA_ALL_EDGES+MESH_POINTS+MESH_SELECTION+POLYHEDRA_ALL_LOOPS);

        k3d::log() << debug << "edge_index_calculator_entry" << std::endl;

        edge_index_calculator_edge_count = edge_index_calculator_entry (
									(unsigned int*) m_pdev_edge_list,
									&m_edge_list_size,
									(unsigned int*) pdev_first_midpoint,
									pdev_has_midpoint,
									(unsigned int*) pdev_index_map,
									(const unsigned int*)m_p_input_device_mesh->get_device_polyhedra().get_per_face_first_loops_pointer(),
									(const unsigned int*)m_p_input_device_mesh->get_device_polyhedra().get_per_face_loop_counts_pointer(),
									(const unsigned int*)m_p_input_device_mesh->get_device_polyhedra().get_per_loop_first_edges_pointer(),
									(const unsigned int*)m_p_input_device_mesh->get_device_polyhedra().get_per_edge_clockwise_edges_pointer(),
									m_p_output_device_mesh->get_device_polyhedra().get_per_edge_selection_pointer(),
									(const unsigned int*) pdev_companions,
									(const unsigned char*) pdev_boundary_edges,
									split_point_count,
									Input.polyhedra->face_first_loops->size(),
									Input.points->size()
									);

        k3d::log() << debug << "resize edge list" << std::endl;
        // resize the edge_list allocated on the device
        k3d::uint32_t* tmp_pdev_edge_list;
        allocate_device_memory((void**)&tmp_pdev_edge_list, m_edge_list_size*sizeof(k3d::uint32_t));
        synchronize_threads();
        copy_from_device_to_device((void*)tmp_pdev_edge_list, (const void*)m_pdev_edge_list, m_edge_list_size*sizeof(k3d::uint32_t));
        synchronize_threads();
        free_device_memory((void*)m_pdev_edge_list);
        synchronize_threads();
        m_pdev_edge_list = tmp_pdev_edge_list;

        document().pipeline_profiler().finish_execution(*this, "Calculate indices");

        document().pipeline_profiler().start_execution(*this, "Allocate memory");

        m_p_output_device_mesh->get_device_polyhedra().resize_edges(edge_index_calculator_edge_count, true);
        //boost::shared_ptr<k3d::mesh::indices_t> output_edge_points(new k3d::mesh::indices_t(edge_index_calculator_edge_count));
        //boost::shared_ptr<k3d::mesh::indices_t> output_clockwise_edges(new k3d::mesh::indices_t(edge_index_calculator_edge_count));
        //boost::shared_ptr<k3d::mesh::selection_t> output_edge_selection(new k3d::mesh::selection_t(edge_index_calculator_edge_count, 0.0));
        //k3d::mesh::points_t& output_points = *k3d::make_unique(Output.points);
        //k3d::mesh::selection_t& output_point_selection = *k3d::make_unique(Output.point_selection);
        const k3d::uint_t new_point_count = m_edge_list_size * split_point_count + Input.points->size();

        //k3d::mesh::indices_t& output_loop_first_edges = *k3d::make_unique(polyhedra.loop_first_edges);

        document().pipeline_profiler().finish_execution(*this, "Allocate memory");

        document().pipeline_profiler().start_execution(*this, "Update indices");

        //polyhedra.edge_points = output_edge_points;
        //polyhedra.clockwise_edges = output_clockwise_edges;

        //m_p_output_device_mesh.reset( new cuda_device_mesh ( Output ) );
        //m_p_output_device_mesh->copy_to_device();

        m_p_output_device_mesh->resize_points_and_selection ( new_point_count, 1.0 );
        k3d::log() << debug << "subdivide_edges_update_indices_entry" << std::endl;

        subdivide_edges_update_indices_entry ((unsigned int*)m_p_input_device_mesh->get_polyhedra_edge_point_indices_pointer(),
                                              (unsigned int*)m_p_input_device_mesh->get_polyhedra_clockwise_edge_point_indices_pointer(),
                                              (unsigned int) Input.polyhedra->edge_points->size(),
                                              (unsigned int*)(m_p_output_device_mesh->get_polyhedra_edge_point_indices_pointer()),
                                              (unsigned int*)(m_p_output_device_mesh->get_polyhedra_clockwise_edge_point_indices_pointer()),
                                              pdev_index_map,
                                              old_edge_count);// index_map.size = old_edge_count
        k3d::log() << debug << "subdivide_edges_update_loop_first_edges_entry" << std::endl;
        subdivide_edges_update_loop_first_edges_entry ((unsigned int*)m_p_output_device_mesh->get_polyhedra_loop_first_edges_pointer(),
                                                 (unsigned int)Input.polyhedra->loop_first_edges->size(),
                                                 pdev_index_map,
                                                 old_edge_count);// index_map.size = old_edge_count


        document().pipeline_profiler().finish_execution(*this, "Update indices");


        document().pipeline_profiler().start_execution(*this, "Split edges");
        k3d::log() << debug << "subdivide_edges_split_edges_entry" << std::endl;
        subdivide_edges_split_edges_entry ((unsigned int*)(m_p_output_device_mesh->get_polyhedra_edge_point_indices_pointer()),
                                           (unsigned int*)(m_p_output_device_mesh->get_polyhedra_clockwise_edge_point_indices_pointer()),
                                           (unsigned int*)m_p_input_device_mesh->get_polyhedra_clockwise_edge_point_indices_pointer(),
                                           pdev_index_map,
                                           m_pdev_edge_list,
                                           (unsigned int)m_edge_list_size,
                                           (unsigned int)m_vertices.pipeline_value(),
                                           pdev_first_midpoint,
                                           pdev_companions,
                                           pdev_boundary_edges
                                           );

        synchronize_threads();


        polyhedra.clockwise_edges.reset();
        polyhedra.edge_points.reset();
        polyhedra.edge_selection.reset();
        polyhedra.loop_first_edges.reset();


        m_p_output_device_mesh->copy_from_device( Output, POLYHEDRA_ALL_EDGES + POLYHEDRA_ALL_LOOPS);

        k3d::log() << debug << "free device memory" << std::endl;
        free_device_memory ( pdev_index_map );
        free_device_memory ( pdev_first_midpoint );
        free_device_memory ( pdev_has_midpoint );
        free_device_memory ( pdev_companions );
        free_device_memory ( pdev_boundary_edges );

        document().pipeline_profiler().finish_execution(*this, "Split edges");

        //polyhedra.edge_selection = output_edge_selection;
        polyhedra.constant_data = Input.polyhedra->constant_data;
        polyhedra.uniform_data = Input.polyhedra->uniform_data;
        k3d::log() << debug << "on_create_mesh::end" << std::endl;
    }

    void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
    {
        document().pipeline_profiler().start_execution(*this, "Validate input");

        if(!k3d::validate_polyhedra(Input))
        {
            document().pipeline_profiler().finish_execution(*this, "Validate input");
            return;
        }
        document().pipeline_profiler().finish_execution(*this, "Validate input");

        //k3d::mesh::points_t& output_points = *k3d::make_unique(Output.points);

        document().pipeline_profiler().start_execution(*this, "Calculate positions");

        subdivide_edges_split_point_calculator ( m_pdev_edge_list,
                                                (unsigned int)m_edge_list_size,
                                                m_p_output_device_mesh->get_points_and_selection_pointer(),
                                                (unsigned int)Input.points->size(),
                                                m_p_input_device_mesh->get_device_polyhedra().get_per_edge_points_pointer(),
                                                m_p_input_device_mesh->get_device_polyhedra().get_per_edge_clockwise_edges_pointer(),
                                                (unsigned int)m_vertices.pipeline_value());


        m_p_output_device_mesh->copy_from_device ( Output, MESH_POINTS + MESH_SELECTION );

        document().pipeline_profiler().finish_execution(*this, "Calculate positions");
    }

    static k3d::iplugin_factory& get_factory()
    {
        static k3d::document_plugin_factory<cuda_mesh_subdivide_edges,
            k3d::interface_list<k3d::imesh_source,
            k3d::interface_list<k3d::imesh_sink > > > factory(
                k3d::uuid(0x7cf6b6b8, 0x154c3103, 0x2db817b2, 0x1319509a),
                "CUDASubdivideEdges",
                "Subdivides edges by creating one or more vertices along selected edges using CUDA API",
                "CUDAMesh",
                k3d::iplugin_factory::EXPERIMENTAL);

        return factory;
    }

private:
    k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_vertices;

    // Cache the midpoints, for fast updating
    //detail::indices_t m_edge_list;

    k3d::uint32_t* m_pdev_edge_list;
    k3d::uint32_t m_edge_list_size;
    boost::shared_ptr<cuda_device_mesh> m_p_output_device_mesh;
    boost::shared_ptr<cuda_device_mesh> m_p_input_device_mesh;
};

/////////////////////////////////////////////////////////////////////////////
// cuda_mesh_subdivide_edges_factory

k3d::iplugin_factory& cuda_mesh_subdivide_edges_factory()
{
    return cuda_mesh_subdivide_edges::get_factory();
}

} // namespace cuda

} // namespace module
