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

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/drawable_gl.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/ibounded.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/itransform_array_3d.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/transformable.h>

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////
// array_3d

class array_3d :
	public k3d::material_client<k3d::gl::drawable<k3d::ri::renderable<k3d::transformable<k3d::persistent<k3d::node> > > > >
{
	typedef k3d::material_client<k3d::gl::drawable<k3d::ri::renderable<k3d::transformable<k3d::persistent<k3d::node> > > > > base;

public:
	array_3d(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_source(init_owner(*this) + init_name("source") + init_label(_("Source")) + init_description(_("Source")) + init_value<k3d::ri::irenderable*>(0)),
		m_layout(init_owner(*this) + init_name("layout") + init_label(_("Layout")) + init_description(_("Layout")) + init_value<k3d::itransform_array_3d*>(0)),
		m_count1(init_owner(*this) + init_name("count1") + init_label(_("Count 1")) + init_description(_("Dimension 1 Count")) + init_value(5) + init_constraint(constraint::minimum(0L)) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar))),
		m_count2(init_owner(*this) + init_name("count2") + init_label(_("Count 2")) + init_description(_("Dimension 2 Count")) + init_value(5) + init_constraint(constraint::minimum(0L)) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar))),
		m_count3(init_owner(*this) + init_name("count3") + init_label(_("Count 3")) + init_description(_("Dimension 3 Count")) + init_value(5) + init_constraint(constraint::minimum(0L)) + init_step_increment(1.0) + init_units(typeid(k3d::measurement::scalar)))
	{
		m_source.changed_signal().connect(make_async_redraw_slot());
		m_layout.changed_signal().connect(make_async_redraw_slot());
		m_count1.changed_signal().connect(make_async_redraw_slot());
		m_count2.changed_signal().connect(make_async_redraw_slot());
		m_count3.changed_signal().connect(make_async_redraw_slot());
	}

	void draw(const k3d::gl::render_state& State)
	{
		k3d::ibounded* const bounded = dynamic_cast<k3d::ibounded*>(m_source.value());
		k3d::itransform_array_3d* const layout = m_layout.value();
		const unsigned long count1 = m_count1.value();
		const unsigned long count2 = m_count2.value();
		const unsigned long count3 = m_count3.value();

		if(!layout)
			return;

		glDisable(GL_LIGHTING);
		glColor3d(0, 1, 1);

		glBegin(GL_POINTS);
		for(unsigned long i = 0; i != count1; ++i)
		{
			for(unsigned long j = 0; j != count2; ++j)
			{
				for(unsigned long k = 0; k != count3; ++k)
				{
					k3d::gl::vertex3d(k3d::point3(0, 0, 0) * layout->get_element(i, count1, j, count2, k, count3));
				}
			}
		}
		glEnd();

		if(!bounded)
			return;

		const k3d::bounding_box3 bounds = bounded->extents();
		if(bounds.empty())
			return;

		glMatrixMode(GL_MODELVIEW);
		for(unsigned long i = 0; i != count1; ++i)
		{
			for(unsigned long j = 0; j != count2; ++j)
			{
				for(unsigned long k = 0; k != count3; ++k)
				{
					glPushMatrix();
					k3d::gl::push_matrix(layout->get_element(i, count1, j, count2, k, count3));
					k3d::gl::draw_bounding_box(bounds);
					glPopMatrix();
				}
			}
		}
	}

	void on_gl_draw(const k3d::gl::render_state& State)
	{
		draw(State);
	}

	void on_gl_select(const k3d::gl::render_state& State, const k3d::gl::selection_state& SelectState)
	{
		k3d::gl::push_selection_token(this);
		draw(State);
		k3d::gl::pop_selection_token();
	}

	void on_renderman_render(const k3d::ri::render_state& State)
	{
		k3d::ri::irenderable* const renderable = m_source.value();
		if(!renderable)
			return;

		k3d::itransform_array_3d* const layout = m_layout.value();
		if(!layout)
			return;

		const unsigned long count1 = m_count1.value();
		const unsigned long count2 = m_count2.value();
		const unsigned long count3 = m_count3.value();

		// Make sure we don't enter an infinite loop trying to render ourself ...
		if(renderable == this)
		{
			k3d::log() << error << factory().name() << " [" << name() << "] cannot instance itself" << std::endl;
			return;
		}

		const k3d::ri::object_handle handle = State.engine.RiObjectBegin();
			k3d::ri::render_state state(State);
			state.render_context = k3d::ri::OBJECT_INSTANCE;
			renderable->renderman_render(state);
		State.engine.RiObjectEnd();

		k3d::ri::setup_material(m_material.value(), State);

		for(unsigned long i = 0; i != count1; ++i)
		{
			for(unsigned long j = 0; j != count2; ++j)
			{
				for(unsigned long k = 0; k != count3; ++k)
				{
					State.engine.RiAttributeBegin();
					State.engine.RiConcatTransform(k3d::ri::convert(layout->get_element(i, count1, j, count2, k, count3)));
					State.engine.RiObjectInstance(handle);
					State.engine.RiAttributeEnd();
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<array_3d > factory(
				k3d::uuid(0xea6c1213, 0x033d42f5, 0x9146951e, 0x3aee7e95),
				"RenderManArray3D",
				_("Renders a three-dimensional array of geometric object instances"),
				"RenderMan Array",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::ri::irenderable*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_source;
	k3d_data(k3d::itransform_array_3d*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_layout;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_count1;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_count2;
	k3d_data(long, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_count3;
};

/////////////////////////////////////////////////////////////////////////////
// array_3d_factory

k3d::iplugin_factory& array_3d_factory()
{
	return array_3d::get_factory();
}

} // namespace libk3drenderman

