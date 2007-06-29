// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
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
		\author Tim Shead <tshead@k-3d.com>
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/fstream.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/ipreview_render_engine.h>
#include <k3dsdk/irender_farm.h>
#include <k3dsdk/irender_frame.h>
#include <k3dsdk/irender_job.h>
#include <k3dsdk/istill_render_engine.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/render_farm.h>

#include <iomanip>
#include <iterator>

namespace libk3dgraphviz
{

/////////////////////////////////////////////////////////////////////////////
// render_engine

class render_engine :
	public k3d::persistent<k3d::node>,
	public k3d::ipreview_render_engine,
	public k3d::istill_render_engine
{
	typedef k3d::persistent<k3d::node> base;

public:
	render_engine(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_render_engine(init_owner(*this) + init_name("render_engine") + init_label(_("Render engine")) + init_description(_("Render engine name")) + init_value(std::string("dot")) + init_values(render_engine_values()))
	{
	}

	bool render_preview()
	{
		// Start a new render job ...
		k3d::irender_job& job = k3d::render_farm().create_job("k3d-preview");

		// Add a single render frame to the job ...
		k3d::irender_frame& frame = job.create_frame("frame");

		// Create an output image path ...
		const k3d::filesystem::path outputimagepath = frame.add_output_file("world.ps");
		return_val_if_fail(!outputimagepath.empty(), false);

		// View the output image when it's done ...
		frame.add_view_operation(outputimagepath);

		// Render it ...
		return_val_if_fail(render(frame, outputimagepath), false);

		// Start the job running ...
		k3d::render_farm().start_job(job);

		return true;
	}

	bool render_frame(const k3d::filesystem::path& OutputImage, const bool ViewImage)
	{
		// Sanity checks ...
		return_val_if_fail(!OutputImage.empty(), false);

		// Start a new render job ...
		k3d::irender_job& job = k3d::render_farm().create_job("k3d-render-frame");

		// Add a single render frame to the job ...
		k3d::irender_frame& frame = job.create_frame("frame");

		// Create an output image path ...
		const k3d::filesystem::path outputimagepath = frame.add_output_file("world.ps");
		return_val_if_fail(!outputimagepath.empty(), false);

		// Copy the output image to its requested destination ...
		frame.add_copy_operation(outputimagepath, OutputImage);

		// View the output image when it's done ...
		if(ViewImage)
			frame.add_view_operation(OutputImage);

		// Render it ...
		return_val_if_fail(render(frame, outputimagepath), false);

		// Start the job running ...
		k3d::render_farm().start_job(job);

		return true;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<render_engine,
			k3d::interface_list<k3d::istill_render_engine,
			k3d::interface_list<k3d::ipreview_render_engine> > > factory(
				k3d::uuid(0xbe72cb50, 0x011f41d8, 0x90449ae0, 0x4c24ace5),
				"GraphVizEngine",
				_("GraphViz Render Engine"),
				"RenderEngines",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	template<typename T>
	static unsigned long to_integer(const T RHS)
	{
		// If this fails, it means that a pointer can't fit in a long int on your platform - you need to adjust the return type of this function
		BOOST_STATIC_ASSERT(sizeof(T) <= sizeof(unsigned long));

		return reinterpret_cast<unsigned long>(RHS);
	}

	static const std::string escaped_string(const std::string& Source)
	{
		std::string result(Source);

		for(std::string::size_type i = result.find('\"'); i != std::string::npos; i = result.find('\"', i+2))
			result.replace(i, 1, "\\\"");

		return result;
	}

	bool render(k3d::irender_frame& Frame, const k3d::filesystem::path& OutputImagePath)
	{
		// Sanity checks ...
		return_val_if_fail(!OutputImagePath.empty(), false);

		// Start our GraphViz DOT file ...
		const k3d::filesystem::path filepath = Frame.add_input_file("world.dot");
		return_val_if_fail(!filepath.empty(), false);

		// Open the DOT file stream ...
		k3d::filesystem::ofstream stream(filepath);
		return_val_if_fail(stream.good(), false);

		// Setup the frame for GraphViz rendering ...
		Frame.add_render_operation("graphviz", m_render_engine.value(), filepath, false);

		stream << "digraph \"" << boost::any_cast<k3d::ustring>(document().title().property_value()).raw() << "\"\n";
		stream << "{\n\n";

		stream << "rankdir=TD\n\n";

		stream << "node [shape=box,style=filled,width=0,height=0]\n\n";

		// Make a mapping of properties-to-objects as we go ...
		std::map<k3d::iproperty*, k3d::inode*> object_map;

		// Create nodes for every document object ...
		const k3d::nodes_t objects = document().nodes().collection();
		for(k3d::nodes_t::const_iterator object = objects.begin(); object != objects.end(); ++object)
		{
			stream << to_integer(*object) << " [label=\"" << escaped_string((*object)->name()) << "\"]\n";

			k3d::iproperty_collection* const property_collection = dynamic_cast<k3d::iproperty_collection*>(*object);
			if(property_collection)
			{
				const k3d::iproperty_collection::properties_t properties = property_collection->properties();
				for(k3d::iproperty_collection::properties_t::const_iterator property = properties.begin(); property != properties.end(); ++property)
				{
					object_map.insert(std::make_pair(*property, *object));

					// Show references between objects ...
					if(typeid(k3d::inode*) == (*property)->property_type())
					{
						k3d::inode* const referenced_object = boost::any_cast<k3d::inode*>((*property)->property_value());
						if(referenced_object)
						{
							stream << to_integer(referenced_object) << " -> " << to_integer(*object);
							stream << " [style=dotted,label=\"" << escaped_string((*property)->property_name()) << "\"]\n";
						}
					}
				}
			}
		}

		// Show property dependencies ...
		stream << "\n";
		const k3d::idag::dependencies_t dependencies = document().dag().dependencies();
		for(k3d::idag::dependencies_t::const_iterator dependency = dependencies.begin(); dependency != dependencies.end(); ++dependency)
		{
			if(dependency->first && dependency->second)
			{
				stream << to_integer(object_map[dependency->second]) << " -> " << to_integer(object_map[dependency->first]);
				stream << " [headlabel=\"" << escaped_string(dependency->first->property_name()) << "\" taillabel=\"" << escaped_string(dependency->second->property_name()) << "\"]\n";
			}
		}

		stream << "\n}\n";

		return true;
	}

	k3d_data(std::string, immutable_name, change_signal, with_undo, local_storage, no_constraint, list_property, with_serialization) m_render_engine;

	const k3d::ilist_property<std::string>::values_t& render_engine_values()
	{
		static k3d::ilist_property<std::string>::values_t values;
		if(values.empty())
		{
			const k3d::options::render_engines_t engines = k3d::options::render_engines();
			for(k3d::options::render_engines_t::const_iterator engine = engines.begin(); engine != engines.end(); ++engine)
			{
				if(engine->type == "graphviz")
					values.push_back(engine->name);
			}
		}
		return values;
	}
};

} // namespace libk3dgraphviz

K3D_MODULE_START(Registry)
	Registry.register_factory(libk3dgraphviz::render_engine::get_factory());
K3D_MODULE_END

