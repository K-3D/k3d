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
#include <k3dsdk/mesh_sink.h>
#include <k3dsdk/node.h>

#include "cuda_mesh.h"

namespace module
{
    
namespace cuda
{
    
/////////////////////////////////////////////////////////////////////////////
// cuda_mesh_k3d_to_cuda_converter

class cuda_mesh_k3d_to_cuda_converter : 
    public k3d::mesh_sink<k3d::node >
{
    typedef k3d::mesh_sink<k3d::node > base;

public:
    cuda_mesh_k3d_to_cuda_converter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
        base(Factory, Document),
        m_output_cuda_device_mesh(init_owner(*this) + init_name("device_output_mesh") + init_label(_("Device Mesh")) + init_description(_("Device mesh")) + init_value<cuda_device_mesh*>(0))        
    {
        m_input_mesh.changed_signal().connect(sigc::mem_fun(*this, &cuda_mesh_k3d_to_cuda_converter::convert_to_cuda_mesh));
    }
    
    ~cuda_mesh_k3d_to_cuda_converter()
    {
        k3d::log() << debug << "Free the cuda mesh object" << std::endl;
        // free the device mesh allocated
        delete m_output_cuda_device_mesh.internal_value();       
    }
    
    void convert_to_cuda_mesh(k3d::iunknown*)
    {
        k3d::log() << info << "Converting to CUDA" << std::endl;
        
        if ( m_output_cuda_device_mesh.internal_value() )
        {
            delete m_output_cuda_device_mesh.internal_value(); 
        }
        
        m_output_cuda_device_mesh.set_value( new cuda_device_mesh ( *(m_input_mesh.pipeline_value()) ) );
        m_output_cuda_device_mesh.internal_value()->copy_to_device();
        
        k3d::log() << info << "Converting to CUDA : after copy" << std::endl;
        
        m_output_cuda_device_mesh.internal_value()->output_debug_info();

    }
    
    static k3d::iplugin_factory& get_factory()
    {
        static k3d::document_plugin_factory<cuda_mesh_k3d_to_cuda_converter,
            k3d::interface_list<k3d::imesh_sink> > factory(
                k3d::uuid(0xd6321dd2, 0xd949bf23, 0x049c58bd, 0x4562ddae),
                "CUDAMeshK3dToCUDAConverter",
                _("Convert a k3d mesh do a CUDA mesh"),
                "CUDAMesh",
                k3d::iplugin_factory::EXPERIMENTAL);

        return factory;
    }

private:
    k3d_data(cuda_device_mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_output_cuda_device_mesh;             
};

/////////////////////////////////////////////////////////////////////////////
// cuda_mesh_k3d_to_cuda_converter

k3d::iplugin_factory& cuda_mesh_k3d_to_cuda_converter_factory()
{
    return cuda_mesh_k3d_to_cuda_converter::get_factory();
}  
  
} // namespace cuda
    
} // namespace module
