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

class prman :
	public k3d::node,
	public k3d::ri::irender_engine
{
	typedef k3d::node base;

public:
	prman(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	k3d::bool_t installed()
	{
		return true;
	}

	k3d::bool_t compile_shader(const k3d::filesystem::path& Shader)
	{
		// Compute some paths that will be used by the compiler ...
		const k3d::filesystem::path shader_source_path = Shader;
		const k3d::filesystem::path shader_binary_path = k3d::shader_cache_path() / k3d::filesystem::generic_path(k3d::filesystem::replace_extension(Shader, ".slo").leaf());
		const k3d::filesystem::path shader_source_directory = Shader.branch_path();
		const k3d::filesystem::path global_source_directory = k3d::share_path() / k3d::filesystem::generic_path("shaders");

		if(k3d::filesystem::up_to_date(shader_source_path, shader_binary_path))
			return true;

		std::ostringstream command_line;
		command_line << "slcomp";
//		command_line << " -I\"" << shader_source_directory.native_filesystem_string()  << "\"";
//		command_line << " -I\"" << global_source_directory.native_filesystem_string()  << "\"";
//		command_line << " -o \"" << shader_binary_path.native_filesystem_string() << "\"";
		command_line << " \"" << shader_source_path.native_filesystem_string() << "\"";

		// Make it happen ...
		return_val_if_fail(k3d::system::spawn_sync(command_line.str()), false);

		return true;
	}

	k3d::bool_t render(k3d::inetwork_render_frame& Frame, const k3d::filesystem::path& RIB)
	{
		k3d::inetwork_render_frame::environment environment;

		k3d::inetwork_render_frame::arguments arguments;
		arguments.push_back(k3d::inetwork_render_frame::argument("-progress"));
		arguments.push_back(k3d::inetwork_render_frame::argument(RIB.native_filesystem_string()));

		Frame.add_exec_command("prman", environment, arguments);

		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<prman,
			k3d::interface_list<k3d::ri::irender_engine> > factory(
				k3d::uuid(0x69186ecd, 0x92440853, 0x24ed05a9, 0xa5a168d3),
				"PRManRenderManEngine",
				_("Provides render integration with PRMan, http://renderman.pixar.com"),
				"RenderMan",
				k3d::iplugin_factory::STABLE
				);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// prman_factory

k3d::iplugin_factory& prman_factory()
{
	return prman::get_factory();
}

} // namespace engine

} // namespace renderman

} // namespace module

