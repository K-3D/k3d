#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>

#include <iostream>

namespace module
{

namespace sample
{

class plugin :
	public k3d::node
{
public:
	plugin(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		k3d::node(Factory, Document)
	{
		k3d::log() << debug << "Howdy, World!" << std::endl;
	}

	~plugin()
	{
		k3d::log() << debug << "Goodbye, Cruel World!" << std::endl;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<plugin> factory(
			k3d::uuid(0x35b34163, 0xa1ee432a, 0x8f8503d2, 0xf09e7ac7),
			"SamplePlugin",
			"Sample K-3D document plugin",
			"Sample",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

} // namespace sample

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::sample::plugin::get_factory());
K3D_MODULE_END

