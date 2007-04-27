// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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
		\brief Implements the RenderManBackgroundPlane K-3D object, which renders a world-aligned plane, centered at the camera's position, with radius equal to the camera farplane
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/vectors.h>

#ifdef	WIN32
#ifdef	near
#undef	near
#endif	//near
#ifdef	far
#undef	far
#endif	//far
#endif	//WIN32
namespace
{

/////////////////////////////////////////////////////////////////////////////
// background_plane

class background_plane :
	public k3d::material_client<k3d::persistent<k3d::node> >,
	public k3d::ri::irenderable
{
	typedef k3d::material_client<k3d::persistent<k3d::node> > base;

public:
	background_plane(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_distance(init_owner(*this) + init_name("distance") + init_label(_("distance")) + init_description(_("Distance")) + init_value(1.0) + init_constraint(k3d::data::constraint::minimum(0.0, k3d::data::constraint::maximum(1.0))))
	{
	}

	void renderman_render(const k3d::ri::render_state& State)
	{
		// We never generate shadows ...
		if(k3d::ri::SHADOW_MAP == State.render_context)
			return;
		// We only generate output for the last sample ...
		if(!k3d::ri::last_sample(State))
			return;

		k3d::iperspective* const perspective = dynamic_cast<k3d::iperspective*>(&State.projection);
		k3d::iorthographic* const orthographic = dynamic_cast<k3d::iorthographic*>(&State.projection);
		if(!perspective && !orthographic)
		{
			k3d::log() << error << k3d_file_reference << ": unknown projection type" << std::endl;
			return;
		}
	
		double left, right, top, bottom, plane;
		if(perspective)
		{
			const double near = boost::any_cast<double>(k3d::get_value(document().dag(), perspective->near()));
			const double far = boost::any_cast<double>(k3d::get_value(document().dag(), perspective->far()));
			plane = k3d::mix(near, far, m_distance.value());
			left = plane * boost::any_cast<double>(k3d::get_value(document().dag(), perspective->left()));
			right = plane * boost::any_cast<double>(k3d::get_value(document().dag(), perspective->right()));
			top = plane * boost::any_cast<double>(k3d::get_value(document().dag(), perspective->top()));
			bottom = plane * boost::any_cast<double>(k3d::get_value(document().dag(), perspective->bottom()));
		}

		if(orthographic)
		{
			const double near = boost::any_cast<double>(k3d::get_value(document().dag(), orthographic->near()));
			const double far = boost::any_cast<double>(k3d::get_value(document().dag(), orthographic->far()));
			plane = k3d::mix(near, far, m_distance.value());
			left = boost::any_cast<double>(k3d::get_value(document().dag(), orthographic->left()));
			right = boost::any_cast<double>(k3d::get_value(document().dag(), orthographic->right()));
			top = boost::any_cast<double>(k3d::get_value(document().dag(), orthographic->top()));
			bottom = boost::any_cast<double>(k3d::get_value(document().dag(), orthographic->bottom()));
		}
		
		State.engine.RiAttributeBegin();
		State.engine.RiAttributeV("identifier", k3d::ri::parameter_list(1, k3d::ri::parameter("name", k3d::ri::UNIFORM, 1, k3d::ri::string(name()))));
		State.engine.RiIdentity();
		State.engine.RiCoordSysTransform(k3d::ri::RI_CAMERA());
		
		k3d::ri::setup_material(m_material.value(), State);

		// Draw a plane ...
		k3d::ri::points points;
		points.push_back(k3d::ri::point(k3d::point3(left, top, plane)));
		points.push_back(k3d::ri::point(k3d::point3(right, top, plane)));
		points.push_back(k3d::ri::point(k3d::point3(left, bottom, plane)));
		points.push_back(k3d::ri::point(k3d::point3(right, bottom, plane)));
		State.engine.RiPatchV("bilinear", k3d::ri::parameter_list(1, k3d::ri::parameter(k3d::ri::RI_P(), k3d::ri::VERTEX, 1, points)));

		State.engine.RiAttributeEnd();
	}

	void renderman_render_complete(const k3d::ri::render_state& State)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<background_plane >factory(
			k3d::uuid(0x80e1c8eb, 0x001d4342, 0xb401551d, 0xd5c45a3d),
			"RenderManBackgroundPlane",
			_("Places a shader on a bilinear patch, aligned with the farplane"),
			"RenderMan",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_distance;
};

} // namespace

namespace libk3drenderman
{

k3d::iplugin_factory& background_plane_factory()
{
	return background_plane::get_factory();
}

} // namespace libk3drenderman



