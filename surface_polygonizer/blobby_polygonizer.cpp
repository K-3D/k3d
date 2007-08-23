// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Timothy M. Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/bounding_box3.h>
#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/result.h>

#include "blobby_polygonizer.h"
#include "isurface_polygonizer.h"
#include "jules_bloomenthal.h"

#include <limits>

// Point to segment distance
double distance_to_segment(const k3d::point3& Point, const k3d::point3& S1, const k3d::point3& S2)
{
	const k3d::vector3 vector = S2 - S1;
	const k3d::vector3 w = Point - S1;

	const double c1 = w * vector;
	if(c1 <= 0)
		return k3d::distance(Point, S1);

	const double c2 = vector * vector;
	if(c2 <= c1)
		return k3d::distance(Point, S2);

	const double b = c1 / c2;
	const k3d::point3 middlepoint = S1 + b * vector;
	return k3d::distance(Point, middlepoint);
}

k3d::point3 nearest_segment_point(const k3d::point3& Point, const k3d::point3& S1, const k3d::point3& S2)
{
	const k3d::vector3 vector = S2 - S1;
	const k3d::vector3 w = Point - S1;

	const double c1 = w * vector;
	if(c1 <= 0)
		return S1;

	const double c2 = vector * vector;
	if(c2 <= c1)
		return S2;

	const double b = c1 / c2;
	const k3d::point3 middlepoint = S1 + b * vector;
	return middlepoint;
}

/////////////////////////////////////////////////////////////////////////////
// blobby_vm

typedef std::vector<k3d::point3> origins_t;

