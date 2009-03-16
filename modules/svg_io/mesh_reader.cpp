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
	\author Carlos Andres Dominguez Caballero (carlosadc@gmail.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/mesh_reader.h>
#include <k3dsdk/node.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/xml.h>

#include <boost/scoped_ptr.hpp>

#include <stack>

namespace module
{

namespace svg
{

namespace io
{

/// Defines storage for a LIFO collection of transformation matrices.
typedef std::stack<k3d::matrix4> transform_stack;

/// Helper function for parse_path to extract a pair (x,y) regardless of the Format i.e. "x,y" or "x y".
static void get_pair(k3d::double_t& x, k3d::double_t& y, std::istringstream& def_stream)
{
	// Based on Tim's code in obj_io
	def_stream >> x;
	if (def_stream.peek() == ',' || def_stream.peek() == ' ')
	{
		char separator;
		def_stream >> separator;
		def_stream >> y;
	}
}

static void parse_line(const k3d::xml::element& SVG, transform_stack& Transformation, k3d::mesh& Mesh, k3d::nurbs_curve::primitive& Primitive)
{
	const k3d::double_t x1 = k3d::xml::attribute_value<k3d::double_t>(SVG, "x1", 0);
	const k3d::double_t y1 = k3d::xml::attribute_value<k3d::double_t>(SVG, "y1", 0);
	const k3d::double_t x2 = k3d::xml::attribute_value<k3d::double_t>(SVG, "x2", 1);
	const k3d::double_t y2 = k3d::xml::attribute_value<k3d::double_t>(SVG, "y2", 1);

	k3d::mesh::points_t control_points;
	control_points.push_back(Transformation.top() * k3d::point3(x1, y1, 0));
	control_points.push_back(Transformation.top() * k3d::point3(x2, y2, 0));

	k3d::nurbs_curve::add_curve(Mesh, Primitive, 2, control_points);
}

static void parse_rect(const k3d::xml::element& SVG, transform_stack& Transformation, k3d::mesh& Mesh, k3d::nurbs_curve::primitive& Primitive)
{
	const k3d::double_t x = k3d::xml::attribute_value<k3d::double_t>(SVG, "x", 0);
	const k3d::double_t y = k3d::xml::attribute_value<k3d::double_t>(SVG, "y", 0);
	const k3d::double_t w = k3d::xml::attribute_value<k3d::double_t>(SVG, "width", 1);
	const k3d::double_t h = k3d::xml::attribute_value<k3d::double_t>(SVG, "height", 1);
	k3d::double_t rx = k3d::xml::attribute_value<k3d::double_t>(SVG, "rx", -1);
	k3d::double_t ry = k3d::xml::attribute_value<k3d::double_t>(SVG, "ry", -1);

	if(ry == -1)
		ry = rx;
	else if(rx == -1)
		rx = ry;

	if(rx <=0 && ry <= 0)
	{
		k3d::mesh::points_t control_points;
		control_points.push_back(Transformation.top() * k3d::point3(x, y, 0));
		control_points.push_back(Transformation.top() * k3d::point3(x + w, y, 0));
		control_points.push_back(Transformation.top() * k3d::point3(x + w, y + h, 0));
		control_points.push_back(Transformation.top() * k3d::point3(x, y + h, 0));

		k3d::nurbs_curve::add_curve(Mesh, Primitive, 2, control_points, 1);
	}
	else
	{

		// Create a circular_arc in the positive quadrant and mirror points to generate corners ...
		k3d::mesh::knots_t corner_knots;
		k3d::mesh::weights_t corner_weights;
		k3d::mesh::points_t corner_control_points;
		k3d::nurbs_curve::circular_arc(k3d::vector3(rx, 0, 0), k3d::vector3(0, ry, 0), 0, k3d::pi() / 2, 1, corner_knots, corner_weights, corner_control_points);

		// Start the curve ...
		k3d::mesh::points_t control_points;
		k3d::mesh::weights_t weights;
		k3d::mesh::knots_t knots;

		// Upper left corner ...
		for(k3d::uint_t i = 0; i != corner_control_points.size(); ++i)
		{
			control_points.push_back(Transformation.top() * k3d::point3(x + rx - corner_control_points[i][0], y + ry - corner_control_points[i][1], 0));
			weights.push_back(corner_weights[i]);
		}

		// Top side ...
		control_points.push_back(Transformation.top() * k3d::mix(k3d::point3(x + rx, y, 0), k3d::point3(x + w - rx, y, 0), 0.5));
		weights.push_back(1);

		// Upper right corner ...
		for(k3d::uint_t i = corner_control_points.size(); i != 0; --i)
		{
			control_points.push_back(Transformation.top() * k3d::point3(x + w - rx + corner_control_points[i-1][0], y + ry - corner_control_points[i-1][1], 0));
			weights.push_back(weights[i-1]);
		}

		// Right side ...
		control_points.push_back(Transformation.top() * k3d::mix(k3d::point3(x + w, y + ry, 0), k3d::point3(x + w, y + h - ry, 0), 0.5));
		weights.push_back(1);

		// Lower right corner
		for(k3d::uint_t i = 0; i != corner_control_points.size(); ++i)
		{
			control_points.push_back(Transformation.top() * k3d::point3(x + w - rx + corner_control_points[i][0], y + h - ry + corner_control_points[i][1], 0));
			weights.push_back(weights[i]);
		}

		// Bottom side ...
		control_points.push_back(Transformation.top() * k3d::mix(k3d::point3(x + rx, y + h, 0), k3d::point3(x + w - rx, y + h, 0), 0.5));
		weights.push_back(1);

		// Lower left corner ...
		for(k3d::uint_t i = corner_control_points.size(); i != 0; --i)
		{
			control_points.push_back(Transformation.top() * k3d::point3(x + rx - corner_control_points[i-1][0], y + h - ry + corner_control_points[i-1][1], 0));
			weights.push_back(weights[i-1]);
		}

		// Left side ...
		control_points.push_back(Transformation.top() * k3d::mix(k3d::point3(x, y + h - ry, 0), k3d::point3(x, y + ry, 0), 0.5));
		weights.push_back(1);

		knots.insert(knots.end(), 3, 0);
		knots.insert(knots.end(), 2, 1);
		knots.insert(knots.end(), 2, 2);
		knots.insert(knots.end(), 2, 3);
		knots.insert(knots.end(), 2, 4);
		knots.insert(knots.end(), 2, 5);
		knots.insert(knots.end(), 2, 6);
		knots.insert(knots.end(), 2, 7);
		knots.insert(knots.end(), 3, 8);

		k3d::nurbs_curve::add_curve(Mesh, Primitive, 3, control_points, weights, knots, 1);
	}
}

static void parse_circle(const k3d::xml::element& SVG, transform_stack& Transformation, k3d::mesh& Mesh, k3d::nurbs_curve::primitive& Primitive)
{
	const k3d::double_t cx = k3d::xml::attribute_value<k3d::double_t>(SVG, "cx", 0);
	const k3d::double_t cy = k3d::xml::attribute_value<k3d::double_t>(SVG, "cy", 0);
	const k3d::double_t r = k3d::xml::attribute_value<k3d::double_t>(SVG, "r", 1);

	k3d::mesh::knots_t knots;
	k3d::mesh::weights_t weights;
	k3d::mesh::points_t control_points;
	k3d::nurbs_curve::circular_arc(k3d::vector3(r, 0, 0), k3d::vector3(0, r, 0), 0, k3d::pi_times_2(), 4, knots, weights, control_points);

	for(k3d::uint_t i = 0; i != control_points.size(); ++i)
		control_points[i] = Transformation.top() * (control_points[i] + k3d::vector3(cx, cy, 0));

	k3d::nurbs_curve::add_curve(Mesh, Primitive, 3, control_points, weights, knots);
}

static void parse_ellipse(const k3d::xml::element& SVG, transform_stack& Transformation, k3d::mesh& Mesh, k3d::nurbs_curve::primitive& Primitive)
{
	const k3d::double_t cx = k3d::xml::attribute_value<k3d::double_t>(SVG, "cx", 0);
	const k3d::double_t cy = k3d::xml::attribute_value<k3d::double_t>(SVG, "cy", 0);
	const k3d::double_t rx = k3d::xml::attribute_value<k3d::double_t>(SVG, "rx", 1);
	const k3d::double_t ry = k3d::xml::attribute_value<k3d::double_t>(SVG, "ry", 1);

	k3d::mesh::knots_t knots;
	k3d::mesh::weights_t weights;
	k3d::mesh::points_t control_points;
	k3d::nurbs_curve::circular_arc(k3d::vector3(rx, 0, 0), k3d::vector3(0, ry, 0), 0, k3d::pi_times_2(), 4, knots, weights, control_points);

	for(k3d::uint_t i = 0; i != control_points.size(); ++i)
		control_points[i] = Transformation.top() * (control_points[i] + k3d::vector3(cx, cy, 0));

	k3d::nurbs_curve::add_curve(Mesh, Primitive, 3, control_points, weights, knots);
}

static void parse_polyline(const k3d::xml::element& SVG, transform_stack& Transformation, k3d::mesh& Mesh, k3d::nurbs_curve::primitive& Primitive)
{
	k3d::mesh::points_t control_points;
	
	std::istringstream point_stream(k3d::xml::attribute_text(SVG, "points"));
	for(k3d::point2 point; ;)
	{
		point_stream >> point[0];
		point_stream.ignore(1);
		point_stream >> point[1];

		if(!point_stream)
			break;

		control_points.push_back(Transformation.top() * k3d::point3(point[0], point[1], 0));
	}

	if(control_points.size() < 2)
		return;

	k3d::nurbs_curve::add_curve(Mesh, Primitive, 2, control_points);
}

static void parse_polygon(const k3d::xml::element& SVG, transform_stack& Transformation, k3d::mesh& Mesh, k3d::nurbs_curve::primitive& Primitive)
{
	k3d::mesh::points_t control_points;

	std::istringstream point_stream(k3d::xml::attribute_text(SVG, "points"));
	for(k3d::point2 point; ;)
	{
		point_stream >> point[0];
		point_stream.ignore(1);
		point_stream >> point[1];

		if(!point_stream)
			break;

		control_points.push_back(Transformation.top() * k3d::point3(point[0], point[1], 0));
	}

	if(control_points.size() < 2)
		return;

	k3d::nurbs_curve::add_curve(Mesh, Primitive, 2, control_points, 1);
}

/*
static void create_arc(k3d::point4 p1, k3d::point4 p2, k3d::double_t phi, k3d::double_t& cx, k3d::double_t&cy,
		k3d::double_t rx, k3d::double_t ry, bool large_arc_flag, bool sweep_flag, int segments,
		std::vector<k3d::point3>& control_points, k3d::mesh::knots_t& knots, k3d::mesh::weights_t& weights)
{
	k3d::matrix4 T = k3d::identity3();
	k3d::point4 xp, cp, c;
	k3d::vector3 v1, v2;
	k3d::double_t theta_one, theta_two, delta_theta;

	T[0][0] = cos(phi);
	T[0][1] = sin(phi);
	T[1][0] = -sin(phi);
	T[1][1] = cos(phi);

	xp = T * k3d::to_point((p1 - p2) / 2);

	cp = std::sqrt(std::abs(rx * rx * ry * ry - rx * rx * xp[1] * xp[1] - ry * ry * xp[0] * xp[0]) / (rx * rx * xp[1] * xp[1] + ry * ry * xp[0] * xp[0])) * k3d::point4((rx / ry) * xp[1], (-ry / rx) * xp[0], 0, 1);

	if (large_arc_flag == sweep_flag)
		cp = -cp;

	cp[3] = 1;

	T[0][0] = cos(phi);
	T[0][1] = -sin(phi);
	T[1][0] = sin(phi);
	T[1][1] = cos(phi);

	c = T * cp + (p1 + p2) / 2;

	v1 = k3d::vector3(1, 0, 0);
	v2 = k3d::vector3((xp[0] - cp[0]) / rx, (xp[1] - cp[1]) / ry, 0);

	theta_one = acos((v1 * v2) / (k3d::length(v1) * k3d::length(v2)));
	if (v1[0]*v2[1] - v1[1]*v2[0] < 0)
		theta_one = -theta_one;

	v1 = v2;
	v2 = k3d::vector3((-xp[0] - cp[0]) / rx, (-xp[1] - cp[1]) / ry, 0);
	delta_theta = k3d::radians((int)k3d::degrees((acos((v1 * v2) / (k3d::length(v1) * k3d::length(v2))))) % 360);

	if (sweep_flag == large_arc_flag)
		delta_theta = -delta_theta;

	theta_one += k3d::pi_times_2();
	if (large_arc_flag == 0)
		theta_two = theta_one + delta_theta;
	else
		if (sweep_flag == 0)
		{
			theta_two = theta_one + k3d::pi_times_2() - delta_theta;
			rx = -rx;
		}
		else
			theta_two = theta_one + k3d::pi_times_2() +  delta_theta;


	k3d::nurbs_curve::circular_arc(k3d::point3(rx, 0, 0), k3d::point3(0, ry, 0), theta_one, theta_two, segments, knots, weights, control_points);
	if (theta_two < theta_one)
	{
		std::vector<k3d::point3> tmp_cp;
		for (int i = 0; i < control_points.size(); i++)
			tmp_cp.push_back(control_points[i]);
		for (int i = 0; i < control_points.size(); i++)
			control_points[i] = tmp_cp[control_points.size()-1-i];
	}
	cx = c[0];
	cy = c[1];
	for (int i = 0; i < control_points.size(); i++)
	{
		k3d::point4 tmp(control_points[i][0], control_points[i][1], control_points[i][2], 1);
		tmp = T * tmp;
		control_points[i] = k3d::point3(tmp[0], tmp[1], tmp[2]);

	}
}
*/

static void parse_path(const k3d::xml::element& SVG, transform_stack& Transformation, k3d::mesh& Mesh, k3d::nurbs_curve::primitive& Primitive)
{
/*
	int count = 0;
	const k3d::string_t def_path = k3d::xml::attribute_text(SVG, "d");
	char token;
	k3d::double_t x, y;
	int last, first;
	bool is_arc = false;
	//Check if coordinates are relative to each other or absolute to the current
	//coordinate system
	bool relative = false;
	//slastpoint stores the second last point in the path
	k3d::point4 slastpoint;
	//lastpoint stores the last point in the path
	k3d::point4 lastpoint;
	//firstpoint stores the first point in the path
	k3d::point4 firstpoint;

	std::istringstream def_stream(def_path);

	def_stream >> token;

	if (token != 'M')
		k3d::log() << error << "Error parsing path " << k3d::xml::attribute_text(SVG, "id") << " missing start point." << std::endl;

	get_pair(x, y, def_stream);

	//initialize the curve and add first point of curve
	last = first = count;
	count++;
	firstpoint = k3d::point4(x, y, 0, 1);
	lastpoint = k3d::point4(x, y, 0, 1);
	add_point(firstpoint);

	k3d::mesh::indices_t points;
	k3d::mesh::knots_t knots;
	k3d::mesh::weights_t weights;

	knots.push_back(0);
	knots.push_back(0);

	while (!def_stream.eof())
	{
		k3d::uint_t order = 3;

		points.push_back(last);
		def_stream >> token;
		switch (token)
		{
			//Line_to attribute: creates a line from current point to (x,y)
		case 'l':
			relative = true;
		case 'L':
			order = 2;
			slastpoint = k3d::point4(x, y, 0, 1);
			get_pair(x, y, def_stream);
			points.push_back(points.back());
			points.push_back(count);
			points.push_back(count);
			count++;
			if (relative)
			{
				x += lastpoint[0];
				y += lastpoint[1];
			}
			lastpoint = k3d::point4(x, y, 0, 1);
			add_point(lastpoint);
			last = points.back();
			break;
			//Horizontal line: creates a line from current point to (current_x + x , current_y)
		case 'h':
			relative = true;
		case 'H':
			order = 2;
			slastpoint = k3d::point4(x, y, 0, 1);
			def_stream >> x;
			points.push_back(points.back());
			points.push_back(count);
			points.push_back(count);
			count++;
			if (relative)
				x += lastpoint[0];
			lastpoint = k3d::point4(x, lastpoint[1], 0, 1);
			add_point(lastpoint);
			last = points.back();
			break;
			//Vertical line: creates a line from current point to (current_x , current_y + y)
		case 'v':
			relative = true;
		case 'V':
			order = 2;
			slastpoint = k3d::point4(x, y, 0, 1);
			def_stream >> y;
			points.push_back(points.back());
			points.push_back(count);
			points.push_back(count);
			count++;
			if (relative)
				y += lastpoint[1];
			lastpoint = k3d::point4(lastpoint[0], y, 0, 1);
			add_point(lastpoint);
			last = points.back();
			break;
			//Closes the path addint the firstpoint as the last point
		case 'Z':
		case 'z':
			order = 2;
			slastpoint = k3d::point4(x, y, 0, 1);
			lastpoint = firstpoint;
			points.push_back(points.back());
			points.push_back(first);
			points.push_back(first);
			break;
			//Curve: Creates a cubic bezier spline from current point to (x3,y3) using (x1,y1) and (x2,y2)
			//       as control points
		case 'c':
			relative = true;
		case 'C':
			order = 4;
			for (int i = 0; i < 3; i++)
			{
				get_pair(x, y, def_stream);
				points.push_back(count);
				count++;
				if (relative)
				{
					x += lastpoint[0];
					y += lastpoint[1];
				}
				add_point(k3d::point4(x, y, 0, 1));
				if (i == 1)
					slastpoint = k3d::point4(x, y, 0, 1);
			}
			lastpoint = k3d::point4(x, y, 0, 1);
			last = points.back();

			//if(knots.size()==0)
			//	knots.push_back(0);
			//else
			//	knots.push_back(knots.back()+1);
			//for(int i=0; i<2; i++)
			//	knots.push_back(knots.back());
			//knots.push_back(knots.back()+1);
			//for(int i=0; i<2; i++)
			//	knots.push_back(knots.back());
			break;
			//Smooth curve: Creates a cubic bezier spline from current point to (x2,y2)
			//              using slastpoint and (x1 y1) as control points
		case 's':
			relative = true;
		case 'S':
			order = 4;
			points.push_back(count);
			count++;
			add_point(k3d::point4(2*lastpoint[0] - slastpoint[0], 2*lastpoint[1] - slastpoint[1], 0, 1));
			for (int i = 0; i < 2; i++)
			{
				get_pair(x, y, def_stream);
				points.push_back(count);
				count++;
				if (relative)
				{
					x += lastpoint[0];
					y += lastpoint[1];
				}
				add_point(k3d::point4(x, y, 0, 1));
				if (i == 0)
					slastpoint = k3d::point4(x, y, 0, 1);
			}
			lastpoint = k3d::point4(x, y, 0, 1);
			last = points.back();
			break;
			//Quadratic curve: Creates a quadratic bezier spline from current point to (x2,y2)
			//                 using (x1,y1) as the control point
		case 'q':
			relative = true;
		case 'Q':
			order = 3;
			points.push_back(count);
			for (int i = 0; i < 2; i++)
			{
				get_pair(x, y, def_stream);
				points.push_back(count);
				count++;
				if (relative)
				{
					x += lastpoint[0];
					y += lastpoint[1];
				}
				add_point(k3d::point4(x, y, 0, 1));
				if (i == 0)
					slastpoint = k3d::point4(x, y, 0, 1);
			}
			lastpoint = k3d::point4(x, y, 0, 1);
			last = points.back();
			break;
			//Smooth quadratic curve: Creates a cubic bezier spline from current point to
			// 				(x1,y1) using slastpoint as the control point
		case 't':
			relative = true;
		case 'T':
			order = 3;
			points.push_back(count);
			points.push_back(count);
			count++;
			slastpoint = k3d::point4(2 * lastpoint[0] - slastpoint[0], 2 * lastpoint[1] - slastpoint[1], 0, 1);
			add_point(slastpoint);

			get_pair(x, y, def_stream);
			points.push_back(count);
			count++;
			if (relative)
			{
				x += lastpoint[0];
				y += lastpoint[1];
			}
			add_point(k3d::point4(x, y, 0, 1));
			lastpoint = k3d::point4(x, y, 0, 1);
			last = points.back();
			break;
			//Arc: creates an arc from current point to (x,y) with the semiaxes radius given by (rx,ry)
			//     fa and fs: are used to choose from the possible 4 arcs created by the two given points
			//     and the given radius
			//     phi is the angle of the x-axis of the ellipse relative to the current x-axis
		case 'a':
			relative = true;
		case 'A':
			order = 3;
			std::vector<k3d::point3> control_points;
			k3d::mesh::knots_t tmp_knots;
			k3d::mesh::weights_t tmp_weights;
			k3d::double_t cx, cy, rx, ry, phi, fa, fs;
			k3d::point4 p1 = k3d::point4(x, y, 0, 1);
			get_pair(rx, ry, def_stream);
			def_stream >> phi;
			get_pair(fa, fs, def_stream);
			get_pair(x, y, def_stream);
			if (relative)
			{
				x += lastpoint[0];
				y += lastpoint[1];
			}
			create_arc(p1, k3d::point4(x, y, 0, 1), k3d::radians(phi), cx, cy, rx, ry, fa, fs, 8, control_points, knots, tmp_weights);
			return_if_fail(tmp_weights.size() == control_points.size());
			for (int i = 1; i < control_points.size(); i++)
			{
				points.push_back(count);
				count++;
				add_point(k3d::point4(control_points[i][0] + cx, control_points[i][1] + cy, control_points[i][2], 1));
			}
			is_arc = true;
			k3d::point3 t_point = control_points.at(control_points.size() - 2);
			slastpoint = k3d::point4(t_point[0], t_point[1], t_point[2], 1);
			t_point = control_points.back();
			lastpoint = k3d::point4(t_point[0], t_point[1], t_point[2], 1);
			last = points.back();
			break;
		}

		if (!is_arc)
		{
			//Start knot Vector Definition to be a Bezier curve
			//knots.push_back(0);
			//for(int i=0; i < order-1; i++)
			//	knots.push_back(1);
			//for(int i=0; i < order-1; i++)
			//	knots.push_back(2);
			//knots.push_back(3);
			knots.push_back(knots.back());
			knots.push_back(knots.back());
			knots.push_back(knots.back() + 1);
			knots.push_back(knots.back());
			//End knot vector definition

		}
	}

	knots.push_back(knots.back());
	knots.push_back(knots.back());

	for (k3d::uint_t point = 0; point < points.size(); point++)
		weights.push_back(1.0);

	k3d::nurbs_curve::add_curve(order, points, weights, knots, Mesh, Primitive);
	relative = false;
	is_arc = false;
*/
}

/// Main parsing function, doesn't handle grouping and just traverse the XML tree recursively
static void parse_graphics(const k3d::xml::element& SVG, transform_stack& Transformation, k3d::mesh& Mesh, k3d::nurbs_curve::primitive& Primitive)
{
	for(k3d::xml::element::elements_t::const_iterator svg = SVG.children.begin(); svg != SVG.children.end(); ++svg)
	{
		const k3d::string_t trans = k3d::xml::attribute_text(*svg, "transform", "none");
		//If there is a transformation extract matrix from data and push into stack
		if (trans != "none")
		{
			k3d::matrix4 mat = k3d::identity3();
			std::istringstream trans_stream(trans);
			k3d::double_t x, y;
			char token;
			char tmp;
			while (!trans_stream.eof())
			{
				trans_stream >> token;
				k3d::matrix4 tmp_mat = k3d::identity3();
				switch (token)
				{
					//Set up translation matrix
					// (( 1  0 0 0)
					//  ( 0  1 0 0)
					//  ( 0  0 1 0)
					//  (tx ty 0 1))
					case 't':
						trans_stream.ignore(9);
						get_pair(x, y, trans_stream);
						tmp_mat[0][3] = x;
						tmp_mat[1][3] = y;
						break;

					//Set up rotation matrix
					// ((cos(t) -sin(t) 0 0)
					//  (sin(t)  cos(t) 0 0)
					//  (     0       0 1 0)
					//  (     0       0 0 1))
					case 'r':
						trans_stream.ignore(6);
						trans_stream >> x;
						x = k3d::radians(x);
						tmp_mat[0][0] = cos(x);
						tmp_mat[0][1] = -sin(x);
						tmp_mat[1][0] = sin(x);
						tmp_mat[1][1] = cos(x);
						break;

					//Set up scaling matrix
					// ((cx 0 0 0)
					//  (0 cy 0 0)
					//  (0  0 1 0)
					//  (0  0 0 1))
					case 's':
						trans_stream >> token;
						if (token == 'k')
						{
							trans_stream.ignore(2);
							trans_stream >> token;
							trans_stream.ignore();
							trans_stream >> x;
							if (token == 'X')
								tmp_mat[0][1] = tan(k3d::radians(x));
							else
								tmp_mat[1][0] = tan(k3d::radians(x));
						}
						else
						{
							trans_stream.ignore(4);
							trans_stream >> x;
							tmp_mat[0][0] = x;
							tmp_mat[1][1] = x;
							break;
						}
						break;

					//Set up custom matrix
					// ((a00 a01 0 0)
					//  (a10 a11 0 0)
					//  (  0   0 1 0)
					//  (a30 a31 0 1))
					case 'm':
						trans_stream.ignore(6);
						trans_stream >> tmp_mat[0][0];
						trans_stream.ignore();
						trans_stream >> tmp_mat[1][0];
						trans_stream.ignore();
						trans_stream >> tmp_mat[0][1];
						trans_stream.ignore();
						trans_stream >> tmp_mat[1][1];
						trans_stream.ignore();
						trans_stream >> tmp_mat[0][3];
						trans_stream.ignore();
						trans_stream >> tmp_mat[1][3];
						trans_stream.ignore();
				}

				trans_stream.ignore(2);
				mat = mat * tmp_mat;
			}
			//Push the generated matrix into the stack relative to the one in the top
			Transformation.push(Transformation.top() * mat);
		}

		if(svg->name == "g")
			parse_graphics(*svg, Transformation, Mesh, Primitive);
		if(svg->name == "line")
			parse_line(*svg, Transformation, Mesh, Primitive);
		if(svg->name == "rect")
			parse_rect(*svg, Transformation, Mesh, Primitive);
		if(svg->name == "circle")
			parse_circle(*svg, Transformation, Mesh, Primitive);
		if(svg->name == "ellipse")
			parse_ellipse(*svg, Transformation, Mesh, Primitive);
		if(svg->name == "polyline")
			parse_polyline(*svg, Transformation, Mesh, Primitive);
		if(svg->name == "polygon")
			parse_polygon(*svg, Transformation, Mesh, Primitive);
		if(svg->name == "path")
			parse_path(*svg, Transformation, Mesh, Primitive);

		if(trans != "none")
			Transformation.pop();
	}
}

/////////////////////////////////////////////////////////////////////////////
// mesh_reader

class mesh_reader :
	public k3d::mesh_reader<k3d::node >
{
	typedef k3d::mesh_reader<k3d::node > base;

public:
	mesh_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_load_mesh(const k3d::filesystem::path& Path, k3d::mesh& Mesh)
	{
		try
		{
			k3d::filesystem::ifstream svg_stream(Path);
			k3d::xml::element svg;
			svg_stream >> svg;

			if(svg.name != "svg")
				throw std::runtime_error("Not an SVG file.");

			transform_stack transformation;
			transformation.push(k3d::identity3());

			k3d::mesh::points_t& points = Mesh.points.create();
			k3d::mesh::selection_t& point_selection = Mesh.point_selection.create();
			boost::scoped_ptr<k3d::nurbs_curve::primitive> primitive(k3d::nurbs_curve::create(Mesh));

			parse_graphics(svg, transformation, Mesh, *primitive);

			primitive->first_curves.push_back(0);
			primitive->curve_counts.push_back(primitive->curve_first_points.size());
			primitive->materials.push_back(0);
		}
		catch(std::exception& e)
		{
			k3d::log() << error << "Caught exception: " << e.what() << std::endl;
		}
		catch(...)
		{
			k3d::log() << error << "Unknown exception" << std::endl;
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory < mesh_reader,
		k3d::interface_list < k3d::imesh_source,
		k3d::interface_list<k3d::imesh_storage> > > factory(
		  k3d::uuid(0xa0149217, 0xf64844bb, 0x4ba01e8f, 0x21d7f651),
		  "SVGMeshReader",
		  _("Reader that loads external Scalable Vector Graphics (.svg) files into the document by reference"),
		  "MeshReader",
		  k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader::get_factory();
}

} // namespace io

} // namespace svg

} // namespace module

