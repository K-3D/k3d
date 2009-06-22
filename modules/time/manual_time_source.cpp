// K-3D
// Copyright (c) 1995-2009, Timothy M. Shead
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
#include <k3dsdk/hints.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/value_demand_storage.h>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace module
{

namespace time
{

/////////////////////////////////////////////////////////////////////////////
// manual_time_source

class manual_time_source :
	public k3d::node
{
	typedef k3d::node base;

public:
	manual_time_source(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_year(init_owner(*this) + init_name("year") + init_label(_("Year")) + init_description(_("Year.")) + init_value(2009)),
		m_month(init_owner(*this) + init_name("month") + init_label(_("Month")) + init_description(_("Month.")) + init_value(1)),
		m_day(init_owner(*this) + init_name("day") + init_label(_("Day")) + init_description(_("Day.")) + init_value(1)),
		m_hour(init_owner(*this) + init_name("hour") + init_label(_("Hour")) + init_description(_("Hour.")) + init_value(0)),
		m_minute(init_owner(*this) + init_name("minute") + init_label(_("Minute")) + init_description(_("Minute.")) + init_value(0)),
		m_second(init_owner(*this) + init_name("second") + init_label(_("Second")) + init_description(_("Second.")) + init_value(0.0)),
		m_time(init_owner(*this) + init_name("time") + init_label(_("Time")) + init_description(_("Time")) + init_value(0.0))
	{
		m_year.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_time.make_slot()));
		m_month.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_time.make_slot()));
		m_day.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_time.make_slot()));
		m_hour.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_time.make_slot()));
		m_minute.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_time.make_slot()));
		m_second.changed_signal().connect(k3d::hint::converter<
			k3d::hint::convert<k3d::hint::any, k3d::hint::none> >(m_time.make_slot()));

		m_time.set_update_slot(sigc::mem_fun(*this, &manual_time_source::execute));
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<manual_time_source > factory(
			k3d::uuid(0xa3f7be22, 0x264d44fb, 0x18849fac, 0x8cdc9e37),
			"ManualTimeSource",
			_("Provides a time source for a K-3D document"),
			"Animation",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

private:
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_year;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_month;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_day;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_hour;
	k3d_data(k3d::int32_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_minute;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_second;
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_time;

	/// Called whenever the output time has been modified and needs to be updated.
	void execute(const std::vector<k3d::ihint*>& Hints, k3d::double_t& Time)
	{
		const k3d::int32_t year = m_year.pipeline_value();
		const k3d::int32_t month = m_month.pipeline_value();
		const k3d::int32_t day = m_day.pipeline_value();
		const k3d::int32_t hour = m_hour.pipeline_value();
		const k3d::int32_t minute = m_minute.pipeline_value();

		const k3d::double_t second = m_second.pipeline_value();
		const k3d::double_t whole_second = std::floor(second);
		const k3d::double_t fractional_second = second - whole_second;	

		const boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
		const boost::posix_time::ptime output(
			boost::gregorian::date(year, month, day),
			boost::posix_time::time_duration(hour, minute, second, fractional_second * boost::posix_time::time_duration::ticks_per_second()));
		const boost::posix_time::time_duration elapsed = output - epoch;

		Time = elapsed.total_seconds();
		Time += static_cast<k3d::double_t>(elapsed.fractional_seconds()) / static_cast<k3d::double_t>(elapsed.ticks_per_second());
	}
};

/////////////////////////////////////////////////////////////////////////////
// manual_time_source_factory

k3d::iplugin_factory& manual_time_source_factory()
{
	return manual_time_source::get_factory();
}

} // namespace time

} // namespace module

