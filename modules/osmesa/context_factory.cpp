// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/context_gl.h>
#include <k3dsdk/icontext_factory_gl.h>
#include <k3dsdk/module.h>
#include <k3dsdk/opengl/api.h>
#include <k3dsdk/result.h>

#include <iterator>
#include <stdexcept>

#include <GL/osmesa.h>

#include <dlfcn.h>

namespace module
{

namespace osmesa
{

typedef OSMesaContext (*OSMesaCreateContext)(GLenum, OSMesaContext);
typedef void (*OSMesaDestroyContext)(OSMesaContext);
typedef GLboolean (*OSMesaMakeCurrent)(OSMesaContext, void*, GLenum, GLsizei, GLsizei);

class context :
	public k3d::gl::context
{
public:
	context(OSMesaContext Context, OSMesaMakeCurrent MakeCurrent, OSMesaDestroyContext DestroyContext, k3d::gl::api& API, const k3d::uint_t Width, const k3d::uint_t Height) :
		osmesa_context(Context),
		osmesa_make_current(MakeCurrent),
		osmesa_destroy_context(DestroyContext),
		api(API),
		width(Width),
		height(Height),
		buffer(Width * Height * 4, 0)
	{
	}

	~context()
	{
		osmesa_destroy_context(osmesa_context);
	}

	void make_current()
	{
		osmesa_make_current(osmesa_context, &buffer[0], GL_UNSIGNED_BYTE, width, height);
	}

	const k3d::gl::api& draw()
	{
		return api;
	}

	OSMesaContext osmesa_context;
	OSMesaMakeCurrent osmesa_make_current;
	OSMesaDestroyContext osmesa_destroy_context;
	const k3d::gl::api& api;
	const k3d::uint_t width;
	const k3d::uint_t height;
	std::vector<GLubyte> buffer;
};

class context_factory :
	public k3d::gl::icontext_factory
{
public:
	context_factory()
	{
	}

	k3d::gl::context* create(const k3d::uint_t Width, const k3d::uint_t Height)
	{
		try
		{
			void* const module = dlopen("libOSMesa.so", RTLD_LAZY | RTLD_LOCAL);
			if(!module)
				throw std::runtime_error(dlerror());
		
			OSMesaCreateContext osmesa_create_context = OSMesaCreateContext(dlsym(module, "OSMesaCreateContext"));
			if(!osmesa_create_context)
				throw std::runtime_error("Missing OSMesaCreateContext function.");

			OSMesaMakeCurrent osmesa_make_current = OSMesaMakeCurrent(dlsym(module, "OSMesaMakeCurrent"));
			if(!osmesa_make_current)
				throw std::runtime_error("Missing OSMesaMakeCurrent function.");

			OSMesaDestroyContext osmesa_destroy_context = OSMesaDestroyContext(dlsym(module, "OSMesaDestroyContext"));
			if(!osmesa_destroy_context)
				throw std::runtime_error("Missing OSMesaDestroyContext function.");

			const OSMesaContext osmesa_context = osmesa_create_context(OSMESA_RGBA, NULL);
			if(!osmesa_context)
				throw std::runtime_error("Error creating OSMesa context.");

			api.glClearColor = (void(*)(GLclampf, GLclampf, GLclampf, GLclampf))dlsym(module, "glClearColor");
			if(!api.glClearColor)
				throw std::runtime_error("Missing glClearColor function.");

			api.glClear = (void(*)(GLbitfield))dlsym(module, "glClear");
			if(!api.glClear)
				throw std::runtime_error("Missing glClear function.");

			context* const result = new context(osmesa_context, osmesa_make_current, osmesa_destroy_context, api, Width, Height);

k3d::log() << debug;
std::copy(result->buffer.begin(), result->buffer.end(), std::ostream_iterator<int>(k3d::log(), " "));
k3d::log() << std::endl;

			result->make_current();
			result->draw().glClearColor(1.0, 0.5, 0.25, 0.125);
			result->draw().glClear(GL_COLOR_BUFFER_BIT);

k3d::log() << debug;
std::copy(result->buffer.begin(), result->buffer.end(), std::ostream_iterator<int>(k3d::log(), " "));
k3d::log() << std::endl;

			return result;
		}
		catch(std::exception& e)
		{
			k3d::log() << error << e.what() << std::endl;
			return 0;
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<context_factory,
			k3d::interface_list<k3d::gl::icontext_factory> > factory(
				k3d::uuid(0xaf052636, 0xf74eeed2, 0xa1ed9cb4, 0x9f88cc5c),
				"OSMesaContextFactory",
				_("Creates OSMesa graphics contexts."),
				"OpenGL",
				k3d::iplugin_factory::EXPERIMENTAL
				);

		return factory;
	}

private:
	k3d::gl::api api;
};

} // namespace osmesa

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::osmesa::context_factory::get_factory());
K3D_MODULE_END

