#ifndef K3DSDK_INETWORK_RENDER_FRAME_H
#define K3DSDK_INETWORK_RENDER_FRAME_H

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

/// Abstract interface that encapsulates all of the work required to render a "frame".
/// All of the steps ("commands") required to render the frame will be executed in the
/// order that they were added using add_exec_command(), add_copy_command(), and add_view_command().
class inetwork_render_frame :
	public virtual iunknown
{
public:
	class variable
	{
	public:
		variable()
		{
		}

		variable(const string_t& Name, const string_t& Value) :
			name(Name),
			value(Value)
		{
		}

		string_t name;
		string_t value;
	};

	typedef std::vector<variable> environment;

	class argument
	{
	public:
		argument()
		{
		}

		argument(const string_t& Value) :
			value(Value)
		{
		}

		string_t value;
	};

	typedef std::vector<argument> arguments;

	/// Returns a unique filepath that can be used as an input/output file for this frame
	virtual const filesystem::path add_file(const string_t& Name) = 0;
	/// Sets-up an arbitrary command to be executed
	virtual void add_exec_command(const string_t& Binary, const environment& Environment, const arguments& Arguments) = 0;
	/// Sets-up a copy operation from one filesystem location to another
	virtual void add_copy_command(const filesystem::path& Source, const filesystem::path& Target) = 0;
	/// Sets-up an view operation that will display a file to the user
	virtual void add_view_command(const filesystem::path& File) = 0;

protected:
	inetwork_render_frame() {}
	inetwork_render_frame(const inetwork_render_frame&) {}
	inetwork_render_frame& operator=(const inetwork_render_frame&) { return *this; }
	virtual ~inetwork_render_frame() {}
};

} // namespace k3d

#endif // K3DSDK_INETWORK_RENDER_FRAME_H

