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
#include <k3dsdk/string_cast.h>
#include <k3dsdk/types.h>
#include <k3dsdk/user_interface.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

namespace module
{

namespace glx
{

class mesa_version_check : public k3d::iunknown
{
public:
	mesa_version_check()
	{
		if(!glewGetContext())
		{
			k3d::log() << warning << "No context defined to determine Mesa version" << std::endl;
			return;
		}
	
		const k3d::string_t version(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
		const k3d::string_t vendor(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		k3d::log() << info << "OpenGL version: " << version << ", vendor: " << vendor << std::endl;

		try
		{
			if(vendor == "Tungsten Graphics, Inc")
			{
				std::vector<k3d::string_t> tokens;
				boost::split(tokens, version, boost::is_any_of(" "));
				if(tokens.size() > 2 && tokens[1] == "Mesa")
				{
					const k3d::string_t version_number = tokens[2];
					std::vector<k3d::string_t> bad_tokens; // Strip off any trailing '-devel' and so on
					boost::split(bad_tokens, version_number, boost::is_any_of("-"));
					std::vector<k3d::string_t> version_tokens;
					boost::split(version_tokens, bad_tokens[0], boost::is_any_of("."));
					if(version_tokens.size() > 1)
					{
						const k3d::uint_t min_major = 7; // minimal major version
						const k3d::uint_t min_minor = 5; // minimal minor version
						const k3d::uint_t min_patch = 1; // minimal patch version
						const k3d::uint_t major = boost::lexical_cast<k3d::uint_t>(version_tokens[0]);
						const k3d::uint_t minor = boost::lexical_cast<k3d::uint_t>(version_tokens[1]);
						if(major < min_major || (major == min_major && minor < min_minor) || (major == min_major && minor == min_minor && (version_tokens.size() == 2 || boost::lexical_cast<k3d::uint_t>(version_tokens[2]) < min_patch)))
						{
							k3d::user_interface().nag_message(
								"MesaVersionCheck.1",
								k3d::ustring::from_utf8(k3d::string_cast(boost::format(_("Your OpenGL version is %1%.")) % version)),
								k3d::ustring::from_utf8(_("On your platform, Mesa versions earlier than 7.5.1 are known to cause problems with face selection. If you experience this, please upgrade your Mesa library. See http://bugs.freedesktop.org/show_bug.cgi?id=16866 for detailed information.")));
						}
					}
				}
			}
		}
		catch(std::runtime_error& E)
		{
			k3d::log() << warning << "error parsing mesa version: " << E.what() << std::endl;
		}
	}
	
	static k3d::iplugin_factory& get_factory()
	{
		k3d::iplugin_factory::metadata_t metadata;
		metadata["k3d:application-start"] = "";
		static k3d::application_plugin_factory<mesa_version_check,
			k3d::interface_list<k3d::iunknown> > factory(
				k3d::uuid(0xaf73ea67, 0x444f8519, 0x9bddf492, 0x305853a8),
				"MesaVersionCheck",
				_("Checks Mesa version and warns if it is known to contain bugs affecting K-3D"),
				"Desktop",
				k3d::iplugin_factory::EXPERIMENTAL,
				metadata);

		return factory;
	}
};

k3d::iplugin_factory& mesa_version_check_factory()
{
	return mesa_version_check::get_factory();
}

} // namespace glx

} // namespace module
