#ifndef K3DSDK_IRENDER_FRAME_H
#define K3DSDK_IRENDER_FRAME_H

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
		\author Tim Shead (tshead@k-3d.com)
*/

#include "iunknown.h"
#include <string>

namespace k3d
{

namespace filesystem { class path; }

/// Abstract interface encapsulating a single frame to be rendered
class irender_frame :
	public virtual iunknown
{
public:
	/// Returns a unique filepath that can be used as an input file for this frame
	virtual const filesystem::path add_input_file(const std::string& Name) = 0;
	/// Returns a unique filepath that can be used as an output file for this frame
	virtual const filesystem::path add_output_file(const std::string& Name) = 0;
	/// Sets-up a render operation for transforming input files to outputfiles (the render engine will be matched against choices in the user options file)
	virtual void add_render_operation(const std::string& Type, const std::string& Name, const filesystem::path& File, const bool VisibleRender) = 0;
	/// Sets-up a copy operation that will be executed once rendering is complete
	virtual void add_copy_operation(const filesystem::path& SourceFile, const filesystem::path& TargetFile) = 0;
	/// Sets-up a file view operation once copying is complete
	virtual void add_view_operation(const filesystem::path& FilePath) = 0;

protected:
	irender_frame() {}
	irender_frame(const irender_frame&) {}
	irender_frame& operator=(const irender_frame&) { return *this; }
	virtual ~irender_frame() {}
};

} // namespace k3d

#endif // K3DSDK_IRENDER_FRAME_H

