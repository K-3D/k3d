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

#include "selection.h"
#include <k3dsdk/log.h>
#include <iostream>

namespace libk3dngui
{

/////////////////////////////////////////////////////////////////////////////
// selection_mode_t

std::ostream& operator<<(std::ostream& Stream, const selection_mode_t& RHS)
{
        switch(RHS)
	{
		case SELECT_NODES:
			Stream << "nodes";
			break;
		case SELECT_POINTS:
			Stream << "points";
			break;
		case SELECT_LINES:
			Stream << "lines";
			break;
		case SELECT_FACES:
			Stream << "faces";
			break;
	}

        return Stream;
}

std::istream& operator>>(std::istream& Stream, selection_mode_t& RHS)
{
        std::string text;
        Stream >> text;

        if(text == "nodes")
                RHS = SELECT_NODES;
        else if(text == "points")
                RHS = SELECT_POINTS;
        else if(text == "lines")
                RHS = SELECT_LINES;
        else if(text == "faces")
                RHS = SELECT_FACES;
        else
                k3d::log() << error << "Unknown enumeration [" << text << "]"<< std::endl;

        return Stream;
}

} // namespace libk3dngui

