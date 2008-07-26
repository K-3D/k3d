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
*   \author Carlos Andres Dominguez Caballero
*   (carlosadc@gmail.com)
*/

//#include "gprim_factory.h"
#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/gprim_factory.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/node.h>
#include <k3dsdk/xml.h>
#include <k3dsdk/nurbs.h>
#include <stack>

namespace module
{

namespace svg
{

namespace io
{

/////////////////////////////////////////////////////////////////////////////
// mesh_reader_implementation

class mesh_reader_implementation :
	public k3d::mesh_source<k3d::node >,
	public k3d::imesh_storage
{
	typedef k3d::mesh_source<k3d::node > base;

public:
	mesh_reader_implementation(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("Input file")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("svg_files"))
	{
		m_file.changed_signal().connect(make_topology_changed_slot());
		
		count = 0;
	}

	void reset_mesh(k3d::mesh* const Mesh)
	{
		m_output_mesh.reset(Mesh);
	}

	void on_create_mesh_topology(k3d::mesh& Mesh)
	{
		const k3d::filesystem::path svg_path = m_file.pipeline_value();
		if(svg_path.empty())
			return;

		k3d::xml::element xml_svg;

		try 
		{
			k3d::log() << info << "Reading SVG file from " << svg_path.native_console_string() << std::endl;
			k3d::filesystem::ifstream svg_stream(svg_path);
			svg_stream >> xml_svg;
			assert_warning(xml_svg.name == "svg");
		}
		catch(...)
		{
			k3d::log() << error << "Error reading SVG file from " << svg_path.native_console_string() << std::endl;
		}

		factory = new k3d::gprim_factory(Mesh);
		count = 0; 

        mstack.push(k3d::identity3D());
		parse_graphics(xml_svg);
		delete factory;
	}

	void on_update_mesh_geometry(k3d::mesh& Mesh)
	{
		// add scale factor and scale code for changing its size
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<mesh_reader_implementation,
                k3d::interface_list<k3d::imesh_source,
                k3d::interface_list<k3d::imesh_storage> > > factory(
			k3d::uuid(0xa0149217, 0xf64844bb, 0x4ba01e8f, 0x21d7f651),
			"SVGMeshReader",
			_("Reader that loads external Scalable Vector Graphics (.svg) files into the document by reference"),
			"MeshReader");

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file;
	k3d::gprim_factory *factory;
	///Variable to keep track of the index of the last point added to the gprim_factory
	int count;
	///Matrix stack to handle coordinate system transformations
	std::stack<k3d::matrix4> mstack;

	///Add point to the gprim_factory using the current coordinate system transformation
	void add_point(k3d::point4 p)
	{
		factory->add_point(mstack.top()*p);
	}

	///Main parsing function, doesn't handle grouping and just traverse the XML tree recursively
	void parse_graphics(const k3d::xml::element& xml_root)
	{
		for(k3d::xml::element::elements_t::const_iterator xml_obj = xml_root.children.begin(); xml_obj != xml_root.children.end(); ++xml_obj)
		{
			const std::string trans = k3d::xml::attribute_text(*xml_obj,"transform","none");
			//If there is a transformation extract matrix from data and push into stack
			if(trans!="none")
			{
				k3d::matrix4 mat = k3d::identity3D();
				std::istringstream trans_stream(trans);
				float x,y;
				char token;
				char tmp;
				while(!trans_stream.eof())
				{
					trans_stream >> token;
					k3d::matrix4 tmp_mat = k3d::identity3D();
					switch(token)
					{

					//Set up translation matrix
					// (( 1  0 0 0)
					//  ( 0  1 0 0)
					//  ( 0  0 1 0)
					//  (tx ty 0 1))
					case 't':
						trans_stream.ignore(9);
						get_pair(x,y,trans_stream);
						tmp_mat[0][3]=x;
						tmp_mat[1][3]=y;
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
						if(token=='k')
						{
							trans_stream.ignore(2);
							trans_stream >> token;
							trans_stream.ignore();
							trans_stream >> x;
							if(token=='X')
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
				mstack.push(mstack.top()*mat);
			}

			if(xml_obj->name == "path")
				parse_path(*xml_obj);
			if(xml_obj->name == "rect")
				parse_rect(*xml_obj);
			if(xml_obj->name == "g")
				parse_graphics(*xml_obj);
			if(xml_obj->name == "circle"||xml_obj->name == "ellipse")
				parse_ellipse(*xml_obj);
			if(xml_obj->name == "line")
				parse_line(*xml_obj);
			if(xml_obj->name == "polyline")
				parse_polyline(*xml_obj, false);
			if(xml_obj->name == "polygon")
				parse_polyline(*xml_obj, true);
			if(trans!="none")
				mstack.pop();
		}
	}

	///Creates rectangles (rounded and not rounded corners)
	void parse_rect(const k3d::xml::element& xml_obj)
	{	
		double x, y, w, h, rx, ry;
		k3d::mesh::weights_t weights;
		k3d::mesh::knots_t knots;
		k3d::mesh::indices_t points;
		
		x = k3d::xml::attribute_value<double>(xml_obj, "x", 0);
		y = k3d::xml::attribute_value<double>(xml_obj, "y", 0);
		w = k3d::xml::attribute_value<double>(xml_obj, "width", 1);
		h = k3d::xml::attribute_value<double>(xml_obj, "height", 1);
		rx = k3d::xml::attribute_value<double>(xml_obj, "rx", -1);
		ry = k3d::xml::attribute_value<double>(xml_obj, "ry", -1); 

		//if rx==-1 the rectangle doesn't have rounded corners
		if(rx == -1 && ry == -1)
		{
			for(int i=0; i<4; i++)
			{
				points.push_back(count);
				count++;
				weights.push_back(1);
				knots.push_back(2*i);
				knots.push_back(2*i+1);
			}
			points.push_back(points.front());
			weights.push_back(1);
			knots.push_back(8);
			knots.push_back(9);
			add_point(k3d::point4(x,y,0,1));
			add_point(k3d::point4(x+w,y,0,1));
			add_point(k3d::point4(x+w,y+h,0,1));
			add_point(k3d::point4(x,y+h,0,1));
			factory->add_nurbs_curve(2,points,knots,weights);
		}
		else
		{
			if(ry == -1)
				ry = rx;
			else if (rx == -1)
				rx = ry;
			
			std::vector<double> tmp_weights;
			std::vector<k3d::point3> control_points;

			//creates upper left corner as a circular_arc and then mirror
			//the points to generate the other corners to save computations
			k3d::mesh::knots_t tmp_knots;
			k3d::nurbs::circular_arc(k3d::point3(-rx, 0, 0), k3d::point3(0, -ry, 0), 0, k3d::pi()/2, 1, tmp_knots, tmp_weights, control_points);
			return_if_fail(tmp_weights.size() == control_points.size());

			//Upper left corner
			for(int i=0; i<control_points.size(); i++)
			{
				points.push_back(count);
				count++;
				add_point(k3d::point4(control_points[i][0]+x+rx, control_points[i][1]+y+ry, control_points[i][2],1));
				weights.push_back(tmp_weights[i]);
			}

			//Mirroring constants
			double tmx = 2*x+w;
			double tmy = 2*y+h;

			points.push_back(points.back());
			weights.push_back(1);

			//Upper right corner
			for(int i=control_points.size()-1; i>=0; i--)
			{
				points.push_back(count);
				count++;
				add_point(k3d::point4(tmx - (control_points[i][0]+x+rx), control_points[i][1]+y+ry,control_points[i][2],1));
				weights.push_back(tmp_weights[i]);
			}

			points.push_back(points.back());
			weights.push_back(1);

			//Lower right corner
			for(int i=0; i<control_points.size(); i++)
			{
				points.push_back(count);
				count++;
				add_point(k3d::point4(tmx - (control_points[i][0]+x+rx), tmy - (control_points[i][1]+y+ry),control_points[i][2],1));
				weights.push_back(tmp_weights[i]);
			}

			points.push_back(points.back());
			weights.push_back(1);

			//Lower left corner
			for(int i=control_points.size()-1; i>=0; i--)
			{
				points.push_back(count);
				count++;
				add_point(k3d::point4(control_points[i][0]+x+rx, tmy - (control_points[i][1]+y+ry),control_points[i][2],1));
				weights.push_back(tmp_weights[i]);
			}

			points.push_back(points.back());
			weights.push_back(1);

			points.push_back(points.front());
			weights.push_back(1);

			for(int i=0; i<3; i++)
				knots.push_back(0);
			for(int i=1; i<=7; i++)
				for(int j=0; j<2; j++)
					knots.push_back(i);
			for(int i=0; i<3; i++)
				knots.push_back(8);

			factory->add_nurbs_curve(3, points, knots, weights);
		}

	
	}

	///Creates ellipses and circles
    void parse_ellipse(const k3d::xml::element& xml_obj)
    {
        double cx, cy, rx, ry;
        cx = k3d::xml::attribute_value<double>(xml_obj, "cx", 0);
        cy = k3d::xml::attribute_value<double>(xml_obj, "cy", 0);

			rx = ry = k3d::xml::attribute_value<double>(xml_obj, "r", -1);

		//if r==-1 this is not a circle so it is an ellipse
        if(rx == -1)
        {
            rx = k3d::xml::attribute_value<double>(xml_obj, "rx", 1);
            ry = k3d::xml::attribute_value<double>(xml_obj, "ry", 1);
        }

		std::vector<double> tmp_weights;
		std::vector<k3d::point3> control_points;
		k3d::mesh::indices_t points;
		k3d::mesh::knots_t knots;
		k3d::mesh::weights_t weights;
		k3d::nurbs::circular_arc(k3d::point3(rx, 0, 0), k3d::point3(0, ry, 0), 0, k3d::pi_times_2(), 4, knots, tmp_weights, control_points);
		return_if_fail(tmp_weights.size() == control_points.size());

		for(int i=0; i<control_points.size(); i++)
		{
				points.push_back(count);
				count++;
				add_point(k3d::point4(control_points[i][0]+cx, control_points[i][1]+cy, control_points[i][2],1));
				weights.push_back(tmp_weights[i]);
		}

		factory->add_nurbs_curve(3, points, knots, weights);
	}

	///Creates line segments
    void parse_line(const k3d::xml::element& xml_obj)
    {
        double x1,x2,y1,y2;
        x1 = k3d::xml::attribute_value<double>(xml_obj, "x1", 0);
        y1 = k3d::xml::attribute_value<double>(xml_obj, "y1", 0);
        x2 = k3d::xml::attribute_value<double>(xml_obj, "x2", 1);
        y2 = k3d::xml::attribute_value<double>(xml_obj, "y2", 0);

		k3d::mesh::indices_t points;
		k3d::mesh::knots_t knots;
		k3d::mesh::weights_t weights;

		for(int i=0; i<2; i++)
		{
				points.push_back(count);
				count++;
				weights.push_back(1);
				knots.push_back(i);
				knots.push_back(i);
		}
		add_point(k3d::point4(x1,y1,0,1));
		add_point(k3d::point4(x2,y2,0,1));

		factory->add_nurbs_curve(2,points,knots,weights);

	}

	//Helper function for get_pair to parse a pair of numbers and check if they
	//are comma separated or space separated
    bool is_together(const std::string& arg)
	{
		if(arg.find(",",1)>arg.length())
			return false;
		return true;
	}

	//Helper function for parse_path to extract a pair (x,y) regardless of the
	//Format i.e. "x,y" or "x y"
	void get_pair(float& x, float& y, std::istringstream& def_stream)
	{
		std::string arg;
		def_stream >> arg;
		if(is_together(arg))
			sscanf(arg.c_str(), "%f,%f", &x, &y);
		else
		{
			x = atof(arg.c_str());
			def_stream >> y;
		}
	}

	//Creates polygonal lines
    void parse_polyline(const k3d::xml::element& xml_obj, bool is_closed)
    {
        const std::string str_points = k3d::xml::attribute_text(xml_obj, "points");
        std::istringstream def_stream(str_points);
        float x, y;

		k3d::mesh::indices_t points;
		k3d::mesh::knots_t knots;
		k3d::mesh::weights_t weights;

		int i=0;
		int first = count;
		while(!def_stream.eof())
		{
				get_pair(x,y,def_stream);
				points.push_back(count);
				count++;
				add_point(k3d::point4(x,y,0,1));
				weights.push_back(1);
				i++;
		}
		if(is_closed)
		{
				points.push_back(first);
				weights.push_back(1);
		}

		knots.push_back(0);
		for(int i=0; i<points.size();i++)
				knots.push_back(i+1);
		knots.push_back(knots.back());

		factory->add_nurbs_curve(2,points,knots,weights);
	}

	void create_arc(k3d::point4 p1, k3d::point4 p2, float phi, float& cx, float&cy,
					float rx, float ry, bool large_arc_flag, bool sweep_flag, int segments,
					std::vector<k3d::point3>& control_points, k3d::mesh::knots_t& knots, k3d::mesh::weights_t& weights)
	{
		k3d::matrix4 T = k3d::identity3D();
		k3d::point4 xp, cp, c;
		k3d::vector3 v1, v2;
		float theta_one, theta_two, delta_theta;

		T[0][0] = cos(phi);
		T[0][1] = sin(phi);
		T[1][0] = -sin(phi);
		T[1][1] = cos(phi);

		xp = T*((p1-p2)/2);

		cp = std::sqrt(std::abs(rx*rx*ry*ry - rx*rx*xp[1]*xp[1] - ry*ry*xp[0]*xp[0])/(rx*rx*xp[1]*xp[1] + ry*ry*xp[0]*xp[0]))*k3d::point4((rx/ry)*xp[1], (-ry/rx)*xp[0], 0, 1);
		
		if(large_arc_flag==sweep_flag)
			cp = -cp;

		cp[3]=1;

		T[0][0] = cos(phi);
		T[0][1] = -sin(phi);
		T[1][0] = sin(phi);
		T[1][1] = cos(phi);

		c = T*cp + (p1+p2)/2;

		v1 = k3d::vector3(1,0,0);
		v2 = k3d::vector3((xp[0]-cp[0])/rx, (xp[1]-cp[1])/ry,0);

		theta_one = acos((v1*v2)/(k3d::length(v1)*k3d::length(v2)));
		if(v1[0]*v2[1]-v1[1]*v2[0]<0)
			theta_one = -theta_one;

		v1 = v2;
		v2 = k3d::vector3((-xp[0]-cp[0])/rx, (-xp[1]-cp[1])/ry,0);
		delta_theta = k3d::radians((int)k3d::degrees((acos((v1*v2)/(k3d::length(v1)*k3d::length(v2)))))%360);
	
		if(sweep_flag == large_arc_flag)
			delta_theta = -delta_theta;

		theta_one += k3d::pi_times_2();
		if(large_arc_flag == 0)
			theta_two = theta_one + delta_theta;
		else
			if(sweep_flag == 0)
			{
				theta_two = theta_one + k3d::pi_times_2() - delta_theta;
				rx = -rx;
			}
			else
				theta_two = theta_one + k3d::pi_times_2() +  delta_theta;


		k3d::nurbs::circular_arc(k3d::point3(rx, 0, 0), k3d::point3(0, ry, 0), theta_one, theta_two, segments, knots, weights, control_points);
		if(theta_two < theta_one)
		{
			std::vector<k3d::point3> tmp_cp;
			for(int i=0; i<control_points.size(); i++)
				tmp_cp.push_back(control_points[i]);
			for(int i=0; i<control_points.size(); i++)
				control_points[i] = tmp_cp[control_points.size()-1-i];
		}
		cx = c[0];
		cy = c[1];
		for(int i=0; i<control_points.size(); i++)
		{
			k3d::point4 tmp(control_points[i][0],control_points[i][1],control_points[i][2],1);
			tmp = T*tmp;
			control_points[i] = k3d::point3(tmp[0],tmp[1],tmp[2]);
			
		}
	}

	///Creates paths composed by either lines, cubic or quadratic bezier curves or arcs
	void parse_path(const k3d::xml::element& xml_obj)
	{
		const std::string def_path = k3d::xml::attribute_text(xml_obj, "d");
		char token;
		float x, y;
		int last, first;
		bool is_arc = false;
		//Check if coordinates are relative to each other or absolute to the current
		//coordinate system
		bool relative=false;
		//slastpoint stores the second last point in the path
		k3d::point4 slastpoint;
		//lastpoint stores the last point in the path
		k3d::point4 lastpoint;
		//firstpoint stores the first point in the path
		k3d::point4 firstpoint;

		std::istringstream def_stream(def_path);

		def_stream >> token;

		if(token != 'M')
			k3d::log() << error << "Error parsing path " << k3d::xml::attribute_text(xml_obj, "id") << " missing start point." << std::endl;

		get_pair(x,y,def_stream);

		//initialize the curve and add first point of curve
		last = first = count;
		count++;
		firstpoint = k3d::point4(x,y,0,1);
		lastpoint = k3d::point4(x,y,0,1);
		add_point(firstpoint);

		k3d::mesh::indices_t points;
		k3d::mesh::knots_t knots;
		k3d::mesh::weights_t weights;

		knots.push_back(0);
		knots.push_back(0);

		while(!def_stream.eof())
		{
			k3d::uint_t order = 3;
			
			points.push_back(last);
			def_stream >> token;
			switch(token)
			{
			//Line_to attribute: creates a line from current point to (x,y)
			case 'l':
				relative = true;
			case 'L':
				order = 2;
				slastpoint = k3d::point4(x,y,0,1);
				get_pair(x,y,def_stream);
				points.push_back(points.back());
				points.push_back(count);
				points.push_back(count);
				count++;
				if(relative)
				{
					x+=lastpoint[0];
					y+=lastpoint[1];
				}
				lastpoint = k3d::point4(x,y,0,1);
				add_point(lastpoint);
				last = points.back();
				break;
			//Horizontal line: creates a line from current point to (current_x + x , current_y)
			case 'h':
				relative = true;
			case 'H':
				order = 2;
				slastpoint = k3d::point4(x,y,0,1);
				def_stream >> x;
				points.push_back(points.back());
				points.push_back(count);
				points.push_back(count);
				count++;
				if(relative)
					x+=lastpoint[0];
				lastpoint = k3d::point4(x,lastpoint[1],0,1);
				add_point(lastpoint);
				last = points.back();
				break;
			//Vertical line: creates a line from current point to (current_x , current_y + y)
			case 'v':
				relative = true;
			case 'V':
				order = 2;
				slastpoint = k3d::point4(x,y,0,1);
				def_stream >> y;
				points.push_back(points.back());
				points.push_back(count);
				points.push_back(count);
				count++;
				if(relative)
					y+=lastpoint[1];
				lastpoint = k3d::point4(lastpoint[0], y, 0, 1);
				add_point(lastpoint);
				last = points.back();
				break;
			//Closes the path addint the firstpoint as the last point
			case 'Z':
			case 'z':
				order = 2;
				slastpoint = k3d::point4(x,y,0,1);
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
				for(int i=0; i<3; i++)
				{
					get_pair(x,y,def_stream);
					points.push_back(count);
					count++;
					if(relative)
					{
						x += lastpoint[0];
						y += lastpoint[1];
					}
					add_point(k3d::point4(x,y,0,1));
					if(i==1)
						slastpoint = k3d::point4(x,y,0,1);
				}
				lastpoint = k3d::point4(x,y,0,1);
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
				add_point(k3d::point4(2*lastpoint[0]-slastpoint[0], 2*lastpoint[1]-slastpoint[1],0,1));
				for(int i=0; i<2; i++)
				{
					get_pair(x,y,def_stream);
					points.push_back(count);
					count++;
					if(relative)
					{
						x+=lastpoint[0];
						y+=lastpoint[1];
					}
					add_point(k3d::point4(x,y,0,1));
					if(i==0)
						slastpoint = k3d::point4(x,y,0,1);
				}
				lastpoint = k3d::point4(x,y,0,1);
				last = points.back();
				break;
			//Quadratic curve: Creates a quadratic bezier spline from current point to (x2,y2)
			//                 using (x1,y1) as the control point
			case 'q':
				relative = true;
			case 'Q':
				order = 3;
				points.push_back(count);
				for(int i=0; i<2; i++)
				{
					get_pair(x,y,def_stream);
					points.push_back(count);
					count++;
					if(relative)
					{
						x += lastpoint[0];
						y += lastpoint[1];
					}
					add_point(k3d::point4(x,y,0,1));
					if(i==0)
						slastpoint = k3d::point4(x,y,0,1);
				}
				lastpoint = k3d::point4(x,y,0,1);
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
				slastpoint =k3d::point4(2*lastpoint[0]-slastpoint[0], 2*lastpoint[1]-slastpoint[1],0,1);
				add_point(slastpoint);

				get_pair(x,y,def_stream);
				points.push_back(count);
				count++;
				if(relative)
				{
					x+=lastpoint[0];
					y+=lastpoint[1];
				}
				add_point(k3d::point4(x,y,0,1));
				lastpoint = k3d::point4(x,y,0,1);
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
				float cx, cy,rx, ry, phi, fa, fs;
				k3d::point4 p1 = k3d::point4(x,y,0,1);
				get_pair(rx,ry,def_stream);
				def_stream >> phi;
				get_pair(fa,fs,def_stream);
				get_pair(x,y,def_stream);
				if(relative)
				{
					x+=lastpoint[0];
					y+=lastpoint[1];
				}
				create_arc(p1,k3d::point4(x,y,0,1),k3d::radians(phi),cx,cy,rx,ry,fa,fs,8,control_points,knots,tmp_weights);
				return_if_fail(tmp_weights.size() == control_points.size());
				for(int i=1; i<control_points.size(); i++)
				{
					points.push_back(count);
					count++;
					add_point(k3d::point4(control_points[i][0]+cx, control_points[i][1]+cy, control_points[i][2],1));
				}
				is_arc = true;
				k3d::point3 t_point = control_points.at(control_points.size()-2);
				slastpoint = k3d::point4(t_point[0], t_point[1], t_point[2],1);
				t_point = control_points.back();
				lastpoint = k3d::point4(t_point[0], t_point[1], t_point[2],1);
				last = points.back();
				break;
			}
			
			if(!is_arc)
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
				knots.push_back(knots.back()+1);
				knots.push_back(knots.back());
				//End knot vector definition

			}
		}

 		knots.push_back(knots.back());
		knots.push_back(knots.back());

		for(k3d::uint_t point = 0; point < points.size(); point++ )
			weights.push_back(1.0);

		factory->add_nurbs_curve(4, points, knots, weights);
		relative = false;
		is_arc = false;

	}

};

k3d::iplugin_factory& mesh_reader_factory()
{
	return mesh_reader_implementation::get_factory();
}

} // namespace io

} // namespace svg

} // namespace module

