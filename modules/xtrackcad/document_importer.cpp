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

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/log.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/nurbs_curve.h>
#include <k3dsdk/path.h>
#include <k3dsdk/plugins.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/types.h>

#include <boost/algorithm/string.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/scoped_ptr.hpp>

#include <stack>

namespace module
{

namespace xtrackcad
{

/////////////////////////////////////////////////////////////////////////////
// document_importer
	
class document_importer :
	public k3d::idocument_importer
{
public:
	k3d::bool_t read_file(k3d::idocument& Document, const k3d::filesystem::path& FilePath)
	{
		try
		{
			k3d::log() << info << "Reading " << FilePath.native_console_string() << " using " << get_factory().name() << std::endl;

			k3d::imesh_storage* const track_source = k3d::plugin::create<k3d::imesh_storage>("FrozenMesh", Document, "Track");
			if(!track_source)
				throw std::runtime_error("Error creating track source.");

			k3d::iunknown* const track_instance = k3d::plugin::create("MeshInstance", Document, "Track Instance");
			if(!track_instance)
				throw std::runtime_error("Error creating track instance.");

			k3d::inode* const track_painter = k3d::plugin::create<k3d::inode>("OpenGLNURBSCurvePainter", Document, "Track Painter");
			if(!track_painter)
				throw std::runtime_error("Error creating track painter.");

			k3d::property::connect(
				Document,
				k3d::property::get(*track_source, "output_mesh"),
				k3d::property::get(*track_instance, "input_mesh"));

			k3d::property::set_internal_value(*track_instance, "gl_painter", track_painter);

			k3d::mesh* const track_mesh = new k3d::mesh();
			track_source->reset_mesh(track_mesh);
			track_mesh->points.create();
			track_mesh->point_selection.create();

			boost::scoped_ptr<k3d::nurbs_curve::primitive> track_curves(k3d::nurbs_curve::create(*track_mesh));
			track_curves->material.push_back(0);

			std::stack<k3d::matrix4> transformation;
			transformation.push(k3d::scale3(1, -1, 1) * k3d::identity3());

			k3d::filesystem::ifstream stream(FilePath);
			while(true)
			{
				std::stringstream line;
				k3d::getline(stream, line);

				if(!stream)
					break;

				k3d::string_t token;
				line >> std::skipws >> token;

				if(token.empty())
					continue;

				if(boost::algorithm::starts_with(token, "#"))
					continue;

				if(token == "TITLE1")
				{
					k3d::string_t title;
					k3d::getline(line, title);
					boost::algorithm::trim(title);

					k3d::iunknown* const annotation = k3d::plugin::create("Annotation2D", Document, "Layout Title");
					if(!annotation)
						throw std::runtime_error("Error creating title annotation.");

					k3d::property::set_internal_value(*annotation, "font_size", 18.0);
					k3d::property::set_internal_value(*annotation, "line_width", 400.0);
					k3d::property::set_internal_value(*annotation, "text", title);
					k3d::property::set_internal_value(*annotation, "x", -0.9);
					k3d::property::set_internal_value(*annotation, "y", 0.9);
				}
				else if(token == "TITLE2")
				{
					k3d::string_t title;
					k3d::getline(line, title);
					boost::algorithm::trim(title);

					k3d::iunknown* const annotation = k3d::plugin::create("Annotation2D", Document, "Layout Secondary Title");
					if(!annotation)
						throw std::runtime_error("Error creating secondary title annotation.");

					k3d::property::set_internal_value(*annotation, "font_size", 14.0);
					k3d::property::set_internal_value(*annotation, "line_width", 400.0);
					k3d::property::set_internal_value(*annotation, "text", title);
					k3d::property::set_internal_value(*annotation, "x", -0.9);
					k3d::property::set_internal_value(*annotation, "y", 0.82);
				}
				else if(token == "ROOMSIZE")
				{
					k3d::double_t width = 0;
					k3d::string_t separator;
					k3d::double_t height = 0;	
					line >> width >> separator >> height;

					k3d::imesh_storage* const source = k3d::plugin::create<k3d::imesh_storage>("FrozenMesh", Document, "Room");
					if(!source)
						throw std::runtime_error("Error creating room source.");

					k3d::iunknown* const instance = k3d::plugin::create("MeshInstance", Document, "Room Instance");
					if(!instance)
						throw std::runtime_error("Error creating room instance.");

					k3d::inode* const painter = k3d::plugin::create<k3d::inode>("OpenGLNURBSCurvePainter", Document, "Room Painter");
					if(!painter)
						throw std::runtime_error("Error creating room painter.");

					k3d::property::connect(
						Document,
						k3d::property::get(*source, "output_mesh"),
						k3d::property::get(*instance, "input_mesh"));

					k3d::property::set_internal_value(*instance, "gl_painter", painter);

					k3d::mesh* const mesh = new k3d::mesh();
					source->reset_mesh(mesh);
					mesh->points.create();
					mesh->point_selection.create();

					boost::scoped_ptr<k3d::nurbs_curve::primitive> curves(k3d::nurbs_curve::create(*mesh));

					k3d::mesh::points_t points;
					points.push_back(transformation.top() * k3d::point3(0, 0, 0));
					points.push_back(transformation.top() * k3d::point3(width, 0, 0));
					points.push_back(transformation.top() * k3d::point3(width, height, 0));
					points.push_back(transformation.top() * k3d::point3(0, height, 0));
					k3d::nurbs_curve::add_curve(*mesh, *curves, 2, points, 1);

					curves->material.push_back(0);
				}
				else if(token == "STRAIGHT" || token == "JOINT")
				{
					k3d::mesh::points_t points;

					while(true)
					{
						std::stringstream line;
						k3d::getline(stream, line);

						k3d::string_t token;
						line >> std::skipws >> token;

						if(token == "END")
							break;

						if(!stream)
							break;

						if(token == "E")
						{
							k3d::point3 point(0, 0, 0);
							line >> point[0] >> point[1];
							points.push_back(transformation.top() * point);
						}
						else if(token == "T")
						{
							k3d::uint_t index = 0;
							k3d::point3 point(0, 0, 0);
							line >> index >> point[0] >> point[1];
							points.push_back(transformation.top() * point);
						}
						else
						{
							k3d::log() << warning << "Unhandled token: " << token << std::endl;
						}
					}

					if(points.size() < 2)
						throw std::runtime_error("Track section requires at least two points.");

					k3d::nurbs_curve::add_curve(*track_mesh, *track_curves, 2, points);
				}
				else if(token == "CURVE")
				{
					k3d::uint_t index = 0;
					k3d::uint_t unused;
					k3d::string_t scale;
					k3d::point3 center(0, 0, 0);
					k3d::double_t radius;
					line >> index >> unused >> unused >> unused >> unused;
					line >> scale >> unused >> center[0] >> center[1] >> unused >> radius;
					center = transformation.top() * center;

					k3d::mesh::points_t points;
					k3d::mesh::doubles_t angles;

					while(true)
					{
						std::stringstream line;
						k3d::getline(stream, line);

						k3d::string_t token;
						line >> std::skipws >> token;

						if(token == "END")
							break;

						if(!stream)
							break;

						if(token == "E")
						{
							k3d::point3 point(0, 0, 0);
							k3d::double_t angle = 0;
							line >> point[0] >> point[1] >> angle;
							points.push_back(transformation.top() * point);
							angles.push_back(angle);
						}
						else if(token == "T")
						{
							k3d::uint_t index = 0;
							k3d::point3 point(0, 0, 0);
							k3d::double_t angle = 0;
							line >> index >> point[0] >> point[1] >> angle;
							points.push_back(transformation.top() * point);
							angles.push_back(angle);
						}
						else
						{
							k3d::log() << warning << "Unhandled token: " << token << std::endl;
						}
					}

					if(2 != points.size())
						throw std::runtime_error("Curved track section requires exactly two endpoints.");

					const k3d::vector3 v0 = points[0] - center;
					const k3d::vector3 v1 = points[1] - center;

					const k3d::double_t angle0 = std::atan2(v0[1], v0[0]);
					const k3d::double_t angle1 = std::atan2(v1[1], v1[0]);

					k3d::double_t start_angle = std::min(angle0, angle1);
					k3d::double_t end_angle = std::max(angle0, angle1);

					// This is a hack ... we assume that all curves are less than 180 degrees, which isn't always the case.
					if(end_angle - start_angle > k3d::pi())
						end_angle += k3d::pi_times_2();

					k3d::mesh::knots_t arc_knots;
					k3d::mesh::weights_t arc_weights;
					k3d::mesh::points_t arc_points;
					k3d::nurbs_curve::circular_arc(
						k3d::vector3(radius, 0, 0),
						k3d::vector3(0, radius, 0),
						start_angle,
						end_angle,
						1,
						arc_knots,
						arc_weights,
						arc_points);

					arc_points[0] = points[0];
					arc_points[1] = arc_points[1] + center;
					arc_points[2] = points[1];
 
					k3d::nurbs_curve::add_curve(*track_mesh, *track_curves, 3, arc_points, arc_weights, arc_knots);
				}
				else if(token == "TURNOUT")
				{
					k3d::uint_t index = 0;
					k3d::uint_t unused;
					k3d::string_t scale;
					k3d::vector3 origin(0, 0, 0);
					k3d::double_t angle = 0;
					line >> index >> unused >> unused >> unused >> unused;
					line >> scale >> unused >> origin[0] >> origin[1] >> unused >> angle;

					transformation.push(transformation.top() * k3d::translate3(origin) * k3d::rotate3(k3d::radians(360.0 - angle), k3d::vector3(0, 0, 1)));

					while(true)
					{
						std::stringstream line;
						k3d::getline(stream, line);

						k3d::string_t token;
						line >> std::skipws >> token;

						if(token == "END")
							break;

						if(!stream)
							break;

						if(token == "S")
						{
							k3d::string_t unused;
							k3d::point3 point0(0, 0, 0);
							k3d::point3 point1(0, 0, 0);
							line >> unused >> unused >> point0[0] >> point0[1] >> point1[0] >> point1[1];

							k3d::mesh::points_t points;
							points.push_back(transformation.top() * point0);
							points.push_back(transformation.top() * point1);
							k3d::nurbs_curve::add_curve(*track_mesh, *track_curves, 2, points);
						}
						else if(token == "C")
						{
							k3d::string_t unused;
							k3d::double_t radius = 0;
							k3d::vector3 center(0, 0, 0);
							k3d::double_t start_angle = 0;
							k3d::double_t swing_angle = 0;
							line >> unused >> unused >> radius >> center[0] >> center[1] >> start_angle >> swing_angle;

							transformation.push(transformation.top() * k3d::translate3(center));

							start_angle = 90.0 - start_angle;
							if(start_angle < 0.0)
								start_angle += 180.0;

							k3d::double_t end_angle = start_angle - swing_angle;

							k3d::mesh::knots_t arc_knots;
							k3d::mesh::weights_t arc_weights;
							k3d::mesh::points_t arc_points;
							k3d::nurbs_curve::circular_arc(
								k3d::vector3(radius, 0, 0),
								k3d::vector3(0, radius, 0),
								k3d::radians(start_angle),
								k3d::radians(end_angle),
								1,
								arc_knots,
								arc_weights,
								arc_points);

							arc_points[0] = transformation.top() * arc_points[0];
							arc_points[1] = transformation.top() * arc_points[1];
							arc_points[2] = transformation.top() * arc_points[2];

							k3d::nurbs_curve::add_curve(*track_mesh, *track_curves, 3, arc_points, arc_weights, arc_knots);

							transformation.pop();
						}
						else
						{
							k3d::log() << warning << "Unhandled token: " << token << std::endl;
						}
					}

					transformation.pop();
				}
				else
				{
					k3d::log() << warning << "Unhandled token: " << token << std::endl;
				}
			}
			
			return true;
		}
		catch(std::exception& e)
		{
			k3d::log() << error << "Caught exception: " << e.what() << std::endl;
		}
		catch(...)
		{
			k3d::log() << error << "Caught unknown exception." << std::endl;
		}			

		return false;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<document_importer, k3d::interface_list<k3d::idocument_importer> > factory(
			k3d::uuid(0x3a086a52, 0x8d422391, 0x10c482a0, 0x1eafc053),
			"XTrackCADDocumentImporter",
			_("XTrackCAD Tracks File ( .xtc )"),
			"",
			k3d::iplugin_factory::EXPERIMENTAL,
			boost::assign::map_list_of("k3d:mime-types", "application/x-xtrackcad"));

		return factory;
	}
};

k3d::iplugin_factory& document_importer_factory()
{
	return document_importer::get_factory();
}

} // namespace xtrackcad

} // namespace module

