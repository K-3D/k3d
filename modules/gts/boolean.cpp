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

/** \file
		\brief Performs boolean operations
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/basic_math.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/material_client.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/legacy_mesh_source.h>
#include <k3dsdk/utility.h>

#include "gts_interface.h"

namespace module
{

namespace gts
{

namespace detail
{

template<typename T>
class wrapper
{
public:
	wrapper(T* const Object) :
		m_object(Object)
	{
	}

	~wrapper()
	{
		if(m_object)
			gts_object_destroy(GTS_OBJECT(m_object));
	}

	operator T*()
	{
		return m_object;
	}

	T* operator->()
	{
		return m_object;
	}

private:
	T* const m_object;
};

template<>
class wrapper<GNode>
{
public:
	wrapper(GNode* const Object) :
		m_object(Object)
	{
	}

	~wrapper()
	{
		gts_bb_tree_destroy(m_object, TRUE);
	}

	operator GNode*()
	{
		return m_object;
	}

private:
	GNode* const m_object;
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// boolean

class boolean :
	public k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > >
{
	typedef k3d::material_client<k3d::legacy::mesh_source<k3d::persistent<k3d::node> > > base;

public:
	boolean(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_type(init_owner(*this) + init_name("type") + init_label(_("Type")) + init_description(_("Boolean operation (intersection, union, difference, reverse difference)")) + init_value(BOOLEAN_INTERSECTION) + init_enumeration(boolean_values())),
		m_input1(init_owner(*this) + init_name("input1") + init_label(_("Input 1")) + init_description(_("Input mesh 1")) + init_value<k3d::legacy::mesh*>(0)),
		m_input2(init_owner(*this) + init_name("input2") + init_label(_("Input 2")) + init_description(_("Input mesh 2")) + init_value<k3d::legacy::mesh*>(0))
	{
		m_type.changed_signal().connect(make_reset_mesh_slot());
		m_input1.changed_signal().connect(make_reset_mesh_slot());
		m_input2.changed_signal().connect(make_reset_mesh_slot());
	}

	void on_initialize_mesh(k3d::legacy::mesh& Mesh)
	{
		// Get the input geometry ...
		const k3d::legacy::mesh* const input1 = m_input1.pipeline_value();
		if(!input1)
			return;

		const k3d::legacy::mesh* const input2 = m_input2.pipeline_value();
		if(!input2)
			return;

		unsigned long polyhedron_index = 0;
		detail::wrapper<GtsSurface> surface1 = gts_polygonal_surface(*input1, polyhedron_index);
		return_if_fail(surface1);
		if(!gts_surface_is_orientable(surface1))
		{
			k3d::log() << error << "input1 is not an orientable manifold" << std::endl;
			return;
		}
		detail::wrapper<GtsSurface> self_intersection1 = gts_surface_is_self_intersecting(surface1);
		if(self_intersection1)
		{
			k3d::log() << error << "input1 self-intersects" << std::endl;
			return;
		}

		detail::wrapper<GtsSurface> surface2 = gts_polygonal_surface(*input2, polyhedron_index);
		return_if_fail(surface2);
		if(!gts_surface_is_orientable(surface2))
		{
			k3d::log() << error << "input2 is not an orientable manifold" << std::endl;
			return;
		}
		detail::wrapper<GtsSurface> self_intersection2 = gts_surface_is_self_intersecting(surface2);
		if(self_intersection2)
		{
			k3d::log() << error << "input2 self-intersects" << std::endl;
			return;
		}

		detail::wrapper<GNode> tree1 = gts_bb_tree_surface(surface1);
		const bool open1 = gts_surface_volume(surface1) < 0;

		detail::wrapper<GNode> tree2 = gts_bb_tree_surface(surface2);
		const bool open2 = gts_surface_volume(surface2) < 0;


		detail::wrapper<GtsSurfaceInter> si = gts_surface_inter_new(gts_surface_inter_class(), surface1, surface2, tree1, tree2, open1, open2);
		return_if_fail(si);

		detail::wrapper<GtsSurface> surface = gts_surface_new(gts_surface_class(), gts_face_class(), gts_edge_class(), gts_vertex_class());
		return_if_fail(surface);

		switch(m_type.pipeline_value())
		{
			case BOOLEAN_INTERSECTION:
				gts_surface_inter_boolean(si, surface, GTS_1_IN_2);
				gts_surface_inter_boolean(si, surface, GTS_2_IN_1);
				break;

			case BOOLEAN_UNION:
				gts_surface_inter_boolean(si, surface, GTS_1_OUT_2);
				gts_surface_inter_boolean(si, surface, GTS_2_OUT_1);
				break;

			case BOOLEAN_DIFFERENCE:
				gts_surface_inter_boolean(si, surface, GTS_1_OUT_2);
				gts_surface_inter_boolean(si, surface, GTS_2_IN_1);
				gts_surface_foreach_face(si->s2, (GtsFunc)gts_triangle_revert, NULL);
				gts_surface_foreach_face(surface2, (GtsFunc)gts_triangle_revert, NULL);
				break;

			case BOOLEAN_REVERSE_DIFFERENCE:
				gts_surface_inter_boolean(si, surface, GTS_2_OUT_1);
				gts_surface_inter_boolean(si, surface, GTS_1_IN_2);
				gts_surface_foreach_face(si->s1, (GtsFunc)gts_triangle_revert, NULL);
				gts_surface_foreach_face(surface1, (GtsFunc)gts_triangle_revert, NULL);
				break;
		}

		// Create output geometry ...
		copy_surface(surface, Mesh);

		k3d::imaterial* const material = m_material.pipeline_value();
		for(k3d::legacy::mesh::polyhedra_t::iterator polyhedron = Mesh.polyhedra.begin(); polyhedron != Mesh.polyhedra.end(); ++polyhedron)
		{
			for(k3d::legacy::polyhedron::faces_t::iterator face = (*polyhedron)->faces.begin(); face != (*polyhedron)->faces.end(); ++face)
				(*face)->material = material;
		}
	}

	void on_update_mesh(k3d::legacy::mesh& Mesh)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<boolean > factory(
				k3d::uuid(0xbb8e6fcc, 0x573245c3, 0xb6166c7e, 0xe4f06b86),
				"GTSBoolean",
				_("Merges polygonal surfaces using boolean operations"),
				"Polygons CSG",
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
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Intersection"), "intersection", _("Render intersecting volumes")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Union"), "union", _("Render the union of two volumnes")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Difference"), "difference", _("Render the difference of two volumes")));
			values.push_back(k3d::ienumeration_property::enumeration_value_t(_("Reverse Difference"), "reverse_difference", _("Render the difference of two volumes")));
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
			k3d::log() << k3d_file_reference << ": unknown enumeration [" << text << "]"<< std::endl;

		return Stream;
	}

	k3d_data(boolean_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_type;
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input1;
	k3d_data(k3d::legacy::mesh*, immutable_name, change_signal, no_undo, local_storage, no_constraint, read_only_property, no_serialization) m_input2;
};

/////////////////////////////////////////////////////////////////////////////
// boolean_factory

k3d::iplugin_factory& boolean_factory()
{
	return boolean::get_factory();
}

} // namespace gts

} // namespace module


