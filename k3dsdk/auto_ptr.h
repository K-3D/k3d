#ifndef K3DSDK_AUTO_PTR_H
#define K3DSDK_AUTO_PTR_H

#include "ideletable.h"
#include <boost/utility.hpp>

namespace k3d
{

/// Provides std::auto_ptr-like behavior, but uses the k3d::ideletable interface for object destruction
template<class interface_t>
class auto_ptr :
	public boost::noncopyable
{
public:
	explicit auto_ptr(interface_t* Object = 0) :
		m_object(Object)
	{
	}

	~auto_ptr()
	{
		reset(0);
	}

	void reset(interface_t* Object = 0)
	{
		delete dynamic_cast<ideletable*>(m_object);
		m_object = Object;
	}

	interface_t* get() const
	{
		return m_object;
	}

	interface_t* operator->() const
	{
		return m_object;
	}

	interface_t& operator*() const
	{
		return *m_object;
	}
	
	interface_t* release()
	{
		interface_t* result = m_object;
		m_object = 0;
		return result;
	}
	
private:
	interface_t* m_object;
};
	
} // namespace k3d

#endif // !K3DSDK_AUTO_PTR_H