/// Blobby virtual machine - calculates the value of an implicit surface at a given 3D point
class blobby_vm :
	public implicit_functor,
	private k3d::legacy::blobby::visitor
{
public:
	blobby_vm(k3d::legacy::blobby& Blobby, origins_t& Origins, k3d::bounding_box3& BBox, bool& IsComplex) :
		origins(Origins),
		bbox(BBox),
		is_complex(IsComplex)
	{
		Blobby.accept(*this);
	}

	double implicit_value(const vertex_t& Point)
	{
		std::stack<double> stack;
		stack.push(0);

		for(unsigned long pc = 0; pc < instructions.size(); )
		{
			switch(instructions[pc++].opcode)
			{
				case CONSTANT:
					stack.push(instructions[pc++].value);
				break;
				case ELLIPSOID:
				{
					double r2 = k3d::to_vector(*reinterpret_cast<k3d::matrix4*>(instructions[pc++].matrix) * Point).length2();
					stack.push(r2 <= 1 ? 1 - 3*r2 + 3*r2*r2 - r2*r2*r2 : 0);
				}
				break;
				case SEGMENT:
				{
					k3d::matrix4 m = *reinterpret_cast<k3d::matrix4*>(instructions[pc++].matrix);
					k3d::point3 start = *reinterpret_cast<k3d::point3*>(instructions[pc++].vector);
					k3d::point3 end = *reinterpret_cast<k3d::point3*>(instructions[pc++].vector);
					const double radius = instructions[pc++].value;
					double r2 = k3d::to_vector(k3d::inverse(k3d::translation3D(nearest_segment_point(Point, start, end)) * k3d::scaling3D(k3d::point3(radius, radius, radius)) * m) * Point).length2();
					stack.push(r2 <= 1 ? (1 - 3*r2 + 3*r2*r2 - r2*r2*r2) : 0);
				}
				break;
				case SUBTRACT:
				{
					// Stack inverts parameters
					const double b = stack.top(); stack.pop();
					const double a = stack.top(); stack.pop();
					stack.push(a - b);
				}
				break;
				case DIVIDE:
				{
					// Stack inverts parameters
					const double b = stack.top(); stack.pop();
					const double a = stack.top(); stack.pop();
					if(b != 0)
						stack.push(a / b);
					else
						stack.push(1.0);
				}
				break;
				case ADD:
				{
					const size_t count = instructions[pc++].count;
					double sum = 0;
					for(size_t i = 0; i != count; ++i)
					{
						sum += stack.top();
						stack.pop();
					}
					stack.push(sum);
				}
				break;
				case MULTIPLY:
				{
					const size_t count = instructions[pc++].count;
					double product = 1;
					for(size_t i = 0; i != count; ++i)
					{
						product *= stack.top();
						stack.pop();
					}
					stack.push(product);
				}
				break;
				case MIN:
				{
					const size_t count = instructions[pc++].count;
					double minimum = std::numeric_limits<double>::max();
					for(size_t i = 0; i != count; ++i)
					{
						minimum = std::min(minimum, stack.top());
						stack.pop();
					}
					stack.push(minimum);
				}
				break;
				case MAX:
				{
					const size_t count = instructions[pc++].count;
					double maximum = -std::numeric_limits<double>::max();
					for(size_t i = 0; i != count; ++i)
					{
						maximum = std::max(maximum, stack.top());
						stack.pop();
					}
					stack.push(maximum);
				}
				break;
			}
		}

		return stack.top();
	}

private:
	void visit_constant(k3d::legacy::blobby::constant& Constant)
	{
		instructions.push_back(instruction(CONSTANT));
		instructions.push_back(instruction(Constant.value));
	}

	void visit_ellipsoid(k3d::legacy::blobby::ellipsoid& Ellipsoid)
	{
		k3d::matrix4 transformation = k3d::translation3D(Ellipsoid.origin->position) * Ellipsoid.transformation;
		grow_bounding_box(transformation);

		instructions.push_back(instruction(ELLIPSOID));
		instructions.push_back(instruction(k3d::inverse(transformation)));
		origins.push_back(Ellipsoid.origin->position);
	}

	void visit_segment(k3d::legacy::blobby::segment& Segment)
	{
		k3d::point3& start = Segment.start->position;
		k3d::point3& end = Segment.end->position;
		k3d::matrix4& transformation = Segment.transformation;
		double radius = Segment.radius;
		grow_bounding_box(k3d::translation3D(start) * transformation, radius);
		grow_bounding_box(k3d::translation3D(end) * transformation, radius);

		instructions.push_back(instruction(SEGMENT));
		instructions.push_back(instruction(transformation));
		instructions.push_back(instruction(start));
		instructions.push_back(instruction(end));
		instructions.push_back(instruction(radius));
		origins.push_back(Segment.start->position);
	}

	void visit_subtract(k3d::legacy::blobby::subtract& Subtract)
	{
		// Note - order matters, here !
		Subtract.subtrahend->accept(*this);
		Subtract.minuend->accept(*this);

		instructions.push_back(instruction(SUBTRACT));

		is_complex = true;
	}

	void visit_divide(k3d::legacy::blobby::divide& Divide)
	{
		// Note - order matters, here !
		Divide.dividend->accept(*this);
		Divide.divisor->accept(*this);

		instructions.push_back(instruction(DIVIDE));

		is_complex = true;
	}

	void visit_add(k3d::legacy::blobby::add& Add)
	{
		Add.operands_accept(*this);

		instructions.push_back(instruction(ADD));
		instructions.push_back(instruction(Add.operands.size()));
	}

	void visit_multiply(k3d::legacy::blobby::multiply& Multiply)
	{
		Multiply.operands_accept(*this);

		instructions.push_back(instruction(MULTIPLY));
		instructions.push_back(instruction(Multiply.operands.size()));
	}

	void visit_min(k3d::legacy::blobby::min& Min)
	{
		Min.operands_accept(*this);

		instructions.push_back(instruction(MIN));
		instructions.push_back(instruction(Min.operands.size()));
	}

	void visit_max(k3d::legacy::blobby::max& Max)
	{
		Max.operands_accept(*this);

		instructions.push_back(instruction(MAX));
		instructions.push_back(instruction(Max.operands.size()));
	}

	typedef enum
	{
		CONSTANT,
		ELLIPSOID,
		SEGMENT,
		SUBTRACT,
		DIVIDE,
		ADD,
		MULTIPLY,
		MIN,
		MAX
	} opcode_t;

	union instruction
	{
	public:
		instruction(const opcode_t OpCode) : opcode(OpCode) {}
		instruction(const size_t Count) : count(Count) {}
		instruction(const double Value) : value(Value) {}
		instruction(const k3d::point3& Vector) { vector[0] = Vector[0]; vector[1] = Vector[1]; vector[2] = Vector[2]; }
		instruction(const k3d::matrix4& Matrix) { Matrix.CopyArray(matrix); }

		opcode_t opcode;
		size_t count;
		double value;
		double vector[3];
		double matrix[16];
	};

	void grow_bounding_box(const k3d::matrix4& transformation, const double radius = 1)
	{
		// Original object is a unit sphere
		const double r = 0.5 * radius;

		bbox.insert(transformation * k3d::point3(-r, 0, 0));
		bbox.insert(transformation * k3d::point3(r, 0, 0));
		bbox.insert(transformation * k3d::point3(0, -r, 0));
		bbox.insert(transformation * k3d::point3(0, r, 0));
		bbox.insert(transformation * k3d::point3(0, 0, -r));
		bbox.insert(transformation * k3d::point3(0, 0, r));
	}

	std::vector<instruction> instructions;
	origins_t& origins;
	k3d::bounding_box3& bbox;
	bool is_complex;
};

