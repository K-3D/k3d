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
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/color.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/share.h>
#include <k3dsdk/string_modifiers.h>

#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <stack>

namespace module
{

namespace lsystem
{

namespace lparser
{

// Based on free code from Laurens Lapre
// http://home.wanadoo.nl/laurens.lapre/lparser.htm
// for more information, read share/doc/lsystem.txt


// Rules
std::vector<std::string> rules;

// Marked rules need special processing when growing shapes are active
std::vector<bool> marks;

// Used to build the production string
std::string object_string;

// Settings stack used for solving [] references
typedef struct s_rec
{
	k3d::point3	pos;	// position in 3space of turtle origin
	k3d::vector3	fow;	// forward direction

	k3d::vector3	lef;	// left direction
	k3d::vector3	upp;	// up direction
	k3d::point3	last;	// last position used for connecting cylinders
	k3d::point3	last_v[8];	// last vertices of object used for connecting cylinders
	double		dis;	// value of F distance
	double		ang;	// value of basic angle
	double		thick;	// value of thickness
	double		dis2;	// value of Z distance
	double		tr;	// trope value
	unsigned long	col;	// current color
	unsigned long	last_col;	// color of last object
} s_rec;

// Flags
bool trope_set = false;
bool rand_set = false;
bool closed_form = false;
bool last_recur = false; // processing the last recursion step

// Init vars
double		thick, min_thick = 0.0, rand_amount = 0.0;
double		trope_amount = 0.0;
unsigned long	polcount = 0;
// Limit total generated polygons
unsigned long	poly_limit = 500000L;
// Set maximum production string size in kbytes
unsigned long	max_string = 2L * 1024L * 1024L;
unsigned long	col = 2, lev, last_col = 0;
double		dis, ang, dis2, tr = 0.2;
k3d::vector3	sky(0.0, 0.0, 1.0);
k3d::vector3	trope;
k3d::point3	last(1.0, 1.0, 1.0), last_v[8];
double		fraction;
k3d::point3	axis_x, axis_y, axis_z;

// Stacks []
std::stack<s_rec> stack;
s_rec save;

// Polygon stack used for solving {} references
typedef std::vector<k3d::point3> vectors_t;
std::stack<vectors_t> pstack;

// Max size of the [] and {} stacks during drawing
// useful during mutation
unsigned long	max_stack_size = 1000;

// Polygons
class polygon
{
public:
	unsigned long a;
	unsigned long b;
	unsigned long c;
	unsigned long d;

	polygon()
	{
		a = b = c = d = 0;
	}

