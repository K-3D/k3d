#ifndef K3DSDK_IHINT_H
#define K3DSDK_IHINT_H

#include "iunknown.h"
#include <iosfwd>

namespace k3d
{

/// Abstract interface implemented by objects that can act as "hints" for change events
class ihint :
	public virtual iunknown
{
public:
	virtual void print(std::ostream& Stream) = 0;

protected:
	ihint() {}
	ihint(const ihint&) {}
	ihint& operator=(const ihint&) { return *this; }
	virtual ~ihint() {}
};

} // namespace k3d

#endif // !K3DSDK_IHINT_H

