#ifndef BLOBBY_POLYGONIZER_H
#define BLOBBY_POLYGONIZER_H

// K-3D
// Copyright (c) 2004-2006, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\brief Defines RiBlobby polygonizer interface
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/legacy_mesh.h>

void polygonize_blobby(k3d::legacy::blobby* Opcode, unsigned long Voxels,
	std::vector<k3d::point3>& Vertices, std::vector<k3d::point3>& Normals,
	std::vector< std::vector<unsigned long> >& Polygons);

#endif // BLOBBY_POLYGONIZER_H

