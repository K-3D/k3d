// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
    \author Evan Lezar (evanlezar@gmail.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>

#include "cuda_mesh.h"

namespace module
{
    
namespace cuda
{
    
/////////////////////////////////////////////////////////////////////////////
// cuda_mesh_cuda_to_k3d_converter

class cuda_mesh_cuda_to_k3d_converter : 
    public k3d::mesh_source<k3d::node >
{
    typedef k3d::mesh_source<k3d::node > base;

public:
    cuda_mesh_cuda_to_k3d_converter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
        base(Factory, Document),
        m_input_cuda_device_mesh(init_owner(*this) + init_name("device_input_mesh") + init_label(_("Device Mesh")) + init_description(_("Device mesh")) + init_value<cuda_device_mesh*>(0))        
    {
        m_input_cuda_device_mesh.changed_signal().connect(sigc::mem_fun(*this, &cuda_mesh_cuda_to_k3d_converter::convert_from_cuda_mesh));
    }
    
    void convert_from_cuda_mesh(k3d::iunknown*)
    {
        k3d::log() << info << "Converting from CUDA" << std::endl;
        
        k3d::log() << debug <<  m_input_cuda_device_mesh.internal_value() << " : " << m_input_cuda_device_mesh.pipeline_value() << std::endl;
        
        if ( m_input_cuda_device_mesh.pipeline_value() )
        {
            m_input_cuda_device_mesh.pipeline_value()->output_debug_info();
            m_input_cuda_device_mesh.pipeline_value()->copy_from_device( *(m_output_mesh.internal_value()) );
        } 
        k3d::log() << info << "After Conversion" << std::endl;
    }
    
    void on_create_mesh_topology(k3d::mesh& Mesh)
    {
        k3d::log() << info << "Create Topology" << std::endl;
    }
    
    void on_update_mesh_geometry(k3d::mesh& Mesh)
    {
        k3d::log() << info << "Update Geometry" << std::endl;
    }
     
    static k3d::iplugin_factory& get_factory()
    {
        static k3d::document_plugin_factory<cuda_mesh_cuda_to_k3d_converter,
            k3d::interface_list<k3d::imesh_source> > factory(
                k3d::uuid(0x53ad6a9b, 0x434e15bc, 0x18297a93, 0xd0b43a5e),
                "CUDAMeshCUDAToK3dConverter",
                _("Convert a CUDA mesh to a k3d mesh"),
                "CUDAMesh",
                k3d::iplugin_factory::EXPERIMENTAL);

        return factory;
    }

private:
    k3d_data(cuda_device_mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input_cuda_device_mesh;             
};

/////////////////////////////////////////////////////////////////////////////
// cuda_mesh_cuda_to_k3d_converter

k3d::iplugin_factory& cuda_mesh_cuda_to_k3d_converter_factory()
{
    return cuda_mesh_cuda_to_k3d_converter::get_factory();
}  
  
} // namespace cuda
    
} // namespace module
