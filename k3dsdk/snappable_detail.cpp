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
		\brief Declares isnappable, an abstract interface implemented by objects to which other objects can be interactively "snapped"
		\author Tim Shead (tshead@k-3d.com)
*/

#include "explicit_snap_source.h"
#include "explicit_snap_target.h"
#include "ideletable.h"
#include "isnap_source.h"
#include "isnap_target.h"
#include "result.h"
#include "snappable_detail.h"
#include "xml.h"

namespace k3d
{

namespace detail
{

struct delete_snap_object
{
	void operator()(isnap_source* Object)
	{
		delete dynamic_cast<ideletable*>(Object);
	}

	void operator()(isnap_target* Object)
	{
		delete dynamic_cast<ideletable*>(Object);
	}
};

} // nsmespace detail

snappable_detail::~snappable_detail()
{
	std::for_each(m_snap_sources.begin(), m_snap_sources.end(), detail::delete_snap_object());
	std::for_each(m_snap_targets.begin(), m_snap_targets.end(), detail::delete_snap_object());
}

const isnappable::snap_sources_t snappable_detail::snap_sources()
{
	return m_snap_sources;
}

const isnappable::snap_targets_t snappable_detail::snap_targets()
{
	return m_snap_targets;
}

void snappable_detail::add_snap_source(isnap_source* const SnapSource)
{
	return_if_fail(std::find(m_snap_sources.begin(), m_snap_sources.end(), SnapSource) == m_snap_sources.end());
	m_snap_sources.push_back(SnapSource);
}

void snappable_detail::add_snap_target(isnap_target* const SnapTarget)
{
	return_if_fail(std::find(m_snap_targets.begin(), m_snap_targets.end(), SnapTarget) == m_snap_targets.end());
	m_snap_targets.push_back(SnapTarget);
}

void snappable_detail::save(xml::element& Element, const ipersistent::save_context& Context)
{
	xml::element* xml_snap_sources = 0;
	for(snap_sources_t::const_iterator snap_source = m_snap_sources.begin(); snap_source != m_snap_sources.end(); ++snap_source)
	{
		if(explicit_snap_source* const source = dynamic_cast<explicit_snap_source*>(*snap_source))
		{
			if(!xml_snap_sources)
				xml_snap_sources = &Element.append(xml::element("snap_sources"));

			xml::element& xml_snap_source = xml_snap_sources->append(xml::element("snap_source"));
			xml_snap_source.append(xml::attribute("label", source->m_label));
			xml_snap_source.append(xml::attribute("position", source->m_position));
			if(source->m_look.get())
				xml_snap_source.append(xml::attribute("look", *source->m_look));
			if(source->m_up.get())
				xml_snap_source.append(xml::attribute("up", *source->m_up));

			if(source->m_groups.size())
			{
				xml::element& xml_groups = xml_snap_source.append(xml::element("groups"));
				for(isnap_source::groups_t::iterator group = source->m_groups.begin(); group != source->m_groups.end(); ++group)
					xml_groups.append(xml::element("group", *group));
			}
		}
	}

	xml::element* xml_snap_targets = 0;
	for(snap_targets_t::const_iterator snap_target = m_snap_targets.begin(); snap_target != m_snap_targets.end(); ++snap_target)
	{
		if(explicit_snap_target* const target = dynamic_cast<explicit_snap_target*>(*snap_target))
		{
			if(!xml_snap_targets)
				xml_snap_targets = &Element.append(xml::element("snap_targets"));

			xml::element& xml_snap_target = xml_snap_targets->append(xml::element("snap_target"));
			xml_snap_target.append(xml::attribute("label", target->m_label));
			xml_snap_target.append(xml::attribute("position", target->m_position));
			if(target->m_look.get())
				xml_snap_target.append(xml::attribute("look", *target->m_look));
			if(target->m_up.get())
				xml_snap_target.append(xml::attribute("up", *target->m_up));

			if(target->m_groups.size())
			{
				xml::element& xml_groups = xml_snap_target.append(xml::element("groups"));
				for(isnap_source::groups_t::iterator group = target->m_groups.begin(); group != target->m_groups.end(); ++group)
					xml_groups.append(xml::element("group", *group));
			}
		}
	}
}

void snappable_detail::load(xml::element& Element, const ipersistent::load_context& Context)
{
	if(xml::element* const xml_snap_sources = xml::find_element(Element, "snap_sources"))
	{
		for(xml::element::elements_t::iterator xml_snap_source = xml_snap_sources->children.begin(); xml_snap_source != xml_snap_sources->children.end(); ++xml_snap_source)
		{
			if(xml_snap_source->name != "snap_source")
				continue;

			const std::string xml_label = xml::attribute_text(*xml_snap_source, "label");
			if(xml_label.empty())
			{
				k3d::log() << error << "snap source without label will be ignored" << std::endl;
				continue;
			}
			
			const std::string xml_position = xml::attribute_text(*xml_snap_source, "position");
			if(xml_position.empty())
			{
				k3d::log() << error << "snap source [" << xml_label << "] without position will be ignored" << std::endl;
				continue;
			}
			
			const std::string xml_look = xml::attribute_text(*xml_snap_source, "look");
			const std::string xml_up = xml::attribute_text(*xml_snap_source, "up");

			explicit_snap_source* snap_source = 0;
			
			if(xml_look.empty() || xml_up.empty())
			{
				snap_source = new explicit_snap_source(
					xml_label,
					from_string<k3d::point3>(xml_position, k3d::point3(0, 0, 0)));
			}
			else
			{
				snap_source = new explicit_snap_source(
					xml_label,
					from_string<k3d::point3>(xml_position, k3d::point3(0, 0, 0)),
					from_string<k3d::vector3>(xml_look, k3d::vector3(0, 0, 1)),
					from_string<k3d::vector3>(xml_up, k3d::vector3(0, 1, 0)));
			}

			if(xml::element* const xml_groups = xml::find_element(*xml_snap_source, "groups"))
			{
				for(xml::element::elements_t::iterator xml_group = xml_groups->children.begin(); xml_group != xml_groups->children.end(); ++xml_group)
				{
					if(xml_group->name != "group")
						continue;
					snap_source->add_group(xml_group->text);
				}
			}
			
			add_snap_source(snap_source);
		}
	}
	
	if(xml::element* const xml_snap_targets = xml::find_element(Element, "snap_targets"))
	{
		for(xml::element::elements_t::iterator xml_snap_target = xml_snap_targets->children.begin(); xml_snap_target != xml_snap_targets->children.end(); ++xml_snap_target)
		{
			if(xml_snap_target->name != "snap_target")
				continue;

			const std::string xml_label = xml::attribute_text(*xml_snap_target, "label");
			if(xml_label.empty())
			{
				k3d::log() << error << "snap target without label will be ignored" << std::endl;
				continue;
			}
			
			const std::string xml_position = xml::attribute_text(*xml_snap_target, "position");
			if(xml_position.empty())
			{
				k3d::log() << error << "snap target [" << xml_label << "] without position will be ignored" << std::endl;
				continue;
			}
			
			const std::string xml_look = xml::attribute_text(*xml_snap_target, "look");
			const std::string xml_up = xml::attribute_text(*xml_snap_target, "up");

			explicit_snap_target* snap_target = 0;

			if(xml_look.empty() || xml_up.empty())
			{
				snap_target = new explicit_snap_target(
					xml_label,
					from_string<k3d::point3>(xml_position, k3d::point3(0, 0, 0)));
			}
			else
			{
				snap_target = new explicit_snap_target(
					xml_label,
					from_string<k3d::point3>(xml_position, k3d::point3(0, 0, 0)),
					from_string<k3d::vector3>(xml_look, k3d::vector3(0, 0, 1)),
					from_string<k3d::vector3>(xml_up, k3d::vector3(0, 1, 0)));
			}

			if(xml::element* const xml_groups = xml::find_element(*xml_snap_target, "groups"))
			{
				for(xml::element::elements_t::iterator xml_group = xml_groups->children.begin(); xml_group != xml_groups->children.end(); ++xml_group)
				{
					if(xml_group->name != "group")
						continue;
					snap_target->add_group(xml_group->text);
				}
			}
			
			add_snap_target(snap_target);
		}
	}
}

} // namespace k3d

