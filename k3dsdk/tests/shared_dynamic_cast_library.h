#ifndef K3DSDK_TESTS_SHARED_DYNAMIC_CAST_LIBRARY_H
#define K3DSDK_TESTS_SHARED_DYNAMIC_CAST_LIBRARY_H

#include <k3dsdk/iunknown.h>

namespace k3d
{

class istuff :
	public virtual iunknown
{
public:

protected:
	istuff() {}
	istuff(const istuff&) {}
	istuff& operator=(const istuff&) { return *this; }
	virtual ~istuff() {}
};

} // namespace k3d

typedef k3d::iunknown* (*make_stuff_entry_point)();

#endif // !K3DSDK_TESTS_SHARED_DYNAMIC_CAST_LIBRARY_H
