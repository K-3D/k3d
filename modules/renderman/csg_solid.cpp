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
		\brief Implements the csg_solid K-3D object, which allows geometry to be rendered with boolean effects
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_ri.h>

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////////////////////////
// csg_solid

class csg_solid :
	public k3d::persistent<k3d::node> ,
	public k3d::ri::irenderable
{
	typedef k3d::persistent<k3d::node>  base;
	
public:
	csg_solid(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_instance(init_owner(*this) + init_name("instance") + init_label(_("instance")) + init_description(_("Instance")) + init_value<k3d::ri::irenderable*>(0))
	{
	}

	void renderman_render(const k3d::ri::render_state& State)
	{
		// We only generate RIB during CSG rendering ...
		if(State.render_context != k3d::ri::CSG_SOLID)
			return;
			
		// We only generate RIB on the last sample ...
		if(!k3d::ri::last_sample(State))
			return;

		k3d::ri::irenderable* const renderable = m_instance.pipeline_value();
		if(!renderable)
			return;

		// Make sure we don't enter an infinite loop trying to render ourself ...
		if(renderable == this)
			{
				k3d::log() << error << factory().name() << " [" << name() << "] cannot instance itself" << std::endl;
				return;
			}

		State.stream.RiSolidBegin(k3d::ri::RI_PRIMITIVE());
			k3d::ri::render_state state(State);
			state.render_context = k3d::ri::CSG_SOLID;
			renderable->renderman_render(state);
		State.stream.RiSolidEnd();
	}

	void renderman_render_complete(const k3d::ri::render_state& State)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<csg_solid >factory(
			k3d::uuid(0x00000001, 0x00000000, 0x00000000, 0x0000007b),
			"RenderManCSGSolid",
			_("Designates a solid volume at render-time"),
			"RenderMan CSG",
			k3d::iplugin_factory::STABLE);

		return factory;	
	}

private:
	k3d_data(k3d::ri::irenderable*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_instance;
};

k3d::iplugin_factory& csg_solid_factory()
{
	return csg_solid::get_factory();
}

} // namespace libk3drenderman

