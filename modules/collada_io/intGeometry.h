// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
	\author Barbiero Mattia
*/

#ifndef __intGeometry_h__
#define __intGeometry_h__


#include <k3dsdk/mesh.h>
#include <dom/domGeometry.h>


//typedef daeSmartRef<intGeometry> intGeometryRef;
//typedef daeTArray<intGeometryRef> intGeometryArray;

/**
 * Geometry describes the visual shape and appearance of an object in the
 * scene. The geometry element categorizes the declaration of geometric information.
 * Geometry is a  branch of mathematics that deals with the measurement, properties,
 * and relationships of  points, lines, angles, surfaces, and solids.
 */

namespace module
{

namespace collada
{

namespace io
{

class intGeometry
{
private:

    k3d::mesh Mesh;
public: // Constuctor and Destructor
	/**
	 * Default Constructor.
	 */
	intGeometry(domGeometry&);
	k3d::mesh getMesh(){return Mesh;}
	/**
	 * Default Destructor.
	 */
	~intGeometry(){}
};
}}}
#endif
