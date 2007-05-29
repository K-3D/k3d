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

/** \file Paint points using a VBO
 */

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/extension_gl.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/hints.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/painter_render_state_gl.h>
#include <k3dsdk/painter_selection_state_gl.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

#include "colored_selection_painter_gl.h"
#include "vbo.h"

namespace libk3ddevelopment
{

	/////////////////////////////////////////////////////////////////////////////
// point_array_painter

class point_array_painter :
	public colored_selection_painter,
	public k3d::hint::hint_processor
{
	typedef colored_selection_painter base;

public:
	point_array_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_points_cache(k3d::painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, point_vbo>::instance(Document)),
		m_selection_cache(k3d::painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, point_selection>::instance(Document))
	{
	}
	
	~point_array_painter()
	{
		m_points_cache.remove_painter(this);
		m_selection_cache.remove_painter(this);
	}
		
	void on_paint_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!Mesh.points || Mesh.points->empty())
			return;
			
		clean_vbo_state();
		
		point_vbo* point_buffer = m_points_cache.create_data(Mesh.points);
		point_buffer->execute(Mesh);
		point_buffer->bind();
		
		m_selection_cache.create_data(Mesh.points)->execute(Mesh);
		
		const k3d::color color = RenderState.node_selection ? selected_mesh_color() : unselected_mesh_color();
		const k3d::color selected_color = RenderState.show_component_selection ? selected_component_color() : color;

		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);
		
		size_t point_count = Mesh.points->size();
		const selection_records_t& point_selection = m_selection_cache.get_data(Mesh.points)->records(); // obtain selection data
		
		if (!point_selection.empty())
		{
			for (selection_records_t::const_iterator record = point_selection.begin(); record != point_selection.end(); ++record)
			{ // color by selection
				k3d::gl::color3d(record->weight ? selected_color : color);
				size_t start = record->begin;
				size_t end = record->end;
				end = end > point_count ? point_count : end;
				size_t count = end - start;
				glDrawArrays(GL_POINTS, start, count);
			}
		}
		else
		{ // empty selection, everything has the same color
			k3d::gl::color3d(color);
			glDrawArrays(GL_POINTS, 0, Mesh.points->size());
		}
		
		clean_vbo_state();
	}
	
	void on_select_mesh(const k3d::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!Mesh.points || Mesh.points->empty())
			return;
			
		if (!SelectionState.select_points)
			return;
		
		clean_vbo_state();

		point_vbo* const point_buffer = m_points_cache.create_data(Mesh.points);
		point_buffer->execute(Mesh);
		point_buffer->bind();
		
		k3d::gl::store_attributes attributes;
		glDisable(GL_LIGHTING);
		
		const size_t point_count = Mesh.points->size();
		for(size_t point = 0; point != point_count; ++point)
		{
			k3d::gl::push_selection_token(k3d::selection::ABSOLUTE_POINT, point);

			glBegin(GL_POINTS);
			glArrayElement(point);
			glEnd();

			k3d::gl::pop_selection_token();
		}

		clean_vbo_state();
	}
	
	void on_mesh_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		return_if_fail(k3d::gl::extension::query_vbo());

		if(!Mesh.points || Mesh.points->empty())
			return;
		
		m_points_cache.register_painter(Mesh.points, this);
		m_selection_cache.register_painter(Mesh.points, this);
				
		process(Mesh, Hint);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<point_array_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
				k3d::uuid(0xe2495ce1, 0x0c4c42eb, 0x81142404, 0x3c25889e),
		"VBOPointPainter",
		_("Renders mesh points"),
		"Development",
		k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

protected:

	///////
	// hint processor implementation
	///////
	
	virtual void on_geometry_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_points_cache.create_data(Mesh.points)->schedule(Mesh, Hint);
	}
	
	virtual void on_selection_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_selection_cache.create_data(Mesh.points)->schedule(Mesh, Hint);
	}
	
	virtual void on_topology_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		on_mesh_deleted(Mesh, Hint);
	}
	
	virtual void on_address_changed(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		k3d::hint::mesh_address_changed_t* address_hint = dynamic_cast<k3d::hint::mesh_address_changed_t*>(Hint);
		return_if_fail(address_hint);
		m_points_cache.switch_key(address_hint->old_points_address, Mesh.points);
		m_selection_cache.switch_key(address_hint->old_points_address, Mesh.points);
	}
	
	virtual void on_mesh_deleted(const k3d::mesh& Mesh, k3d::iunknown* Hint)
	{
		m_points_cache.remove_data(Mesh.points);
		m_selection_cache.remove_data(Mesh.points);
	}
	
private:
	k3d::painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, point_vbo>& m_points_cache;
	k3d::painter_cache<boost::shared_ptr<const k3d::mesh::points_t>, point_selection>& m_selection_cache;
};

	/////////////////////////////////////////////////////////////////////////////
// point_array_painter_factory

	k3d::iplugin_factory& point_array_painter_factory()
	{
		return point_array_painter::get_factory();
	}

} // namespace libk3ddevelopment

