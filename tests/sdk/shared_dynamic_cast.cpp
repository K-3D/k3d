#include <shared_dynamic_cast_config.h>
#include <shared_dynamic_cast_library.h>

#include <iostream>
#include <stdexcept>

#include <dlfcn.h>

int main(int argc, char* argv[])
{
	try
	{
		void* const module = dlopen(SHARED_DYNAMIC_CAST_LIBRARY, RTLD_GLOBAL | RTLD_LAZY);
		if(!module)
			throw std::runtime_error(dlerror());

		make_stuff_entry_point make_stuff = make_stuff_entry_point(dlsym(module, "make_stuff"));
		if(!make_stuff)
			throw std::runtime_error("Missing make_stuff() function.");

		k3d::iunknown* const unknown = make_stuff();
		if(!unknown)
			throw std::runtime_error("Missing object instance.");

		k3d::istuff* const stuff = dynamic_cast<k3d::istuff*>(unknown);
		if(!stuff)
			throw std::runtime_error("Dynamic casting failed.");

		std::cerr << "Success!" << std::endl;
		return 0;
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
}

