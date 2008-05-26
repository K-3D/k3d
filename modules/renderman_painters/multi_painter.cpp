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
#include <k3d-i18n-config.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/node.h>
#include <k3dsdk/properties.h>

namespace libk3drendermanpainters
{

/////////////////////////////////////////////////////////////////////////////
// multi_painter

class multi_painter :
	public k3d::node,
	public k3d::ri::imesh_painter
{
	typedef k3d::node base;

public:
	multi_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::inode*))
			{
				if(k3d::ri::imesh_painter* const painter = dynamic_cast<k3d::ri::imesh_painter*>(boost::any_cast<k3d::inode*>(k3d::property::pipeline_value(property))))
				{
					painter->paint_mesh(Mesh, RenderState);
				}
			}
		}
	}
	
	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		const k3d::iproperty_collection::properties_t& properties = node::properties();
		for(k3d::iproperty_collection::properties_t::const_iterator prop = properties.begin(); prop != properties.end(); ++prop)
		{
			k3d::iproperty& property = **prop;
			if(property.property_type() == typeid(k3d::inode*))
			{
				if(k3d::ri::imesh_painter* const painter = dynamic_cast<k3d::ri::imesh_painter*>(boost::any_cast<k3d::inode*>(k3d::property::pipeline_value(property))))
				{
					painter->paint_complete(Mesh, RenderState);
				}
			}
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<multi_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0x243cba5a, 0x13f342fa, 0x8f1a19cd, 0xfaf34169),
			"RenderManMultiPainter",
			_("Distributes rendering across multiple mesh painters"),
			"RenderMan Painter",
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

} // namespace libk3drendermanpainters

