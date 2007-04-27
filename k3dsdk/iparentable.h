#ifndef K3DSDK_IPARENTABLE_H
#define K3DSDK_IPARENTABLE_H

#include "iunknown.h"

namespace k3d
{

class iproperty;

/// Abstract interface for an object that can be "parented" to another.  Note: parenting implies, but does not require, that the child's transformations be related to the parent's
class iparentable :
	public virtual iunknown
{
public:
	/// Returns the object's parent, if any (could be NULL)
	virtual iproperty& parent() = 0;
	
protected:
	iparentable() {}
	iparentable(const iparentable&) {}
	iparentable& operator=(const iparentable&) { return *this; }
	virtual ~iparentable() {}
};

} // namespace k3d

#endif // !K3DSDK_IPARENTABLE_H

