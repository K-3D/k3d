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
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/bezier.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/share.h>

#include <iterator>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace module
{

namespace freetype2
{

namespace detail
{

const k3d::filesystem::path default_font()
{
	return k3d::share_path() / k3d::filesystem::generic_path("fonts/VeraBd.ttf");
}

/// Defines a closed contour
typedef std::vector<k3d::point3> contour_t;

/// Returns the signed 2D area of a contour
const double area(const contour_t& Contour)
{
	double result = 0;

	for(unsigned long i = 0; i != Contour.size(); ++i)
		result += (Contour[i][0] * Contour[(i+1)%Contour.size()][1]) - (Contour[(i+1)%Contour.size()][0] * Contour[i][1]);

	return result * 0.5;
}

/// Returns true iff a contour is clockwise
const bool clockwise(const contour_t& Contour)
{
	return area(Contour) < 0;
}

/// Encapsulates the freetype FT_Library struct to provide RAII behavior
class freetype_library
{
public:
	freetype_library() :
		m_initialized(0 == FT_Init_FreeType(&m_library))
	{
	}

	~freetype_library()
	{
		if(m_initialized)
			FT_Done_FreeType(m_library);
	}

	operator bool()
	{
		return m_initialized;
	}

	operator FT_Library()
	{
		return m_library;
	}

private:
	const bool m_initialized;
	FT_Library m_library;
};

/// Encapsulates the freetype FT_Face struct to provide RAII behavior
class freetype_face
{
public:
	freetype_face(FT_Library Library, const k3d::filesystem::path& Path) :
		m_initialized(0 == FT_New_Face(Library, Path.native_filesystem_string().c_str(), 0, &m_face))
	{
	}

	~freetype_face()
	{
		if(m_initialized)
			FT_Done_Face(m_face);
	}

	operator bool()
	{
		return m_initialized;
	}

	operator FT_Face()
	{
		return m_face;
	}

	FT_Face& face()
	{
		return m_face;
	}

private:
	const bool m_initialized;
	FT_Face m_face;
};

/// Adds a freetype glyph outline to a k3d::legacy::mesh
class freetype_outline
{
public:
	freetype_outline(const unsigned long CurveDivisions) :
		curve_divisions(CurveDivisions)
	{
		ft_outline_funcs.move_to = raw_move_to_func;
		ft_outline_funcs.line_to = raw_line_to_func;
		ft_outline_funcs.conic_to = raw_conic_to_func;
		ft_outline_funcs.cubic_to = raw_cubic_to_func;
		ft_outline_funcs.shift = 0;
		ft_outline_funcs.delta = 0;
	}

	void convert(FT_Outline& Outline, k3d::legacy::mesh& Mesh, k3d::legacy::polyhedron& Polyhedron, k3d::imaterial* const Material, const k3d::matrix4& Matrix)
	{
		contours.clear();

		// Generate a set of closed contours ...
		FT_Outline_Decompose(&Outline, &ft_outline_funcs, this);

		// Segregate contours into faces and holes, based on their orientation (clockwise or counter-clockwise, respectively)
		contours_t face_contours;
		contours_t hole_contours;
		for(contours_t::iterator contour = contours.begin(); contour != contours.end(); ++contour)
		{
			if(clockwise(*contour))
				face_contours.push_back(*contour);
			else
				hole_contours.push_back(*contour);
		}

		// Create faces ...
		std::vector<k3d::legacy::face*> faces;
		for(contours_t::iterator contour = face_contours.begin(); contour != face_contours.end(); ++contour)
		{
			std::vector<k3d::legacy::split_edge*> edges;
			for(contour_t::iterator point = contour->begin(); point != contour->end(); ++point)
			{
				Mesh.points.push_back(new k3d::legacy::point(Matrix * (*point)));
				edges.push_back(new k3d::legacy::split_edge(Mesh.points.back()));
			}
			k3d::legacy::loop_edges(edges.begin(), edges.end());

			Polyhedron.faces.push_back(new k3d::legacy::face(edges.front(), Material));
			faces.push_back(Polyhedron.faces.back());
		}

		// Add holes ...
		for(contours_t::iterator contour = hole_contours.begin(); contour != hole_contours.end(); ++contour)
		{
			return_if_fail(!faces.empty());

			std::vector<k3d::legacy::split_edge*> edges;
			for(contour_t::iterator point = contour->begin(); point != contour->end(); ++point)
			{
				Mesh.points.push_back(new k3d::legacy::point(Matrix * (*point)));
				edges.push_back(new k3d::legacy::split_edge(Mesh.points.back()));
			}
			k3d::legacy::loop_edges(edges.begin(), edges.end());

			faces[0]->holes.push_back(edges.front());
		}
	}

private:
	void begin_contour(const k3d::point3& From)
	{
		contours.push_back(contour_t());
		last_point = From;
	}

	void line_to(const k3d::point3& To)
	{
		contours.back().push_back(To);
		last_point = To;
	}

	void conic_to(const k3d::point3& From, const k3d::point3& Control, const k3d::point3& To)
	{
		std::vector<k3d::point3> control_points;
		control_points.push_back(From);
		control_points.push_back(Control);
		control_points.push_back(To);

		for(unsigned long i = 0; i != curve_divisions; ++i)
		{
			contours.back().push_back(k3d::Bezier<k3d::point3>(control_points, static_cast<double>(i+1) / static_cast<double>(curve_divisions)));
		}

		last_point = To;
	}

	void cubic_to(const k3d::point3& From, const k3d::point3& Control1, const k3d::point3& Control2, const k3d::point3& To)
	{
		std::vector<k3d::point3> control_points;
		control_points.push_back(From);
		control_points.push_back(Control1);
		control_points.push_back(Control2);
		control_points.push_back(To);

		for(unsigned long i = 0; i != curve_divisions; ++i)
		{
			contours.back().push_back(k3d::Bezier<k3d::point3>(control_points, static_cast<double>(i+1) / static_cast<double>(curve_divisions)));
		}

		last_point = To;
	}

	const k3d::point3 convert(const FT_Vector* RHS)
	{
		return k3d::point3(RHS->x, RHS->y, 0);
	}

	int move_to_func(const FT_Vector* To)
	{
		begin_contour(convert(To));
		return 0;
	}

	int line_to_func(const FT_Vector* To)
	{
		line_to(convert(To));
		return 0;
	}

	int conic_to_func(const FT_Vector* Control, const FT_Vector* To)
	{
		conic_to(last_point, convert(Control), convert(To));
		return 0;
	}

	int cubic_to_func(const FT_Vector* Control1, const FT_Vector* Control2, const FT_Vector* To)
	{
		cubic_to(last_point, convert(Control1), convert(Control2), convert(To));
		return 0;
	}

#if (((FREETYPE_MAJOR) > 2) || ((FREETYPE_MAJOR) == 2 && (FREETYPE_MINOR) >= 2))

	static int raw_move_to_func(const FT_Vector* to, void* user)
	{
		return reinterpret_cast<freetype_outline*>(user)->move_to_func(to);
	}

	static int raw_line_to_func(const FT_Vector* to, void* user)
	{
		return reinterpret_cast<freetype_outline*>(user)->line_to_func(to);
	}

	static int raw_conic_to_func(const FT_Vector* control, const FT_Vector* to, void* user)
	{
		return reinterpret_cast<freetype_outline*>(user)->conic_to_func(control, to);
	}

	static int raw_cubic_to_func(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user)
	{
		return reinterpret_cast<freetype_outline*>(user)->cubic_to_func(control1, control2, to);
	}

#else

	static int raw_move_to_func(FT_Vector* to, void* user)
	{
		return reinterpret_cast<freetype_outline*>(user)->move_to_func(to);
	}

	static int raw_line_to_func(FT_Vector* to, void* user)
	{
		return reinterpret_cast<freetype_outline*>(user)->line_to_func(to);
	}

	static int raw_conic_to_func(FT_Vector* control, FT_Vector* to, void* user)
	{
		return reinterpret_cast<freetype_outline*>(user)->conic_to_func(control, to);
	}

	static int raw_cubic_to_func(FT_Vector* control1, FT_Vector* control2, FT_Vector* to, void* user)
	{
		return reinterpret_cast<freetype_outline*>(user)->cubic_to_func(control1, control2, to);
	}

#endif

	const unsigned long curve_divisions;

	FT_Outline_Funcs ft_outline_funcs;

	k3d::point3 last_point;
	typedef std::vector<contour_t> contours_t;
	contours_t contours;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// poly_text

class poly_text :
	public k3d::material_sink<k3d::legacy::mesh_source<k3d::node > >
{
	typedef k3d::material_sink<k3d::legacy::mesh_source<k3d::node > > base;

public:
	poly_text(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_font_path(init_owner(*this) + init_name("font") + init_label(_("Font")) + init_description(_("Font path")) + init_value(detail::default_font()) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::fonts())),
		m_text(init_owner(*this) + init_name("text") + init_label(_("Text")) + init_description(_("Text")) + init_value<std::string>("Text!")),
		m_curve_divisions(init_owner(*this) + init_name("curve_divisions") + init_label(_("Curve subdivisions")) + init_description(_("Bezier curves subdivision number")) + init_value(3) + init_constraint(constraint::minimum<k3d::int32_t>(1)) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar))),
		m_height(init_owner(*this) + init_name("height") + init_label(_("Height")) + init_description(_("Font height")) + init_value(10.0) + init_step_increment(0.01) + init_units(typeid(k3d::measurement::distance))),
		m_orientation(init_owner(*this) + init_name("orientation") + init_label(_("Orientation")) + init_description(_("Orientation type (forward or backward along X, Y or Z axis)")) + init_value(k3d::PY) + init_enumeration(k3d::signed_axis_values()))
	{
		m_material.changed_signal().connect(make_reset_mesh_slot());
		m_font_path.changed_signal().connect(make_reset_mesh_slot());
		m_text.changed_signal().connect(make_reset_mesh_slot());
		m_curve_divisions.changed_signal().connect(make_reset_mesh_slot());
		m_height.changed_signal().connect(make_reset_mesh_slot());
		m_orientation.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		const k3d::filesystem::path font_path = m_font_path.pipeline_value();
		const std::string text = m_text.pipeline_value();
		const unsigned long curve_divisions = m_curve_divisions.pipeline_value();
		const double height = m_height.pipeline_value();
		k3d::imaterial* const material = m_material.pipeline_value();

		k3d::matrix4 char_orientation;
		k3d::vector3 offset_direction;

		switch(m_orientation.pipeline_value())
		{
			case k3d::PX:
				char_orientation = k3d::rotation3D(k3d::angle_axis(k3d::radians(-90), k3d::vector3(0, 0, 1))) *
					k3d::rotation3D(k3d::angle_axis(k3d::radians(90), k3d::vector3(1, 0, 0)));
				offset_direction = k3d::vector3(0, -1, 0);
				break;
			case k3d::NX:
				char_orientation = k3d::rotation3D(k3d::angle_axis(k3d::radians(90), k3d::vector3(0, 0, 1))) *
					k3d::rotation3D(k3d::angle_axis(k3d::radians(90), k3d::vector3(1, 0, 0)));
				offset_direction = k3d::vector3(0, 1, 0);
				break;
			case k3d::PY:
				char_orientation = k3d::rotation3D(k3d::angle_axis(k3d::radians(90), k3d::vector3(1, 0, 0)));
				offset_direction = k3d::vector3(1, 0, 0);
				break;
			case k3d::NY:
				char_orientation = k3d::rotation3D(k3d::angle_axis(k3d::radians(180), k3d::vector3(0, 0, 1))) *
					k3d::rotation3D(k3d::angle_axis(k3d::radians(90), k3d::vector3(1, 0, 0)));
				offset_direction = k3d::vector3(-1, 0, 0);
				break;
			case k3d::PZ:
				char_orientation = k3d::rotation3D(k3d::angle_axis(k3d::radians(180), k3d::vector3(1, 0, 0)));
				offset_direction = k3d::vector3(1, 0, 0);
				break;
			case k3d::NZ:
				char_orientation = k3d::identity3D();
				offset_direction = k3d::vector3(1, 0, 0);
				break;
		}

		k3d::legacy::polyhedron* const polyhedron = new k3d::legacy::polyhedron();
		Mesh.polyhedra.push_back(polyhedron);

		detail::freetype_library ft_library;
		if(!ft_library)
		{
			k3d::log() << error << "Error initializing FreeType library" << std::endl;
			return;
		}

		detail::freetype_face ft_face(ft_library, font_path);
		if(!ft_face)
		{
			k3d::log() << error << "Error opening font file: " << font_path.native_console_string() << std::endl;
			return;
		}

		if(!FT_IS_SCALABLE(ft_face.face()))
		{
			k3d::log() << error << "Not a scalable font: " << font_path.native_console_string() << std::endl;
			return;
		}

		const double normalize_height = 1.0 / static_cast<double>(ft_face.face()->bbox.yMax - ft_face.face()->bbox.yMin);
		const double scale = normalize_height * height;

		detail::freetype_outline outline(curve_divisions);

		double offset = 0;

		for(std::string::const_iterator c = text.begin(); c != text.end(); ++c)
		{
			if(0 != FT_Load_Glyph(ft_face, FT_Get_Char_Index(ft_face, static_cast<FT_ULong>(*c)), FT_LOAD_NO_SCALE | FT_LOAD_IGNORE_TRANSFORM))
			{
				k3d::log() << error << "Error loading glyph for " << font_path.native_console_string() << "[" << *c << "]" << std::endl;
				continue;
			}

			const k3d::matrix4 matrix =
				k3d::translation3D(offset_direction * (offset * scale)) * char_orientation * k3d::scaling3D(k3d::point3(scale, scale, scale));

			outline.convert(ft_face.face()->glyph->outline, Mesh, *polyhedron, material, matrix);

			offset += ft_face.face()->glyph->metrics.horiAdvance;
		}

		assert_warning(is_valid(*polyhedron));
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<poly_text, k3d::interface_list<k3d::imesh_source > > factory(
			k3d::uuid(0x9acaeaf1, 0x1fe74387, 0xae71cbb3, 0x9b5e33fd),
			"PolyText",
			_("Generates polygonal text using Freetype 2"),
			"Polygons Text",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_font_path;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_text;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_curve_divisions;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_height;
	k3d_data(k3d::signed_axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_orientation;
};

} // namespace freetype2

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::freetype2::poly_text::get_factory());
K3D_MODULE_END

