#ifndef K3DSDK_IRENDER_ENGINE_RI_H
#define K3DSDK_IRENDER_ENGINE_RI_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include "types.h"

namespace k3d
{

namespace filesystem { class path; }
class inetwork_render_frame;

namespace ri
{

/// Abstract interface for objects that provide an interface to specific RenderMan implementations
class irender_engine :
	public virtual iunknown
{
public:
	virtual ~irender_engine() {}

	/// Returns true iff the underlying RenderMan engine is installed and usable
	virtual const bool_t installed() = 0;
	/// Compiles the given shader source code, placing the results into the global shader cache
	virtual const bool_t compile_shader(const filesystem::path& Shader) = 0;
	/// Renders the given RIB file
	virtual const bool_t render(inetwork_render_frame& Frame, const filesystem::path& RIB) = 0;

protected:
	irender_engine() {}
	irender_engine(const irender_engine& Other) : iunknown(Other) {}
	irender_engine& operator=(const irender_engine& ) { return *this; }
};
} // namespace ri

} // namespace k3d

#endif // !K3DSDK_IRENDER_ENGINE_RI_H

