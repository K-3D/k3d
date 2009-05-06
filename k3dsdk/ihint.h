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
	virtual ~ihint() {}
	virtual ihint* clone() = 0;
	virtual void print(std::ostream& Stream) = 0;

protected:
	ihint() {}
	ihint(const ihint& Other) : iunknown(Other) {}
	ihint& operator=(const ihint&) { return *this; }
};

} // namespace k3d

#endif // !K3DSDK_IHINT_H

