#include <shared_dynamic_cast_library.h>

extern "C"
{

class stuff :
	public k3d::istuff
{
public:
};

k3d::iunknown* make_stuff()
{
	return new stuff();
}

} // extern "C"
