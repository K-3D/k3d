#include <k3dsdk/module.h>

namespace module
{
	
namespace ogre
{

namespace io
{
	
extern k3d::iplugin_factory& mesh_writer_factory();

} // namespace io

} // namespace ogre

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::ogre::io::mesh_writer_factory());
K3D_MODULE_END
