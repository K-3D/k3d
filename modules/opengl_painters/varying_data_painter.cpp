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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include "utility.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/named_arrays.h>
#include <k3dsdk/options.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/polyhedron.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/share.h>

#include <FTGL/ftgl.h>

#include <boost/scoped_ptr.hpp>

namespace module
{

namespace opengl
{

namespace painters
{

namespace detail
{

typedef std::vector<k3d::string_t> strings_t;

/// Converts the named array with the given name to an array of strings
template<typename ArrayT>
void named_array_to_strings(const k3d::mesh::attribute_arrays_t& Arrays, const k3d::string_t& ArrayName, strings_t& StringArray)
{
	const ArrayT* array = Arrays.lookup<ArrayT>(ArrayName);
	if (!array)
		return;
	
	const k3d::uint_t array_size = array->size();
	StringArray.resize(array_size);
	for(k3d::uint_t i = 0; i != array_size; ++i)
	{
		StringArray[i] = k3d::string_cast(array->at(i));
	}
}

}

/////////////////////////////////////////////////////////////////////////////
// varying_data_painter

class varying_data_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	varying_data_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_font_path(init_owner(*this) + init_name("font") + init_label(_("Font")) + init_description(_("Font path")) + init_value(k3d::share_path() / k3d::filesystem::generic_path("fonts/VeraBd.ttf")) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::fonts())),
		m_font_size(init_owner(*this) + init_name("font_size") + init_label(_("Font Size")) + init_description(_("Font size.")) + init_value(14.0)),
		m_antialias(init_owner(*this) + init_name("antialias") + init_label(_("Font Antialiasing")) + init_description(_("Render antialiased text.")) + init_value(true)),
		m_color(init_owner(*this) + init_name("color") + init_label(_("Color")) + init_description(_("Color of the text")) + init_value(k3d::color(0, 1, 0))),
		m_edge_offset(init_owner(*this) + init_name("edge_offset") + init_label(_("Edge Offset")) + init_description(_("Offsets numbers away from their edges.")) + init_value(0.5)),
		m_face_offset(init_owner(*this) + init_name("face_offset") + init_label(_("Face Offset")) + init_description(_("Offsets numbers away from their polygons.")) + init_value(0.2)),
		m_array_name(init_owner(*this) + init_name("array_name") + init_label(_("Array Name")) + init_description(_("Specifies the array to draw")) + init_value(std::string("Cs")))
	{
		m_font_path.changed_signal().connect(sigc::mem_fun(*this, &varying_data_painter::on_font_changed));
		m_font_size.changed_signal().connect(sigc::mem_fun(*this, &varying_data_painter::on_font_changed));
		m_antialias.changed_signal().connect(sigc::mem_fun(*this, &varying_data_painter::on_font_changed));
		m_color.changed_signal().connect(make_async_redraw_slot());
		m_edge_offset.changed_signal().connect(make_async_redraw_slot());
		m_face_offset.changed_signal().connect(make_async_redraw_slot());
		m_array_name.changed_signal().connect(make_async_redraw_slot());
	}

	void on_font_changed(k3d::ihint*)
	{
		m_font.reset();
		async_redraw(0);
	}

	void draw(const k3d::mesh::indices_t& FaceFirstLoops, const k3d::mesh::counts_t& FaceLoopCounts, const k3d::mesh::indices_t& LoopFirstEdges, const k3d::mesh::indices_t& EdgePoints, const k3d::mesh::indices_t& ClockwiseEdges, const k3d::mesh::points_t& Points, const k3d::mesh::normals_t& FaceNormals, const k3d::color& Color, const k3d::double_t EdgeOffset, const k3d::double_t FaceOffset, FTFont& Font, const detail::strings_t StringArray)
	{
		k3d::gl::color3d(Color);

		const size_t face_begin = 0;
		const size_t face_end = face_begin + FaceFirstLoops.size();
		for(size_t face = face_begin; face != face_end; ++face)
		{
			const size_t loop_begin = FaceFirstLoops[face];
			const size_t loop_end = loop_begin + FaceLoopCounts[face];
			for(size_t loop = loop_begin; loop != loop_end; ++loop)
			{
				const size_t first_edge = LoopFirstEdges[loop];
				for(size_t edge = first_edge; ; )
				{
					const k3d::point3 vertex1 = Points[EdgePoints[edge]];
					const k3d::point3 vertex2 = Points[EdgePoints[ClockwiseEdges[edge]]];
					const k3d::vector3 edge_vector = vertex2 - vertex1;

					const k3d::point3 position = vertex1 + (0.5 * edge_vector) + (EdgeOffset * k3d::normalize(FaceNormals[face] ^ edge_vector)) + (FaceOffset * k3d::to_vector(k3d::normalize(FaceNormals[face])));

					glRasterPos3d(position[0], position[1], position[2]);
					if(edge < StringArray.size())
						Font.Render(StringArray[edge].c_str());

					edge = ClockwiseEdges[edge];
					if(edge == first_edge)
						break;
				}
			}
		}
	}

	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		if(!has_non_empty_polyhedra(Mesh))
			return;

