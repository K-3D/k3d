// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/inetwork_render_frame.h>
#include <k3dsdk/irender_engine_ri.h>
#include <k3dsdk/log.h>
#include <k3dsdk/node.h>
#include <k3dsdk/path.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/result.h>
#include <k3dsdk/shader_cache.h>
#include <k3dsdk/share.h>
#include <k3dsdk/system.h>

#include <sstream>

namespace module
{

namespace renderman
{

namespace engine
{

class pixie :
	public k3d::persistent<k3d::node>,
	public k3d::ri::irender_engine
{
	typedef k3d::persistent<k3d::node> base;

public:
	pixie(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	const k3d::bool_t installed()
	{
		if(k3d::system::find_executable("rndr").empty())
			return false;

		if(k3d::system::find_executable("sdrc").empty())
			return false;

		return true;
	}

	const k3d::bool_t compile_shader(const k3d::filesystem::path& Shader)
	{
		// Compute some paths that will be used by the compiler ...
		const k3d::filesystem::path shader_source_path = Shader;
		const k3d::filesystem::path shader_binary_path = k3d::shader_cache_path() / k3d::filesystem::generic_path(k3d::filesystem::replace_extension(Shader, ".sdr").leaf());
		const k3d::filesystem::path shader_source_directory = Shader.branch_path();
		const k3d::filesystem::path global_source_directory = k3d::share_path() / k3d::filesystem::generic_path("shaders");

		if(k3d::filesystem::up_to_date(shader_source_path, shader_binary_path))
			return true;

		std::ostringstream command_line;
		command_line << "sdrc";
		command_line << " -I\"" << shader_source_directory.native_filesystem_string()  << "\"";
		command_line << " -I\"" << global_source_directory.native_filesystem_string()  << "\"";
		command_line << " -o \"" << shader_binary_path.native_filesystem_string() << "\"";
		command_line << " " << shader_source_path.native_filesystem_string();

		// Make it happen ...
		return_val_if_fail(k3d::system::spawn_sync(command_line.str()), false);

		return true;
	}

	const k3d::bool_t render(k3d::inetwork_render_frame& Frame, const k3d::filesystem::path& RIB)
	{
		k3d::inetwork_render_frame::environment environment;

		k3d::inetwork_render_frame::arguments arguments;
		arguments.push_back(k3d::inetwork_render_frame::argument(RIB.native_filesystem_string()));

		Frame.add_exec_command("rndr", environment, arguments);

		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<pixie,
			k3d::interface_list<k3d::ri::irender_engine> > factory(
				k3d::uuid(0x7a1d773b, 0xa9472dae, 0xa80223ab, 0x59d62f9a),
				"PixieRenderManEngine",
				_("Provides render integration with Pixie, http://www.renderpixie.com"),
				"RenderMan",
				k3d::iplugin_factory::STABLE
				);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// pixie_factory

k3d::iplugin_factory& pixie_factory()
{
	return pixie::get_factory();
}

} // namespace engine

} // namespace renderman

} // namespace module

