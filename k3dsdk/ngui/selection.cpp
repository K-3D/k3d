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

namespace k3d
{

namespace ngui
{

namespace selection
{

/////////////////////////////////////////////////////////////////////////////
// mode

std::ostream& operator<<(std::ostream& Stream, const mode& RHS)
{
        switch(RHS)
	{
		case NODES:
			Stream << "nodes";
			break;
		case POINTS:
			Stream << "points";
			break;
		case SPLIT_EDGES:
			Stream << "split_edges";
			break;
		case UNIFORM:
			Stream << "uniform";
			break;
	}

        return Stream;
}

std::istream& operator>>(std::istream& Stream, mode& RHS)
{
        std::string text;
        Stream >> text;

        if(text == "nodes")
                RHS = NODES;
        else if(text == "points")
                RHS = POINTS;
        else if(text == "split_edges")
                RHS = SPLIT_EDGES;
        else if(text == "uniform")
                RHS = UNIFORM;
        else
                k3d::log() << error << "Unknown enumeration [" << text << "]"<< std::endl;

        return Stream;
}

} // namespace selection

} // namespace ngui

} // namespace k3d

