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
		\brief Implements the CSG Operator K-3D object, which allows geometry to be rendered with boolean effects
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_ri.h>

#include <iostream>

namespace libk3drenderman
{

/////////////////////////////////////////////////////////////////////////////
// csg_operator

/// Allows geometry to be rendered with boolean effects
class csg_operator :
	public k3d::persistent<k3d::node> ,
	public k3d::ri::irenderable
{
	typedef k3d::persistent<k3d::node>  base;

public:
	csg_operator(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_type(init_owner(*this) + init_name("type") + init_label(_("type")) + init_description(_("Operation")) + init_enumeration(boolean_values()) + init_value(BOOLEAN_INTERSECTION)),
		m_instance1(init_owner(*this) + init_name("instance1") + init_label(_("instance1")) + init_description(_("Instance1")) + init_value<k3d::ri::irenderable*>(0)),
		m_instance2(init_owner(*this) + init_name("instance2") + init_label(_("instance2")) + init_description(_("Instance2")) + init_value<k3d::ri::irenderable*>(0))
	{
	}

	void renderman_render(const k3d::ri::render_state& State)
	{
		// We only generate RIB for the last sample ...
		if(!k3d::ri::last_sample(State))
			return;

		k3d::ri::irenderable* const renderable1 = m_instance1.pipeline_value();
		k3d::ri::irenderable* const renderable2 = m_instance2.pipeline_value();
		if(!renderable1 || !renderable2)
			return;

		// Make sure we don't enter an infinite loop trying to render ourself ...
		if(renderable1 == this || renderable2 == this)
		{
			k3d::log() << error << factory().name() << " [" << name() << "] cannot instance itself" << std::endl;
			return;
		}

		// Record the type of boolean ...
		k3d::ri::render_state state(State);
		state.render_context = k3d::ri::CSG_SOLID;

		switch(m_type.pipeline_value())
		{
			case BOOLEAN_INTERSECTION:
				State.stream.RiSolidBegin(k3d::ri::RI_INTERSECTION());
				renderable1->renderman_render(state);
				renderable2->renderman_render(state);
				break;

			case BOOLEAN_UNION:
				State.stream.RiSolidBegin(k3d::ri::RI_UNION());
				renderable1->renderman_render(state);
				renderable2->renderman_render(state);
				break;

			case BOOLEAN_DIFFERENCE:
				State.stream.RiSolidBegin(k3d::ri::RI_DIFFERENCE());
				renderable1->renderman_render(state);
				renderable2->renderman_render(state);
				break;

			case BOOLEAN_REVERSE_DIFFERENCE:
				State.stream.RiSolidBegin(k3d::ri::RI_DIFFERENCE());
				renderable2->renderman_render(state);
				renderable1->renderman_render(state);
				break;
		}

		State.stream.RiSolidEnd();
	}

	void renderman_render_complete(const k3d::ri::render_state& State)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<csg_operator >factory(
			k3d::uuid(0x00000001, 0x00000000, 0x00000000, 0x0000000a),
			"RenderManCSGOperator",
			_("Combines solids using boolean operators at render-time"),
			"RenderMan CSG",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	typedef enum
	{
		BOOLEAN_INTERSECTION,
		BOOLEAN_UNION,
		BOOLEAN_DIFFERENCE,
		BOOLEAN_REVERSE_DIFFERENCE
	} boolean_t;

	static const k3d::ienumeration_property::enumeration_values_t& boolean_values()
	{
		static k3d::ienumeration_property::enumeration_values_t values;
		if(values.empty())
		{
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Intersection", "intersection", "Render intersecting volumes"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Union", "union", "Render the union of two volumnes"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Difference", "difference", "Render the difference of two volumes"));
			values.push_back(k3d::ienumeration_property::enumeration_value_t("Reverse Difference", "reverse_difference", "Render the difference of two volumes"));
		}
		
		return values;
	}
	
	friend std::ostream& operator<<(std::ostream& Stream, const boolean_t& Value)
	{
		switch(Value)
		{
			case BOOLEAN_UNION:
				Stream << "union";
				break;
			case BOOLEAN_INTERSECTION:
				Stream << "intersection";
				break;
			case BOOLEAN_DIFFERENCE:
				Stream << "difference";
				break;
			case BOOLEAN_REVERSE_DIFFERENCE:
				Stream << "reverse_difference";
				break;
		}

		return Stream;
	}

	friend std::istream& operator>>(std::istream& Stream, boolean_t& Value)
	{
		std::string text;
		Stream >> text;

		if(text == "union")
			Value = BOOLEAN_UNION;
		else if(text == "intersection")
			Value = BOOLEAN_INTERSECTION;
		else if(text == "difference")
			Value = BOOLEAN_DIFFERENCE;
		else if(text == "reverse_difference")
			Value = BOOLEAN_REVERSE_DIFFERENCE;
		else
			k3d::log() << error << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

		return Stream;
	}

	k3d_data(boolean_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
	k3d_data(k3d::ri::irenderable*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_instance1;
	k3d_data(k3d::ri::irenderable*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_instance2;
};

k3d::iplugin_factory& csg_operator_factory()
{
	return csg_operator::get_factory();
}

} // namespace libk3drenderman