	polygon(unsigned long A, unsigned long B, unsigned long C, unsigned long D) :
		a(A), b(B), c(C), d(D)
	{
	}
};


// Store vertices and polygons
vectors_t vertices;
typedef std::vector<polygon> polygons_t;
polygons_t polygons;
k3d::signed_axis orientation;
bool flip_normals;

// Bounding-box
k3d::bounding_box3 bounding_box;
double bbox_x = 0;
double bbox_y = 0;
double bbox_z = 0;

// LViewer colors
const unsigned long color_number = 15;
k3d::color colors[color_number] = {
	k3d::color(0.5, 0.5, 0.5), // default
	k3d::color(0.3, 0.3, 0.3),
	k3d::color(0.8, 0.4, 0.4),
	k3d::color(0.8, 0.8, 0.4),
	k3d::color(0.4, 0.8, 0.4),
	k3d::color(0.4, 0.8, 0.8),
	k3d::color(0.4, 0.4, 0.8),
	k3d::color(0.8, 0.4, 0.8),
	k3d::color(0.2, 0.5, 0.2),
	k3d::color(0.2, 0.5, 0.5),
	k3d::color(0.2, 0.2, 0.5),
	k3d::color(0.5, 0.2, 0.5),
	k3d::color(0.6, 0.2, 0.2),
	k3d::color(0.5, 0.5, 0.5),
	k3d::color(0.9, 0.9, 0.9)
};

// Convert an object to a k3d::legacy::mesh
bool add_geometry(unsigned long Color, k3d::legacy::mesh* mesh, k3d::imaterial* const Material)
{
	std::vector<k3d::legacy::point*> points;

	for(unsigned long t = 0; t < vertices.size(); t++)
	{
		// Update orientation and change handedness
		double x = 0;
		double y = 0;
		double z = 0;
		switch(orientation)
		{
			case k3d::PX:
				x = vertices[t][2];
				y = -vertices[t][1];
				z = vertices[t][0];
				break;
			case k3d::NX:
				x = -vertices[t][2];
				y = -vertices[t][1];
				z = -vertices[t][0];
				break;
			case k3d::PY:
				x = -vertices[t][0];
				y = vertices[t][2];
				z = vertices[t][1];
				break;
			case k3d::NY:
				x = vertices[t][0];
				y = -vertices[t][2];
				z = vertices[t][1];
				break;
			case k3d::PZ:
				x = -vertices[t][0];
				y = -vertices[t][1];
				z = vertices[t][2];
				break;
			case k3d::NZ:
				x = vertices[t][0];
				y = -vertices[t][1];
				z = -vertices[t][2];
				break;
		}

		k3d::legacy::point* point = new k3d::legacy::point(x, y, z);
		points.push_back(point);
		mesh->points.push_back(point);

		bounding_box.insert(point->position);
	}

	for(unsigned long t = 0; t < polygons.size(); t++)
	{
		std::vector<k3d::legacy::split_edge*> edges;
		edges.push_back(new k3d::legacy::split_edge(points[polygons[t].a]));
		edges.push_back(new k3d::legacy::split_edge(points[polygons[t].b]));
		edges.push_back(new k3d::legacy::split_edge(points[polygons[t].c]));

		if(polygons[t].c != polygons[t].d)
			edges.push_back(new k3d::legacy::split_edge(points[polygons[t].d]));

		if(flip_normals)
			std::reverse(edges.begin(), edges.end());
		k3d::legacy::loop_edges(edges.begin(), edges.end());
		k3d::legacy::polyhedron& polyhedron = *mesh->polyhedra.back();

		k3d::legacy::face* new_face = new k3d::legacy::face(edges.front(), Material);
		return_val_if_fail(new_face, false);
		polyhedron.faces.push_back(new_face);

		// Set color ...
		unsigned long color_index = Color;
		if(color_index > color_number - 1)
			color_index = 0;

		new_face->uniform_data["Cs"] = colors[color_index];
	}

	return true;
}


// Here we build a cube shape directly on the input vectors
void add_cube(k3d::point3 start, k3d::point3 end, k3d::vector3 up, unsigned long color, k3d::legacy::mesh* mesh, k3d::imaterial* const Material, const k3d::signed_axis Orientation)
{
	// Check size
	k3d::vector3 direction = end - start;
	double length = direction.length();
	if(length == 0)
		return;

	double s = length * thick;
	s = std::max(s, min_thick);
	s *= 0.5;

	k3d::vector3 d1 = k3d::normalize(direction);
	k3d::vector3 d2 = k3d::normalize(up);

	k3d::vector3 d3 = k3d::normalize(d1 ^ d2);

	vertices.clear();
	vertices.resize(4);

	// Base 1, 3
	d1 = k3d::normalize(d2 + d3);
	vertices[0] = start + s * d1;
	vertices[2] = start + (-s) * d1;

	// Base 2, 4
	d1 = k3d::normalize(d2 - d3);
	vertices[1] = start + s * d1;
	vertices[3] = start + (-s) * d1;

	// Top
	for(unsigned long i = 0; i < 4; i++)
		vertices.push_back(vertices[i] + direction);

	// Polygons
	polygons.clear();
	polygons.push_back(polygon(0, 4, 5, 1));
	polygons.push_back(polygon(1, 5, 6, 2));
	polygons.push_back(polygon(2, 6, 7, 3));
	polygons.push_back(polygon(3, 7, 4, 0));
	polygons.push_back(polygon(0, 1, 2, 3));
	polygons.push_back(polygon(7, 6, 5, 4));

	add_geometry(color, mesh, Material);
}

// The lastxxx vars are used to store the previous top of the cylinder
// for connecting a next one; since the vars are stacked for [] we can
// connect correctly according to current nesting level
void add_cylinder(k3d::point3 start, k3d::point3 end, k3d::vector3 up, unsigned long color, k3d::legacy::mesh* mesh, k3d::imaterial* const Material, const k3d::signed_axis Orientation)
{
	// Check size
	k3d::vector3 direction = end - start;
	double length = direction.length();
	if(length == 0.0)
		return;

	double s = length * thick;
	s = std::max(s, min_thick);
	s *= 0.5;

	k3d::vector3 d1 = k3d::normalize(direction);
	k3d::vector3 d2 = k3d::normalize(up);

	k3d::vector3 d3 = k3d::normalize(d1 ^ d2);

	k3d::point3 t1 = k3d::to_point(k3d::normalize(d2 + d3));
	k3d::point3 t2 = k3d::to_point(k3d::normalize(d2 - d3));

	vertices.clear();
	vertices.resize(8);

	vertices[0] = start + s * t1;
	vertices[4] = start + (-s) * t1;
	vertices[2] = start + s * t2;
	vertices[6] = start + (-s) * t2;

	s *= 0.7071;
	vertices[1] = start + s * t1 + s * t2;
	vertices[3] = start + (-s) * t1 + s * t2;
	vertices[5] = start + (-s) * t1 + (-s) * t2;
	vertices[7] = start + s * t1 + (-s) * t2;

	// Top
	for(unsigned long i = 0; i < 8; i++)
		vertices.push_back(vertices[i] + direction);

	if(last_col == color)
	{
		direction = start - last;
		length = direction.length();
		double dd = std::numeric_limits<double>::max();

		// Connect cylinders if near enough
		if(length < 1.0)
		{
			// Find nearest vertex
			unsigned long ii = 0;
			for(unsigned long i = 0; i < 8; i++)
			{
				direction = vertices[0] - last_v[i];
				length = direction.length();
				if(length < dd)
				{
					dd = length;
					ii = i;
				}
			}

			for(unsigned long i = 0; i < 8; i++)
			{
				vertices[i] = last_v[ii];
				ii = (ii + 1) % 8;
			}
		}
	}

	// Polygons
	polygons.clear();
	polygons.push_back(polygon(0, 8, 9, 1));
	polygons.push_back(polygon(1, 9, 10, 2));
	polygons.push_back(polygon(2, 10, 11, 3));
	polygons.push_back(polygon(3, 11, 12, 4));
	polygons.push_back(polygon(4, 12, 13, 5));
	polygons.push_back(polygon(5, 13, 14, 6));
	polygons.push_back(polygon(6, 14, 15, 7));
	polygons.push_back(polygon(7, 15, 8, 0));

	add_geometry(color, mesh, Material);

	// Save cylinder's parameters and top vertices
	last_col = color;
	last = end;
	for(unsigned long i = 0; i < 8; i++)
		last_v[i] = vertices[i + 8];
}

// L-system routines ------------------------------------------------------

// Get a line from a .ls file;
// skips comments (lines beginning with '#') and empty lines
bool ls_line(std::istream& file, std::string& linebuffer)
{
	while(!file.eof())
	{
		k3d::getline(file, linebuffer);

		// Skip comments ...
		if(linebuffer[0] == '#')
			continue;

		// Skip blank lines
		if(!(k3d::trim(linebuffer)).size())
			continue;

		// Must qualify
		return true;
	}

	// end-of-file reached
	return false;
}

bool load_configuration_values(const k3d::filesystem::path& file_path, double& recursion, double& basic_angle, double& thickness)
{
	// Open configuration file
	k3d::filesystem::ifstream file(file_path);
	if(!file.good())
	{
		k3d::log() << error << k3d_file_reference << ": error opening [" << file_path.native_console_string() << "]" << std::endl;
		return 0.0;
	}

	// Get recursion level
	std::string temp;
	return_val_if_fail(ls_line(file, temp), false);
	std::stringstream scan(temp);
	scan >> recursion;

	// Get asic angle
	return_val_if_fail(ls_line(file, temp), false);
	std::stringstream scan2(temp);
	scan2 >> basic_angle;

	// Get thickness
	return_val_if_fail(ls_line(file, temp), false);
	std::stringstream scan3(temp);
	scan3 >> thickness;

	return true;
}

// Process a ls file and setup rules
bool load_configuration_rules(const double recursion, const double basic_angle, const double thickness, const k3d::filesystem::path& file_path)
{
	// Open grammar file
	k3d::filesystem::ifstream file(file_path);
	if(!file.good())
	{
		k3d::log() << error << k3d_file_reference << ": error opening [" << file_path.native_console_string() << "]" << std::endl;
		return false;
	}

	// Set default values
	trope_set = false;
	rand_set = false;
	closed_form = false;
	last_recur = false;

	min_thick = 0.0; rand_amount = 0.0;
	trope_amount = 0.0;
	polcount = 0;
	poly_limit = 500000L;
	col = 2; last_col = 0;
	tr = 0.2;
	sky = k3d::vector3(0.0, 0.0, 1.0);
	last = k3d::point3(1.0, 1.0, 1.0);

	// Clear stacks
	stack = std::stack<s_rec>();
	pstack = std::stack<vectors_t>();

	// Skip but setup recursion level, basic angle and thickness
	std::string temp;
	return_val_if_fail(ls_line(file, temp), false);

	lev = (unsigned long)std::floor(recursion);
	fraction = recursion - (double)lev;
	if(fraction > 0)
		lev++;

	return_val_if_fail(ls_line(file, temp), false);
	ang = basic_angle / 180 * 3.141592654;

	return_val_if_fail(ls_line(file, temp), false);
	thick = thickness / 100;

	// Axiom
	return_val_if_fail(ls_line(file, temp), false);

	object_string = strtok(const_cast<char*>(temp.c_str()), " \r\n\t#");

	// Get rules
	rules.clear();
	for(unsigned long i = 0; i < 150; i++)
	{
		return_val_if_fail(ls_line(file, temp), false);

		std::string rule = strtok(const_cast<char*>(temp.c_str()), " \r\n\t#");

		if(!rule.size())
			continue;

		if(rule[0] == '@')
			break;

		rules.push_back(rule);
	}

	// Add default rules
	rules.push_back("+=+");
	rules.push_back("-=-");
	rules.push_back("&=&");
	rules.push_back("^=^");
	rules.push_back("<=<");
	rules.push_back(">=>");

	rules.push_back("%=%");
	rules.push_back("|=|");
	rules.push_back("!=!");
	rules.push_back("?=?");
	rules.push_back(":=:");
	rules.push_back(";=;");
	rules.push_back("\'=\'");
	rules.push_back("\"=\"");
	rules.push_back("c=c");

	rules.push_back("[=[");
	rules.push_back("]=]");
	rules.push_back("{={");
	rules.push_back("}=}");

	rules.push_back("F=F");
	rules.push_back("f=f");
	rules.push_back("t=t");
	rules.push_back("g=g");
	rules.push_back("Z=Z");
	rules.push_back("z=z");
	rules.push_back("*=*");
	rules.push_back("$=$");
	rules.push_back("~=~");

	rules.push_back(".=.");
	rules.push_back("1=1");
	rules.push_back("2=2");
	rules.push_back("3=3");
	rules.push_back("4=4");
	rules.push_back("5=5");
	rules.push_back("6=6");
	rules.push_back("7=7");
	rules.push_back("8=8");
	rules.push_back("9=9");
	rules.push_back("0=0");
	rules.push_back("(=(");
	rules.push_back(")=)");

	// Closer default
	rules.push_back("_=_");

	// Set start values for F and Z distances
	dis = 100.0;
	dis2 = dis * 0.5;

	// Get marks
	marks.clear();
	for(unsigned long n = 0; n < rules.size(); n++)
		marks.push_back(false);

	// Check which rules need to be marked for last recursion when growing
	for(unsigned long n = 0; n < rules.size(); n++)
	{
		if(rules[n][0] == '+')
			break;

		marks[n] = true;

		// All rules with basic move/block before '=' mark false
		if(rules[n][0] == 'F')
			marks[n] = false;
		if(rules[n][0] == 'f')
			marks[n] = false;
		if(rules[n][0] == 'Z')
			marks[n] = false;
		if(rules[n][0] == 'z')
			marks[n] = false;
	}

	return true;
}

// Apply mutations to the rules
const double inv_max = 1.0 / RAND_MAX;
double Rnd()
{
	return rand() * inv_max;
}

void L_mutate()
{
	unsigned long n;
	for(n = 0; n < rules.size(); n++)
		if(rules[n][0] == '+')
			break;

	double rules_n = static_cast<double>(n);
	const unsigned long max = 1000;

	unsigned long i = static_cast<unsigned long>(Rnd() * 6.0);
	switch(i)
	{
		default:
			return;

		// Insert
		case 1:
		{
			std::string T("");
			i = static_cast<unsigned long>(Rnd() * rules_n);
			T = rules[i][0];

			unsigned long j = static_cast<unsigned long>(Rnd() * rules_n);

			unsigned long k = (unsigned long)(Rnd() * (double)rules[j].length());
			k = (k < 2) ? 2 : k;
			std::string rulet = std::string(rules[j], k);
			rules[j].replace(k, rulet.length(), '[' + T + ']');
			rules[j].replace(k, rulet.length(), std::string('[' + T + ']'));

			rules[j] += rulet;
		}
		break;

		// Replace
		case 0:
		case 2:
		{
			std::string R("");
			std::string T("");

			do
			{
				unsigned long i = static_cast<unsigned long>(Rnd() * rules_n);
				unsigned long j = static_cast<unsigned long>(Rnd() * rules_n);
				T = rules[i][0];
				R = rules[j][0];
			}
			while(T == R);

			for(unsigned long ii = 0; ii < max; ii++)
			{
				i = static_cast<unsigned long>(Rnd() * rules_n);
				for(unsigned long j = 2; j < (rules[i]).length(); j++)
				{
					if(rules[i][j] == T[0])
					{
						rules[i][j] = R[0];
						return;
					}
				}
			}
		}
		break;

		// Append
		case 3:
		{
			std::string S("");
			i = static_cast<unsigned long>(Rnd() * rules_n);
			S = rules[i][0];

			i = static_cast<unsigned long>(Rnd() * rules_n);
			rules[i] = S;
		}
		break;

		// Swap directions
		case 4:
			for(unsigned long ii = 0; ii < max; ii++)
			{
				i = static_cast<unsigned long>(Rnd() * rules_n);
				for(unsigned long j = 2; j < rules[i].size(); j++)
				{
					char mutations[12][2] = {
						{ '+', '-' },
						{ '-', '+' },
						{ '&', '^' },
						{ '^', '&' },
						{ '>', '<' },
						{ '<', '>' },
						{ '|', '%' },
						{ '%', '|' },
						{ ':', ';' },
						{ ';', ':' },
						{ '\'', '"' },
						{ '"', '\'' } };

					unsigned long random = (unsigned long)(Rnd() * 12.0);

					if(random < 0 || random > 11)
						return;

					if(rules[i][j] == mutations[random][0])
					{
						rules[i][j] = mutations[random][1];
						return;
					}
				}
			}
		break;

		// Swap sizes
		case 5:
			for(unsigned long ii = 0; ii < max; ii++)
			{
				i = static_cast<unsigned long>(Rnd() * rules_n);
				for(unsigned long j = 2; j < rules[i].size(); j++)
				{
					char mutations[6][2] = {
						{ 'F', 'Z' },
						{ 'Z', 'F' },
						{ 'f', 'z' },
						{ 'z', 'f' },
						{ '!', '?' },
						{ '?', '!' } };

					unsigned long random = (unsigned long)(Rnd() * 6.0);

					if(random < 0 || random > 5)
						return;

					if(rules[i][j] == mutations[random][0])
					{
						rules[i][j] = mutations[random][1];
						return;
					}
				}
			}
		break;
	}
}

// Expand l-system into production string.
// object_string is read with the k counter and
// the next generation is built up in otemp
void L_system()
{
	// Map rule names
	typedef std::map<char, unsigned long> map_t;
	map_t S;

	// Each char gets a rule number
	for(unsigned long i = rules.size(); i > 0; i--)
		if(rules[i - 1].size())
			S[(rules[i - 1][0])] = i - 1;

	// For each recursion
	unsigned long max = max_string - 10L;
	bool incomplete = false;
	for(unsigned long l = 0; l < lev; l++)
	{
		// Need markers ?
		bool marker = (l == (lev - 1)) && (fraction != 0.0);

		std::string otemp("");
		unsigned long ss = 0;

		// For each char in the string
		for(unsigned long k = 0; k < object_string.size(); k++)
		{
			// Default rule which does nothing
			unsigned long i = rules.size() - 1;
			// i = rule number attached to current char
			map_t::const_iterator find_rule = S.find(object_string[k]);
			if(find_rule != S.end())
				i = find_rule->second;

			// s = size of current rule
			unsigned long s = rules[i].size() - 2;
			ss += s;

			// Overflow
			if(ss >= max)
			{
				l = lev;
				incomplete = true;
				break;

			}

			// Add mark char
			if(marker && marks[i])
			{
				otemp += '@';
				otemp += std::string(rules[i], 2, s);
				otemp += '@';
			}
			else
			{
				// Copy
				otemp += std::string(rules[i], 2, s);
			}
		}

		// Copy the temp string to object_string and repeat cycle
		object_string = otemp;
	}
}

// Save mutated ls-system for re-run
void L_save()
{
/*
	remove("mutation.ls");
	FILE* f = fopen("mutation.ls", "wt");
	if(!f)
		User_error("Cannot open file [mutation.ls]");

	fprintf(f, "%d\n", lev);
	fprintf(f, "%g\n", (ang / 3.141592654) * 180.0);
	fprintf(f, "%g\n", thick * 100.0);

	fprintf(f, "%s\n", object_string.c_str());

	for(unsigned long i = 0; i < rules.size(); i++)
		{
			if(rules[i][0] == '+')
				break;

			fprintf(f, "%s\n", rules[i].c_str());
		}

	fprintf(f, "@\n");
	fclose(f);
*/
}

// Read a (xx) value from a production string
// at location j and return it as double
double parse_value(unsigned long& j)
{
	// Skip current character and '('
	j += 2;

	std::string val("");
	while(object_string[j] != ')')
		val += object_string[j++];

	std::stringstream scan(val);
	double r = 0.0;
	scan >> r;

	if(last_recur)
		return r * fraction;

	return r;
}

// Set up a rotation matrix
k3d::vector3 C1, C2, C3;

void set_rotation_matrix(double a, k3d::vector3 n)
{
	double cosa = cos(a);
	double sina = sin(a);

	double n11 = n[0] * n[0];
	double n22 = n[1] * n[1];
	double n33 = n[2] * n[2];

	double nxy = n[0] * n[1];
	double nxz = n[0] * n[2];
	double nyz = n[1] * n[2];

	C1[0] = n11 + (1.0 - n11) * cosa;
	C1[1] = nxy * (1.0 - cosa) - n[2] * sina;
	C1[2] = nxz * (1.0 - cosa) + n[1] * sina;

	C2[0] = nxy * (1.0 - cosa) + n[2] * sina;
	C2[1] = n22 + (1.0 - n22) * cosa;
	C2[2] = nyz * (1.0 - cosa) - n[0] * sina;

	C3[0] = nxz * (1.0 - cosa) - n[1] * sina;
	C3[1] = nyz * (1.0 - cosa) + n[0] * sina;
	C3[2] = n33 + (1.0 - n33) * cosa;
}

k3d::vector3 rotate(const k3d::vector3& In)
{
	return k3d::normalize(k3d::vector3(C1 * In, C2 * In, C3 * In));
}

// Process a production string and generate form
void L_draw(k3d::legacy::mesh* mesh, k3d::imaterial* const Material, const k3d::signed_axis Orientation)
{
	// Save values
	double thick_l = 0;
	double ang_l = 0;
	double dis_l = 0;
	double dis2_l = 0;
	double trope_l = 0;

	bool poly_on = false;

	// Setup vectors
	k3d::point3 pos(0.0, 0.0, 0.0);
	k3d::vector3 fow(0.0, 0.0, 1.0);
	k3d::vector3 lef(0.0, 1.0, 0.0);
	k3d::vector3 upp(1.0, 0.0, 0.0);

	trope = k3d::normalize(trope);

	for(unsigned long i = 0; i < object_string.size(); i++)
	{
		// Overflow
		if(polcount > poly_limit)
			break;

		// The next char in the string
		char next = object_string[i + 1];

		// The current char in the string
		switch(object_string[i])
		{
			default:
				break;

			// Marks last recursion level during growing phase
			case '@':
				last_recur = !last_recur;
				if(last_recur)
				{
					// Store all variables and do fraction
					thick_l = thick;
					ang_l = ang;
					dis_l = dis;
					dis2_l = dis2;
					trope_l = trope_amount;

					dis *= fraction;
					dis2 *= fraction;
					thick *= fraction;
					ang *= fraction;
					trope_amount *= fraction;
				}
				else
				{
					// Restore
					thick = thick_l;
					ang = ang_l;
					dis = dis_l;
					dis2 = dis2_l;
					trope_amount = trope_l;
				}
			break;

			case '+':
				save.ang = ang;
				if(next == '(')
				{
					ang = 0.017453 * parse_value(i);
					if(last_recur)
						ang *= fraction;
				}

				set_rotation_matrix(-ang, upp);
				fow = rotate(fow);
				lef = rotate(lef);
				ang = save.ang;
			break;

			case '-':
				save.ang = ang;
				if(next == '(')
				{
					ang = 0.017453 * parse_value(i);
					if(last_recur)
						ang *= fraction;
				}

				set_rotation_matrix(ang, upp);
				fow = rotate(fow);
				lef = rotate(lef);
				ang = save.ang;
			break;

			case '~':
			{
				double r = 6.0;
				if(next == '(')
					r = 0.017453 * parse_value(i);
				else if(rand_set)
					r = 0.017453 * rand_amount;

				double a = Rnd() * r * 2.0 - r;
				set_rotation_matrix(a, upp);
				fow = rotate(fow);
				lef = rotate(lef);
				a = (Rnd() * r * 2.0) - r;
				set_rotation_matrix(a, lef);
				fow = rotate(fow);
				upp = rotate(upp);
				a = (Rnd() * r * 2.0) - r;
				set_rotation_matrix(a, fow);
				lef = rotate(lef);
				upp = rotate(upp);
			}
			break;

			case 't':
			{
				if((fow[0] == 0.0) && (fow[1] == 0.0))
					break;

				save.tr = tr;
				if(trope_set)
					tr = trope_amount;

				if(next == '(')
				{
					tr = parse_value(i);
					if(last_recur)
						tr *= fraction;
				}

				trope = fow;
				trope[0] = -trope[0];
				trope[1] = -trope[1];
				trope[2] = 0.0;
				trope = k3d::normalize(trope);
				double r = tr * (fow * trope);
				set_rotation_matrix(-r, lef);
				fow = rotate(fow);
				upp = rotate(upp);
				tr = save.tr;
			}
			break;

			case '$':
			{
				k3d::vector3 v = fow - sky;
				if(v.length() == 0.0)
					break;

				lef = fow ^ sky;
				upp = fow ^ lef;
				if(upp[2] < 0.0)
				{
					upp = -upp;
					lef = -lef;
				}
			}
			break;

			case '&':
				save.ang = ang;
				if(next == '(')
				{
					ang = 0.017453 * parse_value(i);
					 if(last_recur)
					ang *= fraction;
				}

				set_rotation_matrix(ang, lef);
				fow = rotate(fow);
				upp = rotate(upp);
				ang = save.ang;
			break;

			case '^':
				save.ang = ang;
				if(next == '(')
				{
					ang = 0.017453 * parse_value(i);
					if(last_recur)
						ang *= fraction;
				}

				set_rotation_matrix(-ang, lef);
				fow = rotate(fow);
				upp = rotate(upp);
				ang = save.ang;
			break;

			case '<':
				save.ang = ang;
				if(next == '(')
				{
					ang = 0.017453 * parse_value(i);
					if(last_recur)
						ang *= fraction;
				}

				set_rotation_matrix(-ang, fow);
				lef = rotate(lef);
				upp = rotate(upp);
				ang = save.ang;
			break;

			case '>':
				save.ang = ang;
				if(next == '(')
				{
					ang = 0.017453 * parse_value(i);
					if(last_recur)
						ang *= fraction;
				}

				set_rotation_matrix(ang, fow);
				lef = rotate(lef);
				upp = rotate(upp);
				ang = save.ang;
			break;

			case '%':
				set_rotation_matrix(3.141592654, fow);
				lef = rotate(lef);
				upp = rotate(upp);
			break;

			case '|':
				set_rotation_matrix(3.141592654, upp);
				fow = rotate(fow);
				lef = rotate(lef);
			break;

			case '!':
				if(next == '(')
				{
					if(last_recur)
						thick *= 1.0 + fraction * (parse_value(i) - 1.0);
					else
						thick *= parse_value(i);
				}
				else
				{
					if(last_recur)
						thick *= 1.0 + fraction * (0.7 - 1.0);
					else
						thick *= 0.7;
				}
			break;

			case '?':
				if(next == '(')
				{
					if(last_recur)
						thick *= 1.0 + fraction * (parse_value(i) - 1.0);
					else
						thick *= parse_value(i);
				}
				else
				{
					if(last_recur)
						thick /= 1.0 + fraction * (0.7 - 1.0);
					else
						thick /= 0.7;
				}
			break;

			case ':':
				if(next == '(')
				{
					if(last_recur)
						ang *= 1.0 + fraction * (parse_value(i) - 1.0);
					else
						ang *= parse_value(i);
				}
				else
				{
					if(last_recur)
						ang *= 1.0 + fraction * (0.9 - 1.0);
					else
						ang *= 0.9;
				}
			break;

			case ';':
				if(next == '(')
				{
					if(last_recur)
						ang *= 1.0 + fraction * (parse_value(i) - 1.0);
					else
						ang *= parse_value(i);
				}
				else
				{
					if(last_recur)
						ang /= 1.0 + fraction * (0.9 - 1.0);
					else
						ang /= 0.9;
				}
			break;

			case '\'':
				if(next == '(')
				{
					double r = parse_value(i);
					if(last_recur)
					{
						dis *= 1.0 + fraction * (r - 1.0);
						dis2 *= 1.0 + fraction * (r - 1.0);
					}
					else
					{
						dis *= r;
						dis2 *= r;
					}
				}
				else
				{
					if(last_recur)
					{
						dis *= 1.0 + fraction * (0.9 - 1.0);
						dis2 *= 1.0 + fraction * (0.9 - 1.0);
					}
					else
					{
						dis *= 0.9;
						dis2 *= 0.9;
					}
				}
			break;

			case '"':
				if(next == '(')
				{
					double r = parse_value(i);
					if(last_recur)
					{
						dis *= 1.0 + fraction * (r - 1.0);
						dis2 *= 1.0 + fraction * (r - 1.0);
					}
					else
					{
						dis *= r;
						dis2 *= r;
					}
				}
				else
				{
					if(last_recur)
					{
						dis /= 1.0 + fraction * (0.9 - 1.0);
						dis2 /= 1.0 + fraction * (0.9 - 1.0);
					}
					else
					{
						dis /= 0.9;
						dis2 /= 0.9;
					}
				}
			break;

			case 'Z':
			{
				save.dis2 = dis2;
				if(next == '(')
				{
					dis2 = parse_value(i);
					if(last_recur)
						dis2 *= fraction;
				}

				k3d::point3 end = pos + dis2 * fow;
				if(closed_form)
					add_cylinder(pos, end, upp, col, mesh, Material, Orientation);
				else
					add_cube(pos, end, upp, col, mesh, Material, Orientation);

				pos = end;
				dis2 = save.dis2;
			}
			break;

			case 'F':
			{
				save.dis = dis;
				if(next == '(')
				{
					dis = parse_value(i);
					if(last_recur)
						dis *= fraction;
				}

				k3d::point3 end = pos + dis * fow;
				if(closed_form)
					add_cylinder(pos, end, upp, col, mesh, Material, Orientation);
				else
					add_cube(pos, end, upp, col, mesh, Material, Orientation);

				pos = end;
				dis = save.dis;
			}
			break;

			case '[':
			{
				s_rec new_rec;
				new_rec.pos = pos;
				new_rec.fow = fow;
				new_rec.lef = lef;
				new_rec.upp = upp;
				new_rec.col = col;
				new_rec.dis = dis;
				new_rec.dis2 = dis2;
				new_rec.ang = ang;
				new_rec.thick = thick;
				new_rec.tr = tr;

				if(closed_form)
				{
					new_rec.last = last;
					new_rec.last_col = last_col;
					for(unsigned long j = 0; j < 8; j++)
						new_rec.last_v[j] = last_v[j];
				}

				if(stack.size() < max_stack_size)
					stack.push(new_rec);
			}
			break;

			case ']':
			{
				if(!stack.size())
					break;

				s_rec old_rec = stack.top();
				pos = old_rec.pos;
				fow = old_rec.fow;
				lef = old_rec.lef;
				upp = old_rec.upp;
				col = old_rec.col;
				dis = old_rec.dis;
				dis2 = old_rec.dis2;
				ang = old_rec.ang;
				thick = old_rec.thick;
				tr = old_rec.tr;
				if(closed_form)
				{
					last = old_rec.last;
					last_col = old_rec.last_col;
					for(unsigned long j = 0; j < 8; j++)
						last_v[j] = old_rec.last_v[j];
				}

				stack.pop();
			}
			break;

			case '{':
				if(poly_on)
				{
					vectors_t new_rec = vertices;
					if(pstack.size() < max_stack_size)
						pstack.push(new_rec);
				}

				poly_on = true;

				vertices.clear();
				vertices.push_back(pos);
			break;

			case 'f':
				save.dis = dis;
				if(next == '(')
				{
					dis = parse_value(i);
					if(last_recur)
						dis *= fraction;
				}

				pos = pos + dis * fow;
				if(poly_on)
					vertices.push_back(pos);

				dis = save.dis;
			break;

			case '.':
				if(poly_on)
					vertices.push_back(pos);
			break;

			case 'g':
				save.dis = dis;
				if(next == '(')
				{
					dis = parse_value(i);
					if(last_recur)
						dis *= fraction;
				}

				pos = pos + dis * fow;
				dis = save.dis;
			break;

			case 'z':
				save.dis2 = dis2;
				if(next == '(')
				{
					dis2 = parse_value(i);
					if(last_recur)
						dis2 *= fraction;
				}

				pos = pos + dis2 * fow;
				if(poly_on)
					vertices.push_back(pos);

				dis2 = save.dis2;
			break;

			case '}':
				polygons.clear();
				if(vertices.size() > 3)
				{
					for(unsigned long j = 1; j < vertices.size() - 1; j++)
						polygons.push_back(polygon(0, j, j + 1, j + 1));

					add_geometry(col, mesh, Material);
				}

				poly_on = false;
				if(pstack.size() > 0)
				{
					vertices.clear();

					if(!pstack.size())
						break;

					vertices = pstack.top();
					pstack.pop();

					poly_on = true;
				}
			break;

			case 'c':
				if(next == '(')
					col = (unsigned long)parse_value(i);
				else
					col++;
			break;
		}
	}
}

bool l_parser(const unsigned long RandomSeed, const bool ClosedForm, const double Trope, const unsigned long Mutations, const unsigned long MutationSeed, const unsigned long MaximalStackSize, k3d::legacy::mesh* Mesh, k3d::imaterial* const Material, const k3d::signed_axis Orientation, const bool FlipNormals)
{
	// Make sure we have a polyhedron to fill
	return_val_if_fail(Mesh->polyhedra.size(), false);

	// set minimum thickness
	//min_thick = ;

	// set amount of trope
	trope_amount = Trope;

	// create closed connected cylinders
	closed_form = ClosedForm;

	// Save face orientation variables
	orientation = Orientation;
	flip_normals = FlipNormals;

	// Execute mutations
	srand(MutationSeed);
	for(unsigned long i = 0; i < Mutations; i++)
		// Perform mutations on stored rule set
		L_mutate();

	// Create L-system production string
	L_system();

	// Parse production string and create geometry
	srand(RandomSeed);
	max_stack_size = MaximalStackSize;
	L_draw(Mesh, Material, Orientation);

	return true;
}

} // namespace lparser

/////////////////////////////////////////////////////////////////////////////
// l_parser

class l_parser :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	l_parser(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_file_path(init_owner(*this) + init_name("file") + init_label(_("File")) + init_description(_("File path for lsystem description")) + init_value(k3d::filesystem::path(k3d::share_path() / k3d::filesystem::generic_path("lsystem/abop-nested-polygon-leaves.ls"))) + init_path_mode(k3d::ipath_property::READ) + init_path_type("lsystems")),
		m_recursion(init_owner(*this) + init_name("growth") + init_label(_("Growth")) + init_description(_("Growth value")) + init_value(5.0) + init_step_increment(0.1) + init_constraint(constraint::minimum<double>(1.0)) + init_units(typeid(k3d::measurement::scalar))),
		m_basic_angle(init_owner(*this) + init_name("basic_angle") + init_label(_("Basic angle")) + init_description(_("Basic angle")) + init_value(10.0) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar))),
		m_thickness(init_owner(*this) + init_name("thickness") + init_label(_("Thickness")) + init_description(_("Thickness")) + init_value(50.0) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar))),
		m_random_seed(init_owner(*this) + init_name("random_seed") + init_label(_("Random seed")) + init_description(_("Random seed")) + init_value(0) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_size(init_owner(*this) + init_name("size") + init_label(_("Size")) + init_description(_("Size")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::scalar))),
		m_closed_form(init_owner(*this) + init_name("closed_form") + init_label(_("Closed form")) + init_description(_("Closed form")) + init_value(true)),
		m_mutations(init_owner(*this) + init_name("mutations") + init_label(_("Mutations")) + init_description(_("Mutations")) + init_value(0) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_mutation_seed(init_owner(*this) + init_name("mutation_seed") + init_label(_("Mutation seed")) + init_description(_("Mutation seed")) + init_value(0) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_max_stack_size(init_owner(*this) + init_name("max_stack_size") + init_label(_("Max stack size")) + init_description(_("Max stack size")) + init_value(1000) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_orientation(init_owner(*this) + init_name("orientation") + init_label(_("Orientation")) + init_description(_("Orientation type (forward or backward along X, Y or Z axis)")) + init_value(k3d::PZ) + init_enumeration(k3d::signed_axis_values())),
		m_flip_normals(init_owner(*this) + init_name("flip_normals") + init_label(_("Flip normals")) + init_description(_("Flip normals in case the faces are reversed")) + init_value(false))
	{
		m_file_path.changed_signal().connect(sigc::mem_fun(*this, &l_parser::on_new_file));

		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_recursion.changed_signal().connect(make_reset_mesh_slot());
		m_basic_angle.changed_signal().connect(make_reset_mesh_slot());
		m_thickness.changed_signal().connect(make_reset_mesh_slot());
		m_random_seed.changed_signal().connect(make_reset_mesh_slot());
		m_size.changed_signal().connect(make_reset_mesh_slot());
		m_closed_form.changed_signal().connect(make_reset_mesh_slot());
		m_mutations.changed_signal().connect(make_reset_mesh_slot());
		m_mutation_seed.changed_signal().connect(make_reset_mesh_slot());
		m_max_stack_size.changed_signal().connect(make_reset_mesh_slot());
		m_orientation.changed_signal().connect(make_reset_mesh_slot());
		m_flip_normals.changed_signal().connect(make_reset_mesh_slot());

		// Init with default example
		init_lsystem();
	}

	void init_lsystem()
	{
		// Load configuration file
		const k3d::filesystem::path file_path = m_file_path.pipeline_value();
		double recursion, basic_angle, thickness;
		if(!lparser::load_configuration_values(file_path, recursion, basic_angle, thickness))
			return;

		// Set configuration's default recursion values
		k3d::property::set_internal_value(m_recursion, recursion);
		k3d::property::set_internal_value(m_basic_angle, basic_angle);
		k3d::property::set_internal_value(m_thickness, thickness);

		// Reset bounding box
		lparser::bounding_box = k3d::bounding_box3();
		lparser::bbox_x = lparser::bbox_y = lparser::bbox_z = 0;
	}

	void on_new_file(k3d::iunknown*)
	{
		init_lsystem();
		make_reset_mesh_slot()(0);
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const double recursion = m_recursion.pipeline_value();
		const double basic_angle = m_basic_angle.pipeline_value();
		const double thickness = m_thickness.pipeline_value();
		const unsigned long random_seed = m_random_seed.pipeline_value();
		const double size = m_size.pipeline_value();
		const bool closed_form = m_closed_form.pipeline_value();
		const unsigned long mutations = m_mutations.pipeline_value();
		const unsigned long mutation_seed = m_mutation_seed.pipeline_value();
		const unsigned long max_stack_size = m_max_stack_size.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		// Load configuration file
		const k3d::filesystem::path file_path = m_file_path.pipeline_value();
		if(!lparser::load_configuration_rules(recursion, basic_angle, thickness, file_path))
			return;

		// Create geometry ...
		k3d::legacy::polyhedron* polyhedron = new k3d::legacy::polyhedron();
		Mesh.polyhedra.push_back(polyhedron);
		lparser::l_parser(random_seed, closed_form, 0, mutations, mutation_seed, max_stack_size, &Mesh, material, m_orientation.pipeline_value(), m_flip_normals.pipeline_value());

		// Cache first bounding box to allow growth
		if(lparser::bbox_x == 0 && lparser::bbox_y == 0 && lparser::bbox_z == 0)
		{
			lparser::bbox_x = lparser::bounding_box.width();
			lparser::bbox_y = lparser::bounding_box.height();
			lparser::bbox_z = lparser::bounding_box.depth();
		}

		// Resize ...
		double bbox_size = std::max(std::max(lparser::bbox_x, lparser::bbox_y), lparser::bbox_z);
		if(bbox_size > 0)
		{
			double new_size = 1 / bbox_size * size;
			for(k3d::legacy::mesh::points_t::const_iterator point = Mesh.points.begin(); point != Mesh.points.end(); point++)
				(*point)->position *= new_size;
		}

		assert_warning(is_valid(*polyhedron));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<l_parser, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0xa637e99d, 0x707c4342, 0x8c6d4d15, 0x78c9054a),
			"LSystemParser",
			_("Generates an L-System object from a configuration file"),
			"Polygons",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_file_path;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_recursion;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_basic_angle;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_thickness;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_random_seed;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_size;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_closed_form;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_mutations;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_mutation_seed;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_max_stack_size;
	k3d_data(k3d::signed_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_orientation;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_flip_normals;
};

} // namespace lsystem

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::lsystem::l_parser::get_factory());
K3D_MODULE_END

