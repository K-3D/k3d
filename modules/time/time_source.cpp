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

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>

#include <limits>

namespace module
{

namespace time
{

/////////////////////////////////////////////////////////////////////////////
// time_source

class time_source :
	public k3d::persistent<k3d::node>
{
	typedef k3d::persistent<k3d::node>  base;

public:
	time_source(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_start_time(init_owner(*this) + init_name("start_time") + init_label(_("Start Time")) + init_description(_("Controls the start time (in seconds) for rendering animations.")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::time))),
		m_end_time(init_owner(*this) + init_name("end_time") + init_label(_("End Time")) + init_description(_("Controls the end time (in seconds) for rendering animations.")) + init_value(4.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::time))),
		m_frame_rate(init_owner(*this) + init_name("frame_rate") + init_label(_("Frame Rate")) + init_description(_("Controls the frame rate (in frames-per-second) for rendering animations.")) + init_value(30.0) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(std::numeric_limits<double>::epsilon()))),
		m_time(init_owner(*this) + init_name("time") + init_label(_("Time")) + init_description(_("Controls the current time displayed in the viewports.")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::time)))
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<time_source > factory(
			k3d::classes::TimeSource(),
			"TimeSource",
			_("Provides a time source for a K-3D document"),
			"Animation",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_start_time;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_end_time;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_frame_rate;
	k3d_data(double, immutable_name, change_signal, no_undo, local_storage, no_constraint, measurement_property, with_serialization) m_time;
};

/////////////////////////////////////////////////////////////////////////////
// time_source_factory

k3d::iplugin_factory& time_source_factory()
{
	return time_source::get_factory();
}

} // namespace time

} // namespace module

