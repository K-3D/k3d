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
#include <k3dsdk/gl/api.h>
#include <k3dsdk/gl/offscreen_context_factory.h>
#include <k3dsdk/gl/offscreen_context.h>
#include <k3dsdk/module.h>
#include <k3dsdk/result.h>

#include <boost/optional.hpp>

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
	public k3d::gl::offscreen_context
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

	const k3d::gl::api& on_begin()
	{
		osmesa_make_current(osmesa_context, &buffer[0], GL_UNSIGNED_BYTE, width, height);
		return api;
	}

	void on_end()
	{
	}

	const k3d::uint8_t* buffer_begin()
	{
		return &buffer[0];
	}

	const k3d::uint8_t* buffer_end()
	{
		return &buffer[0] + buffer.size();
	}

	OSMesaContext osmesa_context;
	OSMesaMakeCurrent osmesa_make_current;
	OSMesaDestroyContext osmesa_destroy_context;
	const k3d::gl::api& api;
	const k3d::uint_t width;
	const k3d::uint_t height;
	std::vector<GLubyte> buffer;
};

struct api_loader
{
	api_loader(void* Module) :
		module(Module)
	{
	}

	template<typename FunctionT>
	void operator()(const char* Name, FunctionT& Function) const
	{
		if(void* function = dlsym(module, Name))
			Function = FunctionT(function);
	}

	void* const module;
};

class context_factory :
	public k3d::gl::offscreen_context_factory
{
public:
	context_factory()
	{
	}

	k3d::gl::offscreen_context* create(const k3d::uint_t Width, const k3d::uint_t Height)
	{
		try
		{
			if(!module)
			{
				module = dlopen("/opt/local/lib/libOSMesa.dylib", RTLD_LAZY | RTLD_LOCAL);
				if(!module.get())
					throw std::runtime_error(dlerror());
			}

			if(!osmesa_create_context)
				osmesa_create_context = OSMesaCreateContext(dlsym(module.get(), "OSMesaCreateContext"));
			if(!osmesa_create_context.get())
				throw std::runtime_error("Missing OSMesaCreateContext function.");

			if(!osmesa_make_current)
				osmesa_make_current = OSMesaMakeCurrent(dlsym(module.get(), "OSMesaMakeCurrent"));
			if(!osmesa_make_current.get())
				throw std::runtime_error("Missing OSMesaMakeCurrent function.");

			if(!osmesa_destroy_context)
				osmesa_destroy_context = OSMesaDestroyContext(dlsym(module.get(), "OSMesaDestroyContext"));
			if(!osmesa_destroy_context.get())
				throw std::runtime_error("Missing OSMesaDestroyContext function.");

			if(!api)
			{
				api = k3d::gl::api();
				api.get().load(api_loader(module.get()));
			}

			const OSMesaContext osmesa_context = osmesa_create_context.get()(OSMESA_RGBA, NULL);
			if(!osmesa_context)
				throw std::runtime_error("Error creating OSMesa context.");

			return new context(osmesa_context, osmesa_make_current.get(), osmesa_destroy_context.get(), api.get(), Width, Height);
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
			k3d::interface_list<k3d::gl::offscreen_context_factory> > factory(
				k3d::uuid(0xaf052636, 0xf74eeed2, 0xa1ed9cb4, 0x9f88cc5c),
				"OSMesaContextFactory",
				_("Creates OSMesa graphics contexts."),
				"OpenGL",
				k3d::iplugin_factory::EXPERIMENTAL
				);

		return factory;
	}

private:
	static boost::optional<void*> module;
	static boost::optional<OSMesaCreateContext> osmesa_create_context;
	static boost::optional<OSMesaMakeCurrent> osmesa_make_current;
	static boost::optional<OSMesaDestroyContext> osmesa_destroy_context;
	static boost::optional<k3d::gl::api> api;
};

boost::optional<void*> context_factory::module;
boost::optional<OSMesaCreateContext> context_factory::osmesa_create_context;
boost::optional<OSMesaMakeCurrent> context_factory::osmesa_make_current;
boost::optional<OSMesaDestroyContext> context_factory::osmesa_destroy_context;
boost::optional<k3d::gl::api> context_factory::api;

/////////////////////////////////////////////////////////////////////////////
// context_factory_factory

k3d::iplugin_factory& context_factory_factory()
{
	return context_factory::get_factory();
}

} // namespace osmesa

} // namespace module

