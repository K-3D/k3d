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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/mesh_painter_gl.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property.h>

namespace libk3dopengl
{

/////////////////////////////////////////////////////////////////////////////
// multi_painter

class multi_painter :
	public k3d::gl::mesh_painter
{
	typedef k3d::gl::mesh_painter base;

public:
	multi_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void on_paint_mesh(const k3d::dev::mesh& Mesh, const k3d::gl::painter_render_state& RenderState)
	{
		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::inode*))
			{
				if(k3d::gl::imesh_painter* const painter = dynamic_cast<k3d::gl::imesh_painter*>(boost::any_cast<k3d::inode*>(k3d::get_value(document().dag(), property))))
				{
					painter->paint_mesh(Mesh, RenderState);
				}
			}
		}
	}
	
	void on_select_mesh(const k3d::dev::mesh& Mesh, const k3d::gl::painter_render_state& RenderState, const k3d::gl::painter_selection_state& SelectionState)
	{
		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::inode*))
			{
				if(k3d::gl::imesh_painter* const painter = dynamic_cast<k3d::gl::imesh_painter*>(boost::any_cast<k3d::inode*>(k3d::get_value(document().dag(), property))))
				{
					painter->select_mesh(Mesh, RenderState, SelectionState);
				}
			}
		}
	}
	
	void on_mesh_changed(const k3d::dev::mesh& Mesh, k3d::iunknown* Hint)
	{
		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::inode*))
			{
				if(k3d::gl::imesh_painter* const painter = dynamic_cast<k3d::gl::imesh_painter*>(boost::any_cast<k3d::inode*>(k3d::get_value(document().dag(), property))))
				{
					painter->mesh_changed(Mesh, Hint);
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<multi_painter, k3d::interface_list<k3d::gl::imesh_painter > > factory(
			k3d::uuid(0x6bc7d8a1, 0xbbe7491f, 0xa2405d97, 0x8b136c41),
			"OpenGLMultiPainter",
			_("Distributes rendering across multiple mesh painters"),
			"OpenGL Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// multi_painter_factory

k3d::iplugin_factory& multi_painter_factory()
{
	return multi_painter::get_factory();
}

} // namespace libk3dopengl

