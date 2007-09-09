#ifndef CONVERSION_H_
#define CONVERSION_H_

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
//
// As a special exception, you have permission to link this program
// with the CGAL (http://www.cgal.org) library and distribute executables, as long as you
// follow the requirements of the GNU GPL in regard to all of the
// software in the executable aside from CGAL.

/** \file
		\brief Conversion between CGAL and K-3D formats
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/


#include "threshold_number.h"

//#include <CGAL/Gmpz.h>
//#include <CGAL/MP_Float.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Filtered_kernel.h>
//#include <CGAL/Lazy_exact_nt.h>
//#include <CGAL/Homogeneous.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/to_rational.h>

#include <CGAL/assertions.h>

#include <k3dsdk/algebra.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/mesh.h>

namespace libk3dbooleans
{
	//typedef CGAL::Lazy_exact_nt<double> NT; // Using an exact number type would probably be a good idea, but fails to compile...
	typedef CGAL::threshold_number NT;
	//typedef CGAL::Gmpz NT;
	typedef CGAL::Cartesian<NT> Kernel;
	typedef CGAL::Quotient<NT> Rational;
	typedef CGAL::Nef_polyhedron_3<Kernel, CGAL::SNC_indexed_items> Nef_polyhedron;
	typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
	typedef CGAL::Point_3<Kernel> Point_3;
	
	/// Converts a Polyhedron to a k3d mesh
	void to_mesh(const Polyhedron& Polyhedron, k3d::mesh& Mesh, k3d::imaterial* const Material);
	
	/// Converts a Nef_polyhedron to a k3d mesh
	void to_mesh(Nef_polyhedron& NefPolyhedron, k3d::mesh& Mesh, k3d::imaterial* const Material);
	
	/// Converts a k3d mesh to a Nef polyhedron
	boost::shared_ptr<Nef_polyhedron> to_nef(const k3d::mesh& Mesh);
	
	/// Convert errors to the k3d logging system
	void k3d_failure_handler(const char *type, const char *expr, const char* file, int line, const char* msg);
	
}

#endif /*CONVERSION_H_*/
