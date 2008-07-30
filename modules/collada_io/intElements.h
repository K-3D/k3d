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

// intElements is the implementation of the conversion between the dae format elements and
// their equivalent on K-3D

#ifndef __intGeometry_h__
#define __intGeometry_h__


#include <k3dsdk/mesh.h>
#include <k3dsdk/algebra.h>
#include <dom/domGeometry.h>
#include <dom/domCamera.h>
#include <dom/domLight.h>

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

// Integrates mesh creation and loading 
// from domGeometry -> FrozenMesh
class intGeometry
{
private:

    k3d::mesh Mesh;
public: // Constuctor and Destructor
	/**
	* Default Constructor.
	* Passes ccst (Current Coordinate System Transformation) to compute real vertex positions
	*/
	intGeometry(domGeometry&, const k3d::matrix4& ccst);

	/// returns generated mesh in k3d::mesh format
	k3d::mesh getMesh(){return Mesh;}
	/**
	 * Default Destructor.
	 */
	~intGeometry(){}
};

// Integrates camera loading
// from domCamera -> Camera
class intCamera
{
private:
	k3d::point3 Position;
	k3d::point3 Direction;
	//k3d::float FOV;
public:
	intCamera(domCamera&, const k3d::matrix4& ccst);
	k3d::point3 get_position(){return Position;}
	k3d::point3 get_direction(){return Direction;}
};


// Integrates light loading
// from domLight -> Light
class intLight
{
private:
	k3d::point3 Position;
public:
	intLight(domLight&, const k3d::matrix4& ccst);
	k3d::point3 get_position(){return Position;}
};

}}}
#endif