		if(!m_font)
		{
			if(m_antialias.pipeline_value())
				m_font.reset(new FTPixmapFont(m_font_path.pipeline_value().native_filesystem_string().c_str()));
			else
				m_font.reset(new FTBitmapFont(m_font_path.pipeline_value().native_filesystem_string().c_str()));

			m_font->FaceSize(static_cast<unsigned int>(m_font_size.pipeline_value()));
			m_font->UseDisplayList(true);
			if(m_font->Error())
			{
				k3d::log() << error << "error initializing font" << std::endl;
				return;
			}
		}

		const k3d::double_t edge_offset = m_edge_offset.pipeline_value();
		const k3d::double_t face_offset = m_face_offset.pipeline_value();

		for(k3d::mesh::primitives_t::const_iterator primitive = Mesh.primitives.begin(); primitive != Mesh.primitives.end(); ++primitive)
		{
			boost::scoped_ptr<k3d::polyhedron::const_primitive> polyhedron(k3d::polyhedron::validate(**primitive));
			if(!polyhedron.get())
				continue;
			const k3d::mesh::indices_t& face_first_loops = polyhedron->face_first_loops;
			const k3d::mesh::counts_t& face_loop_counts = polyhedron->face_loop_counts;
			const k3d::mesh::indices_t& loop_first_edges = polyhedron->loop_first_edges;
			const k3d::mesh::indices_t& edge_points = polyhedron->edge_points;
			const k3d::mesh::indices_t& clockwise_edges = polyhedron->clockwise_edges;
			const k3d::mesh::points_t& points = *Mesh.points;

			const size_t face_count = face_first_loops.size();
	
			// Calculate face normals ...
			k3d::typed_array<k3d::normal3> normals(face_count);
			for(size_t face = 0; face != face_count; ++face)
				normals[face] = k3d::polyhedron::normal(edge_points, clockwise_edges, points, loop_first_edges[face_first_loops[face]]);
	
			k3d::gl::store_attributes attributes;
			glDisable(GL_LIGHTING);
			
			// Try some different types for the array to print
			const k3d::string_t array_name = m_array_name.pipeline_value();
			detail::strings_t string_array;
			detail::named_array_to_strings<k3d::mesh::colors_t>(polyhedron->face_varying_data, array_name, string_array);
			detail::named_array_to_strings<k3d::mesh::normals_t>(polyhedron->face_varying_data, array_name, string_array);
			detail::named_array_to_strings<k3d::mesh::indices_t>(polyhedron->face_varying_data, array_name, string_array);
			detail::named_array_to_strings<k3d::mesh::weights_t>(polyhedron->face_varying_data, array_name, string_array);
			
			draw(face_first_loops, face_loop_counts, loop_first_edges, edge_points, clockwise_edges, points, normals, m_color.pipeline_value(), edge_offset, face_offset, *m_font, string_array);
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<varying_data_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x4a155ddc, 0x55436122, 0xaf1892bb, 0xfc5e8241),
			"OpenGLVaryingDataPainter",
			_("Prints associated varying data near each edge, for debugging purposes"),
			"OpenGL Painter",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_font_path;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_font_size;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_antialias;
	k3d_data(k3d::color, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_color;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_edge_offset;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_face_offset;
	k3d_data(k3d::string_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_array_name;

	boost::scoped_ptr<FTFont> m_font;
};

/////////////////////////////////////////////////////////////////////////////
// varying_data_painter_factory

k3d::iplugin_factory& varying_data_painter_factory()
{
	return varying_data_painter::get_factory();
}

} // namespace painters

} // namespace opengl

} // namespace module

