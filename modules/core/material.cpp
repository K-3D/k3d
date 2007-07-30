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
		\author Tim Shead <tshead@k-3d.com>
*/

#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imaterial_gl.h>
#include <k3dsdk/imaterial_ri.h>
#include <k3dsdk/iyafray.h>
#include <k3dsdk/node.h>
#include <k3dsdk/node_change_signal.h>
#include <k3dsdk/persistent.h>

namespace libk3dcore
{

/////////////////////////////////////////////////////////////////////////////
// material

class material :
	public k3d::node_change_signal<k3d::persistent<k3d::node> >,
	public k3d::imaterial
{
	typedef k3d::node_change_signal<k3d::persistent<k3d::node> > base;

public:
	material(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_gl_material(init_owner(*this) + init_name("gl_material") + init_label(_("GL Material")) + init_description(_("OpenGL Material")) + init_value<k3d::gl::imaterial*>(0)),
		m_ri_material(init_owner(*this) + init_name("ri_material") + init_label(_("RenderMan Material")) + init_description(_("RenderMan Material")) + init_value<k3d::ri::imaterial*>(0)),
		m_yafray_material(init_owner(*this) + init_name("yafray_material") + init_label(_("Yafray Material")) + init_description(_("Yafray Material")) + init_value<k3d::yafray::imaterial*>(0))
	{
		m_gl_material.changed_signal().connect(make_node_change_slot());
	}

	k3d::gl::imaterial* gl_material()
	{
		return m_gl_material.pipeline_value();
	}

	k3d::ri::imaterial* ri_material()
	{
		return m_ri_material.pipeline_value();
	}

	k3d::yafray::imaterial* yafray_material()
	{
		return m_yafray_material.pipeline_value();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<material,
			k3d::interface_list<k3d::imaterial> > factory(
				k3d::classes::Material(),
				"Material",
				_("Material"),
				"Materials",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::gl::imaterial*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_gl_material;
	k3d_data(k3d::ri::imaterial*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_ri_material;
	k3d_data(k3d::yafray::imaterial*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_yafray_material;
};

k3d::iplugin_factory& material_factory()
{
	return material::get_factory();
}

} // namespace libk3dcore

