#include <k3dsdk/module.h>

namespace module
{
	
namespace gmsh
{

namespace io
{
	
extern k3d::iplugin_factory& mesh_writer_factory();

} // namespace io

} // namespace gmsh

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::gmsh::io::mesh_writer_factory());
K3D_MODULE_END
