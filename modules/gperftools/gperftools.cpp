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
	\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/log.h>
#include <k3dsdk/options.h>
#include <k3dsdk/path.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/system.h>
#include <k3dsdk/types.h>
#include <k3dsdk/user_interface.h>

#include <gperftools/profiler.h>

namespace module
{

namespace gperftools
{

// Optional config item to set the profile file directory
inline const k3d::string_t profile_files() { return "profile_files"; }

class gperftools : public k3d::iunknown
{
public:
	gperftools()
	{
		const k3d::string_t prefix = "k3d-profile";
		const k3d::string_t suffix = "pprof";

		k3d::filesystem::path profile_path = k3d::options::get_path(profile_files());
		if(profile_path.empty())
			profile_path = k3d::system::get_temp_directory();

		// Ensure that the profile gets a unique name ...
		k3d::uint_t  index = 0;
		k3d::string_t profile_name(prefix + "-" + k3d::string_cast(index++) + "." + suffix);
		while(k3d::filesystem::exists(profile_path / k3d::filesystem::generic_path(profile_name)))
			profile_name = prefix + "-" + k3d::string_cast(index++) + "." + suffix;

		k3d::log() << info << "gperftools: saving profile data to file " << (profile_path / k3d::filesystem::generic_path(profile_name)).native_console_string() << std::endl;

		ProfilerStart((profile_path / k3d::filesystem::generic_path(profile_name)).native_filesystem_string().c_str());
	}

	~gperftools()
	{
		k3d::log() << info << "gperftools: Stopping profiler" << std::endl;
		ProfilerStop();
	}

	static k3d::iplugin_factory& get_factory()
	{
		k3d::iplugin_factory::metadata_t metadata;
		metadata["k3d:application-start"] = "";
		static k3d::application_plugin_factory<gperftools,
			k3d::interface_list<k3d::iunknown> > factory(
				k3d::uuid(0x2b517d24, 0x88462d22, 0x5401a896, 0xdd48c985),
				"GPerftools",
				_("Starts CPU profiling using GPerftools"),
				"Development",
				k3d::iplugin_factory::EXPERIMENTAL,
				metadata);

		return factory;
	}
};

k3d::iplugin_factory& gperftools_factory()
{
	return gperftools::get_factory();
}

} // namespace gperftools

} // namespace module
