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
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/isnap_source.h>
#include <k3dsdk/isnap_target.h>
#include <k3dsdk/isnappable.h>
#include <k3dsdk/itransform_sink.h>
#include <k3dsdk/itransform_source.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/transform.h>

namespace libk3dcore
{

/////////////////////////////////////////////////////////////////////////////
// get_snap_source

k3d::isnap_source* get_snap_source(k3d::iunknown* Snappable)
{
	k3d::isnappable* const snappable = dynamic_cast<k3d::isnappable*>(Snappable);
	if(!snappable)
		return 0;

	const k3d::isnappable::snap_sources_t sources = snappable->snap_sources();
	if(sources.empty())
		return 0;

	return sources[0];
}

//////////////////////////////////////////////////////////////////////////////
// get_snap_target

k3d::isnap_target* get_snap_target(k3d::iunknown* Snappable)
{
	k3d::isnappable* const snappable = dynamic_cast<k3d::isnappable*>(Snappable);
	if(!snappable)
		return 0;

	const k3d::isnappable::snap_targets_t targets = snappable->snap_targets();
	if(targets.empty())
		return 0;

	return targets[0];
}

////////////////////////////////////////////////////////////////////////////////////
// snap

class snap :
	public k3d::node,
	public k3d::itransform_source,
	public k3d::itransform_sink
{
	typedef k3d::node base;

public:
	snap(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_input_matrix(init_owner(*this) + init_name("input_matrix") + init_label(_("Input matrix")) + init_description(_("Input matrix")) + init_value(k3d::identity3())),
		m_output_matrix(init_owner(*this) + init_name("output_matrix") + init_label(_("Output matrix")) + init_description(_("Read only")) + init_slot(sigc::mem_fun(*this, &snap::output_value))),
		m_source(init_owner(*this) + init_name("source") + init_label(_("Source Node")) + init_description(_("Source Node")) + init_value(static_cast<k3d::isnappable*>(0))),
		m_snap_source(init_owner(*this) + init_name("snap_source") + init_label(_("Snap Source")) + init_description(_("Snap Source")) + init_value(std::string("-- None --")) + init_values(m_snap_sources)),
		m_target(init_owner(*this) + init_name("target") + init_label(_("Target Node")) + init_description(_("Target Node")) + init_value(static_cast<k3d::isnappable*>(0))),
		m_snap_target(init_owner(*this) + init_name("snap_target") + init_label(_("Snap Target")) + init_description(_("Snap Target")) + init_value(std::string("-- None --")) + init_values(m_snap_targets)),
		m_snap_orientation(init_owner(*this) + init_name("snap_orientation") + init_label(_("Snap Orientation")) + init_description(_("Snap Orientation")) + init_value(true))
	{
		m_source.changed_signal().connect(sigc::mem_fun(*this, &snap::on_source_changed));
		m_target.changed_signal().connect(sigc::mem_fun(*this, &snap::on_target_changed));

		m_input_matrix.changed_signal().connect(m_output_matrix.make_reset_slot());
		m_source.changed_signal().connect(m_output_matrix.make_reset_slot());
		m_snap_source.changed_signal().connect(m_output_matrix.make_reset_slot());
		m_target.changed_signal().connect(m_output_matrix.make_reset_slot());
		m_snap_target.changed_signal().connect(m_output_matrix.make_reset_slot());
		m_snap_orientation.changed_signal().connect(m_output_matrix.make_reset_slot());

		on_source_changed(0);
		on_target_changed(0);
	}

	void on_source_changed(k3d::iunknown*)
	{
		m_snap_sources.clear();
		m_snap_sources.push_back(k3d::ienumeration_property::enumeration_value_t("-- None --", "-- None --", "-- None --"));

		if(k3d::isnappable* const snappable = m_source.pipeline_value())
		{
			const k3d::isnappable::snap_sources_t sources = snappable->snap_sources();
			for(k3d::isnappable::snap_sources_t::const_iterator source = sources.begin(); source != sources.end(); ++source)
				m_snap_sources.push_back(k3d::ienumeration_property::enumeration_value_t((*source)->label(), (*source)->label(), (*source)->label()));
		}

		m_snap_source.notify_enumeration_values_changed();
	}

	void on_target_changed(k3d::iunknown*)
	{
		m_snap_targets.clear();
		m_snap_targets.push_back(k3d::ienumeration_property::enumeration_value_t("-- None --", "-- None --", "-- None --"));

		if(k3d::isnappable* const snappable = m_target.pipeline_value())
		{
			const k3d::isnappable::snap_targets_t targets = snappable->snap_targets();
			for(k3d::isnappable::snap_targets_t::const_iterator target = targets.begin(); target != targets.end(); ++target)
				m_snap_targets.push_back(k3d::ienumeration_property::enumeration_value_t((*target)->label(), (*target)->label(), (*target)->label()));
		}

		m_snap_target.notify_enumeration_values_changed();
	}

	k3d::iproperty& transform_source_output()
	{
		return m_output_matrix;
	}

	k3d::iproperty& transform_sink_input()
	{
		return m_input_matrix;
	}

	k3d::matrix4 output_value()
	{
		const k3d::matrix4 input_matrix = m_input_matrix.pipeline_value();
		k3d::isnappable* const source_node = m_source.pipeline_value();
		k3d::isnappable* const target_node = m_target.pipeline_value();

		k3d::isnap_source* const source = get_snap_source(source_node);
		if(!source)
			return input_matrix;

		k3d::isnap_target* const target = get_snap_target(target_node);
		if(!target)
			return input_matrix;

		const k3d::matrix4 source_matrix = k3d::node_to_world_matrix(*source_node);
		const k3d::matrix4 target_matrix = k3d::node_to_world_matrix(*target_node);

		const k3d::point3 source_position = k3d::inverse(target_matrix) * source_matrix * source->source_position();

		k3d::point3 target_position;
		if(!target->target_position(source_position, target_position))
			return input_matrix;

/*
		if(m_snap_orientation.pipeline_value())
		{
			k3d::vector3 source_orientation;
			if(source->source_orientation(source_orientation))
			{
				source_orientation = k3d::inverse(target_matrix) * source_matrix * source_orientation;

				// Calculate the target orientation in world coordinates
				k3d::vector3 target_orientation;
				if(target->target_orientation(source_position, target_orientation))
				{
					k3d::matrix4 rotation = k3d::rotate3(k3d::angle_axis(
						std::acos(k3d::normalize(source_orientation) * k3d::normalize(target_orientation)),
						source_orientation ^ target_orientation));

					return k3d::translate3(target_position - source_position) * input_matrix * rotation;
				}
			}
		}
*/

		return k3d::translate3(target_position - source_position) * input_matrix;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<snap,
			k3d::interface_list<k3d::itransform_source,
			k3d::interface_list<k3d::itransform_sink > > > factory(
				k3d::uuid(0x176d4553, 0x65fc48ca, 0x845a8160, 0xd31b41ae),
				"Snap",
				_("Snaps one node to another"),
				"Snap",
				k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

	k3d_data(k3d::matrix4, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_input_matrix;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, computed_storage, no_constraint, read_only_property, no_serialization) m_output_matrix;
	k3d_data(k3d::isnappable*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_source;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_snap_source;
	k3d_data(k3d::isnappable*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_target;
	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_snap_target;
	k3d_data(bool, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_snap_orientation;

	k3d::ienumeration_property::enumeration_values_t m_snap_sources;
	k3d::ienumeration_property::enumeration_values_t m_snap_targets;
};

/////////////////////////////////////////////////////////////////////////////
// snap_factory

k3d::iplugin_factory& snap_factory()
{
	return snap::get_factory();
}

} // namespace libk3dcore

