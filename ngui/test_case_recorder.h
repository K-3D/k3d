#ifndef NGUI_TEST_CASE_RECORDER_H
#define NGUI_TEST_CASE_RECORDER_H

// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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

namespace libk3dngui
{

/// Records user input (a tutorial, basically) directly to a file, so test cases can be recorded even when there's a crash
void create_test_case_recorder();

} // namespace libk3dngui

#endif // NGUI_TEST_CASE_RECORDER_H

