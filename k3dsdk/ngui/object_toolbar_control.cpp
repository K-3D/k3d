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
	\author Tim Shead (tshead@k-3d.com)
*/

#include "button.h"
#include "document_state.h"
#include "icons.h"
#include "object_toolbar_control.h"
#include "properties.h"
#include "render.h"
#include "toggle_button.h"
#include "toolbar.h"
#include "uri.h"
#include "widget_manip.h"

#include <k3d-i18n-config.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/imesh_sink.h>
#include <k3dsdk/imesh_source.h>
#include <k3dsdk/imesh_storage.h>
#include <k3dsdk/ipipeline.h>
#include <k3dsdk/irender_animation.h>
#include <k3dsdk/irender_camera_animation.h>
#include <k3dsdk/irender_camera_frame.h>
#include <k3dsdk/irender_camera_preview.h>
#include <k3dsdk/irender_frame.h>
#include <k3dsdk/irender_preview.h>
#include <k3dsdk/utility.h>

#include <gtkmm/image.h>
#include <gtkmm/stock.h>

using namespace libk3dngui;

namespace k3d
{

namespace ngui
{

namespace object_toolbar
{

/// toggle_button model that turns a property-bypass into a togglable action
class bypass_property_proxy :
	public toggle_button::imodel
{
public:
	bypass_property_proxy(document_state& DocumentState, iproperty& InputProperty, iproperty& OutputProperty) :
		m_document_state(DocumentState),
		m_input_property(InputProperty),
		m_output_property(OutputProperty)
	{
	}

	const Glib::ustring label()
	{
		return _("Bypass modifier");
	}

	const bool_t value()
	{
		// true == bypassed, false == normal
		return m_document_state.document().pipeline().dependency(m_output_property) == &m_input_property;
	}

	void set_value(const bool_t Value)
	{
		// true == bypassed, false == normal
		ipipeline::dependencies_t dependencies;
		dependencies.insert(std::make_pair(&m_output_property, Value ? &m_input_property : static_cast<iproperty*>(0)));
		m_document_state.document().pipeline().set_dependencies(dependencies);

		m_changed_signal.emit();
	}

	sigc::connection connect_changed_signal(const sigc::slot<void>& Slot)
	{
		return m_changed_signal.connect(Slot);
	}

private:
	bypass_property_proxy(const bypass_property_proxy&);
	bypass_property_proxy& operator=(const bypass_property_proxy&);

	document_state& m_document_state;
	iproperty& m_input_property;
	iproperty& m_output_property;

	sigc::signal<void> m_changed_signal;
};

///////////////////////////////////////////////////////////////////////////
// control::implementation

class control::implementation
{
public:
	implementation(document_state& DocumentState, icommand_node& Parent, const string_t& Name) :
		m_document_state(DocumentState),
		m_toolbar(Parent, Name)
	{
	}

	void set_object(iunknown* Object)
	{
		// Delete existing icons ...
		Glib::ListHandle<Gtk::Widget*> children = m_toolbar.get_children();
		std::for_each(children.begin(), children.end(), delete_object());

		// Add a help button ...
		if(inode* const node = dynamic_cast<inode*>(Object))
		{
			const string_t uri = "http://www.k-3d.org/wiki/" + node->factory().name();

			button::control* const control =
				new button::control(m_toolbar, "online_help", Gtk::Stock::HELP)
					<< connect_button(sigc::bind(sigc::ptr_fun(&k3d::ngui::uri::open), uri))
					<< set_tooltip(_("Display online help."));

			m_toolbar.row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		// Add controls for managing user properties ...
		if(inode* const node = dynamic_cast<inode*>(Object))
		{
			button::control* const control =
				new button::control(m_toolbar, "add_user_property", *Gtk::manage(new Gtk::Image(Gtk::Stock::ADD, Gtk::ICON_SIZE_BUTTON)))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_add_user_property), node))
					<< set_tooltip(_("Add a user property."));

