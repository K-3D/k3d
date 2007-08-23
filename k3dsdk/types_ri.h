#ifndef K3DSDK_TYPES_RI_H
#define K3DSDK_TYPES_RI_H

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

#include "algebra.h"
#include "bounding_box3.h"
#include "color.h"
#include "path.h"
#include "typed_array.h"

#include <boost/any.hpp>
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <list>
#include <vector>

namespace k3d
{

namespace ri
{

/// RenderMan integer datatype
typedef boost::int32_t integer;
/// RenderMan unsigned integer datatype
typedef boost::uint32_t unsigned_integer;
/// RenderMan real datatype
typedef double real;
/// RenderMan string datatype
typedef std::string string;
/// RenderMan point datatype
typedef k3d::point3 point;
/// RenderMan vector datatype
typedef k3d::vector3 vector;
/// RenderMan normal datatype
typedef k3d::normal3 normal;
/// Renderman color datatype
typedef k3d::color color;
/// Renderman hpoint datatype
typedef k3d::point4 hpoint;
/// RenderMan matrix datatype
typedef k3d::matrix4 matrix;
/// Filesystem path datatype
typedef filesystem::path path;
/// 3D bounding-box datatype
typedef k3d::bounding_box3 bound;
/// RenderMan light handle datatype
typedef boost::uint32_t light_handle;
/// RenderMan object handle datatype
typedef boost::uint32_t object_handle;

/// RenderMan integer array
typedef std::vector<integer> integers;
/// RenderMan unsigned integer array
typedef std::vector<unsigned_integer> unsigned_integers;
/// RenderMan real array
typedef std::vector<real> reals;
/// RenderMan string array
typedef std::vector<string> strings;
/// RenderMan point array
typedef std::vector<point> points;
/// RenderMan vector array
typedef std::vector<vector> vectors;
/// RenderMan normal array
typedef std::vector<normal> normals;
/// RenderMan color array
typedef std::vector<color> colors;
/// RenderMan hpoint array
typedef std::vector<hpoint> hpoints;
/// RenderMan matrix array
typedef std::vector<matrix> matrices;

/// Enumerates parameter storage classes defined by the RenderMan standard
typedef enum
{
	CONSTANT,
	UNIFORM,
	VARYING,
	VERTEX,
	FACEVARYING
} storage_class_t;

/// Defines a named array of homogeneous RenderMan data
class parameter
{
public:
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const integer& Value);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const unsigned_integer& Value);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const real& Value);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const string& Value);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const point& Value);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const vector& Value);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const normal& Value);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const color& Value);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const hpoint& Value);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const matrix& Value);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<integer>* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<unsigned_integer>* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<real>* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<string>* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<point>* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<vector>* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<normal>* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<color>* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<hpoint>* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const typed_array<matrix>* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const array* Values);
	parameter(const string& Name, const storage_class_t StorageClass, const unsigned_integer TupleSize, const boost::any& Value);

	const string name;
	const storage_class_t storage_class;
	const unsigned_integer tuple_size;
	boost::shared_ptr<const k3d::array> storage;
};

/// Defines a collection of named arrays
typedef std::list<parameter> parameter_list;

/// Defines a collection of intra-frame sample times in the range [0, 1]
typedef std::vector<real> sample_times_t;

/// Enumerates possible values for the current render context
typedef enum
{
	/// We're generating a CSG solid object
	CSG_SOLID,
	/// We're generating an object instance
	OBJECT_INSTANCE,
	/// We're generating a shadow map
	SHADOW_MAP,
	/// We're generating an environment map
	ENVIRONMENT_MAP,
	/// We're generating the final output frame
	FINAL_FRAME,

} render_context_t;

} // namespace ri

} // namespace k3d

#endif // K3DSDK_TYPES_RI_H