/////////////////////////////////////////////////////////////////////////////
// polygonize_blobby

void polygonize_blobby(k3d::legacy::blobby* Opcode, unsigned long Voxels, vertices_t& Vertices, vertices_t& Normals, polygons_t& Polygons)
{
	assert_warning(Opcode);

	// Set up VM
	origins_t origins;
	k3d::bounding_box3 bbox;
	bool is_complex = false;
	blobby_vm blobby_functor(*Opcode, origins, bbox, is_complex);

	// Check for empty set
	if(!origins.size())
		return;

	// Compute voxel size based on bounding box
	const double max = std::max(std::max(bbox.width(), bbox.height()), bbox.depth());
	const double min = std::min(std::min(bbox.width(), bbox.height()), bbox.depth());
	const double mid = (min+max) / 2;

	// Adaptative voxel size (for OpenGL preview)
	unsigned long voxels = Voxels;
	if(voxels == 0)
	{
		// Experimental values
		voxels = 20;
		if(mid < 12)
			voxels = 12;
		if(mid < 8 && !is_complex)
			voxels = 8;
	}

	double voxel_size = mid / static_cast<double>(voxels);

	// Set up polygonizer
	int n_x_over_2 = static_cast<int>(bbox.width() / voxel_size / 2) + 1;
	int n_y_over_2 = static_cast<int>(bbox.height() / voxel_size / 2) + 1;
	int n_z_over_2 = static_cast<int>(bbox.depth() / voxel_size / 2) + 1;

	bloomenthal_polygonizer polygonizer(
		bloomenthal_polygonizer::MARCHINGCUBES,
		voxel_size, // Voxel size
		0.421875, // Threshold (blobby specific)
		-n_x_over_2, n_x_over_2, // Lattice X min-max
		-n_y_over_2, n_y_over_2, // Lattice Y min-max
		-n_z_over_2, n_z_over_2, // Lattice Z min-max
		vertex_t(bbox.nx + bbox.width() / 2,
			bbox.ny + bbox.height() / 2,
			bbox.nz + bbox.depth() / 2), // Lattice center
		static_cast<implicit_functor&>(blobby_functor),
		Vertices, Normals, Polygons);

	polygonizer.cross_limits();

	// Polygonize blobbies
	bool missed_blobbies = false;
	for(origins_t::const_iterator p = origins.begin(); p != origins.end(); ++p)
		if(!polygonizer.polygonize_from_inside_point(*p))
			missed_blobbies = true;

	// Second chance for missed blobbies
	if(missed_blobbies)
		polygonizer.polygonize_whole_grid();
}

