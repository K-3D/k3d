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
		\brief Implements the RenderManBackgroundSphere K-3D object, which renders a world-aligned sphere, centered at the camera's position, with radius equal to the camera farplane
		\author Tim Shead (tshead@k-3d.com)
*/
#include <k3dsdk/document_plugin_factory.h>

#include <k3d-i18n-config.h>
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
// background_sphere

class background_sphere :
	public k3d::material_client<k3d::persistent<k3d::node> >,
	public k3d::ri::irenderable
{
	typedef k3d::material_client<k3d::persistent<k3d::node> > base;

public:
	background_sphere(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_distance(init_owner(*this) + init_name("distance") + init_label(_("distance")) + init_description(_("Distance")) + init_value(0.99) + init_constraint(k3d::data::constraint::minimum(0.0, k3d::data::constraint::maximum(1.0)))),
		m_render_final(init_owner(*this) + init_name("render_final") + init_label(_("render_final")) + init_description(_("Visible in the final rendered image")) + init_value(true))
	{
	}

	void renderman_render(const k3d::ri::render_state& State)
	{
		if(!m_render_final.pipeline_value())
			return;
			
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
	
		double radius = 0;
		if(perspective)
		{
			const double near = boost::any_cast<double>(k3d::property::pipeline_value(perspective->near()));
			const double far = boost::any_cast<double>(k3d::property::pipeline_value(perspective->far()));
			radius = k3d::mix(near, far, m_distance.pipeline_value());
		}

		if(orthographic)
		{
			const double near = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->near()));
			const double far = boost::any_cast<double>(k3d::property::pipeline_value(orthographic->far()));
			radius = k3d::mix(near, far, m_distance.pipeline_value());
		}

		// Get data from the camera ...
		const k3d::point3 camera_coords = State.camera_matrix * k3d::point3(0, 0, 0);

		State.stream.RiAttributeBegin();
		State.stream.RiAttributeV("identifier", k3d::ri::parameter_list(1, k3d::ri::parameter("name", k3d::ri::UNIFORM, 1, k3d::ri::string(name()))));
		State.stream.RiIdentity();
		State.stream.RiTranslate(camera_coords[0], camera_coords[1], camera_coords[2]);
		State.stream.RiRotate(90.0, 1.0, 0.0, 0.0);

		k3d::ri::setup_material(m_material.pipeline_value(), State);
		
		State.stream.RiSphereV(radius, -radius, radius, 360.0);
		State.stream.RiAttributeEnd();
	}

	void renderman_render_complete(const k3d::ri::render_state& State)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<background_sphere >factory(
			k3d::uuid(0x00000001, 0x00000000, 0x00000000, 0x00000019),
			"RenderManBackgroundSphere",
			_("Places a shader on a sphere, centered on the camera and aligned with world coordinates"),
			"RenderMan",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, writable_property, with_serialization) m_distance;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_render_final;
};

} // namespace

namespace libk3drenderman
{

k3d::iplugin_factory& background_sphere_factory()
{
	return background_sphere::get_factory();
}

} // namespace libk3drenderman