			m_toolbar.row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		// Add controls for cameras
		if(icamera* const camera = dynamic_cast<icamera*>(Object))
		{
			button::control* const camera_preview =
				new button::control(m_toolbar, "render_camera_preview", *Gtk::manage(new Gtk::Image(load_icon("render_preview", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_render_camera_camera_preview), camera))
					<< set_tooltip(_("Render Preview"));

			button::control* const camera_frame =
				new button::control(m_toolbar, "render_camera_frame", *Gtk::manage(new Gtk::Image(load_icon("render_frame", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_render_camera_camera_frame), camera))
					<< set_tooltip(_("Render Frame"));

			button::control* const camera_animation =
				new button::control(m_toolbar, "render_camera_animation", *Gtk::manage(new Gtk::Image(load_icon("render_animation", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_render_camera_camera_animation), camera))
					<< set_tooltip(_("Render Animation"));

			m_toolbar.row(0).pack_start(*Gtk::manage(camera_preview), Gtk::PACK_SHRINK);
			m_toolbar.row(0).pack_start(*Gtk::manage(camera_frame), Gtk::PACK_SHRINK);
			m_toolbar.row(0).pack_start(*Gtk::manage(camera_animation), Gtk::PACK_SHRINK);
		}

		// Add controls for camera render engines ...
		if(irender_camera_preview* const render_camera_preview = dynamic_cast<irender_camera_preview*>(Object))
		{
			button::control* const control =
				new button::control(m_toolbar, "render_camera_preview", *Gtk::manage(new Gtk::Image(load_icon("render_preview", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_render_camera_preview), render_camera_preview))
					<< set_tooltip(_("Render Preview"));

			m_toolbar.row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		if(irender_camera_frame* const render_camera_frame = dynamic_cast<irender_camera_frame*>(Object))
		{
			button::control* const control =
				new button::control(m_toolbar, "render_camera_frame", *Gtk::manage(new Gtk::Image(load_icon("render_frame", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_render_camera_frame), render_camera_frame))
					<< set_tooltip(_("Render Frame"));

			m_toolbar.row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		if(irender_camera_animation* const render_camera_animation = dynamic_cast<irender_camera_animation*>(Object))
		{
			button::control* const control =
				new button::control(m_toolbar, "render_camera_animation", *Gtk::manage(new Gtk::Image(load_icon("render_animation", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_render_camera_animation), render_camera_animation))
					<< set_tooltip(_("Render Animation"));

			m_toolbar.row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		// Add controls for render engines
		if(irender_preview* const render_preview = dynamic_cast<irender_preview*>(Object))
		{
			button::control* const control =
				new button::control(m_toolbar, "render_preview", *Gtk::manage(new Gtk::Image(load_icon("render_preview", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_render_preview), render_preview))
					<< set_tooltip(_("Render Preview"));

			m_toolbar.row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		if(irender_frame* const render_frame = dynamic_cast<irender_frame*>(Object))
		{
			button::control* const control =
				new button::control(m_toolbar, "render_frame", *Gtk::manage(new Gtk::Image(load_icon("render_frame", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_render_frame), render_frame))
					<< set_tooltip(_("Render Frame"));

			m_toolbar.row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		if(irender_animation* const render_animation = dynamic_cast<irender_animation*>(Object))
		{
			button::control* const control =
				new button::control(m_toolbar, "render_animation", *Gtk::manage(new Gtk::Image(load_icon("render_animation", Gtk::ICON_SIZE_BUTTON))))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_render_animation), render_animation))
					<< set_tooltip(_("Render Animation"));

			m_toolbar.row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		// Add a "reset" button for nodes that implement imesh_storage (FrozenMesh, external file readers, etc) ...
		if(imesh_storage* const mesh_storage = dynamic_cast<imesh_storage*>(Object))
		{
			button::control* const control =
				new button::control(m_toolbar, "reset_mesh", _("Reset Mesh"))
					<< connect_button(sigc::bind(sigc::mem_fun(*this, &implementation::on_reset_mesh), mesh_storage))
					<< set_tooltip(_("Reset / Reload Mesh"));

			m_toolbar.row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
		}

		// Add a "disable" button for mesh modifiers ...
		if(imesh_source* const mesh_source = dynamic_cast<imesh_source*>(Object))
		{
			if(imesh_sink* const mesh_sink = dynamic_cast<imesh_sink*>(Object))
			{
				toggle_button::control* const control =
					new toggle_button::control(
						m_toolbar,
						"disable_mesh_modifier",
						new bypass_property_proxy(m_document_state, mesh_sink->mesh_sink_input(), mesh_source->mesh_source_output()),
						&m_document_state.document().state_recorder(),
						_("Disable"))
						<< set_tooltip(_("Disable / bypass mesh modifier"));

				m_toolbar.row(0).pack_start(*Gtk::manage(control), Gtk::PACK_SHRINK);
			}
		}

		m_toolbar.show_all();
	}

	void on_add_user_property(inode* Node)
	{
		k3d::ngui::property::create(*Node, m_toolbar);
	}
	
	void on_render_camera_camera_preview(icamera* Camera)
	{
	       irender_camera_preview* const render_camera_preview = pick_camera_preview_render_engine(m_document_state);
	       if(!render_camera_preview)
		       return;

	       render(*Camera, *render_camera_preview);
	}

	void on_render_camera_camera_frame(icamera* Camera)
	{
	       irender_camera_frame* const render_camera_frame = pick_camera_still_render_engine(m_document_state);
	       if(!render_camera_frame)
		       return;

	       render(*Camera, *render_camera_frame);
	}

	void on_render_camera_camera_animation(icamera* Camera)
	{
	       irender_camera_animation* const render_camera_animation = pick_camera_animation_render_engine(m_document_state);
	       if(!render_camera_animation)
		       return;

	       render(m_document_state, *Camera, *render_camera_animation);
	}

	void on_render_camera_preview(irender_camera_preview* RenderCameraPreview)
	{
	       icamera* const camera = pick_camera(m_document_state);
	       if(!camera)
		       return;

	       render(*camera, *RenderCameraPreview);
	}

	void on_render_camera_frame(irender_camera_frame* RenderCameraFrame)
	{
	       icamera* const camera = pick_camera(m_document_state);
	       if(!camera)
		       return;

	       render(*camera, *RenderCameraFrame);
	}

	void on_render_camera_animation(irender_camera_animation* RenderCameraAnimation)
	{
	       icamera* const camera = pick_camera(m_document_state);
	       if(!camera)
		       return;

	       render(m_document_state, *camera, *RenderCameraAnimation);
	}

	void on_render_preview(irender_preview* RenderPreview)
	{
		render(*RenderPreview);
	}

	void on_render_frame(irender_frame* RenderFrame)
	{
		render(*RenderFrame);
	}

	void on_render_animation(irender_animation* RenderAnimation)
	{
		render(m_document_state, *RenderAnimation);
	}

	void on_reset_mesh(imesh_storage* MeshStorage)
	{
		MeshStorage->reset_mesh(0);
	}

	document_state& m_document_state;
	toolbar::control m_toolbar;
};

///////////////////////////////////////////////////////////////////////////
// control

control::control(document_state& DocumentState, icommand_node& Parent, const string_t& Name) :
	m_implementation(new implementation(DocumentState, Parent, Name))
{
}

control::~control()
{
	delete m_implementation;
}

void control::set_object(iunknown* Object)
{
	m_implementation->set_object(Object);
}

Gtk::Widget& control::get_widget()
{
	return m_implementation->m_toolbar;
}

} // namespace object_toolbar

} // namespace ngui

} // namespace k3d

