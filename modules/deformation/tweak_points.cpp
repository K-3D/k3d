// K-3D
// Copyright (c) 2005, Romain Behar
//
// Contact: romainbehar@yahoo.com
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
		\author Romain Behar (romainbehar@yahoo.com)
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>

namespace libk3ddeformation
{

/////////////////////////////////////////////////////////////////////////////
// tweak_points

class tweak_points :
	public k3d::mesh_simple_deformation_modifier
{
	typedef k3d::mesh_simple_deformation_modifier base;
	typedef std::vector<k3d::point3> tweaks_t;

public:
	tweak_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_tweaks(init_owner(*this) + init_name("tweaks") + init_label(_("Tweaks")) + init_description(_("Tweak array translates each point in a custom diretion, those values are set by the application")) + init_value(tweaks_t()))
	{
		m_tweaks.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const tweaks_t tweaks = m_tweaks.pipeline_value();

		const size_t point_begin = 0;
		const size_t point_end = point_begin + std::min(tweaks.size(), OutputPoints.size());
		for(size_t point = point_begin; point != point_end; ++point)
			OutputPoints[point] = InputPoints[point] + tweaks[point];
	}

	void save(k3d::xml::element& Element, const k3d::ipersistent::save_context& Context)
	{
		base::save(Element, Context);

		k3d::xml::element& tweaks = Element.append(k3d::xml::element("tweaks"));

		// tweak_array -ought- to be a copy, since m_tweaks.pipeline_value() returns a -temporary- object
		const tweaks_t tweak_array = m_tweaks.pipeline_value();
		for(tweaks_t::const_iterator tweak_i = tweak_array.begin(); tweak_i != tweak_array.end(); ++tweak_i)
			tweaks.append(k3d::xml::element("tweak", k3d::xml::attribute("value", *tweak_i)));
	}

	void load(k3d::xml::element& Element, const k3d::ipersistent::load_context& Context)
	{
		base::load(Element, Context);

		k3d::xml::element* const tweaks = k3d::xml::find_element(Element, "tweaks");
		return_if_fail(tweaks);

		tweaks_t loaded_tweaks;
		for(k3d::xml::element::elements_t::iterator element = tweaks->children.begin(); element != tweaks->children.end(); ++element)
        {
            if(element->name == "tweak")
            {
                loaded_tweaks.push_back(k3d::xml::attribute_value<k3d::point3>(*element, "value", k3d::point3(0, 0, 0)));
            }
            else
            {
                k3d::log() << warning << k3d_file_reference << ": unknown element \"" << element->name << "\" will be ignored ... " << std::endl;
            }
        }

		m_tweaks.set_value(loaded_tweaks);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<tweak_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xed302b87, 0x49bf4fe6, 0x99064963, 0x17ec12d9),
				"TweakPoints",
				_("Translates each mesh point with custom vector"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(tweaks_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, no_serialization) m_tweaks;
};

/////////////////////////////////////////////////////////////////////////////
// tweak_points_factory

k3d::iplugin_factory& tweak_points_factory()
{
	return tweak_points::get_factory();
}

} // namespace libk3ddeformation


