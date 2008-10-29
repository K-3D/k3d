#ifndef K3DSDK_IPROJECTION_H
#define K3DSDK_IPROJECTION_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\brief Declares abstract interfaces that encapsulate viewing projections
		\author Tim Shead (tshead@k-3d.com)
*/

#include "k3d-platform-config.h"

#include "iunknown.h"
#ifdef	K3D_API_WIN32
#ifdef	near
#undef	near
#endif	// near
#ifdef	far
#undef	far
#endif	// far
#endif	// K3D_API_WIN32

namespace k3d
{

// Forward declarations
class iproperty;

/////////////////////////////////////////////////////////////////////////////
// iprojection

/// Abstract interface encapsulating a 3D-to-2D viewing projection
class iprojection :
	public virtual k3d::iunknown
{
public:

protected:
	iprojection() {}
	iprojection(const iprojection&) {}
	iprojection& operator=(const iprojection&) { return *this; }
	virtual ~iprojection() {}
};

/// Abstract interface encapsulating a standard perspective (truncated pyramid, or frustum) transformation
class iperspective :
	public iprojection
{
public:
	virtual iproperty& left() = 0;
	virtual iproperty& right() = 0;
	virtual iproperty& top() = 0;
	virtual iproperty& bottom() = 0;
	virtual iproperty& near() = 0;
	virtual iproperty& far() = 0;

protected:
	iperspective() {}
	iperspective(const iperspective&) {}
	iperspective& operator=(const iperspective&) { return *this; }
	virtual ~iperspective() {}
};

/// Abstract interface encapsulating a standard orthographic transformation
class iorthographic :
	public iprojection
{
public:
	virtual iproperty& left() = 0;
	virtual iproperty& right() = 0;
	virtual iproperty& top() = 0;
	virtual iproperty& bottom() = 0;
	virtual iproperty& near() = 0;
	virtual iproperty& far() = 0;

protected:
	iorthographic() {}
	iorthographic(const iorthographic&) {}
	iorthographic& operator=(const iorthographic&) { return *this; }
	virtual ~iorthographic() {}
};

} // namespace k3d

#endif // !K3DSDK_IPROJECTION_H
