#ifndef K3DSDK_NGUI_BITMAP_PREVIEW_H
#define K3DSDK_NGUI_BITMAP_PREVIEW_H

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
		\brief Declares the bitmap_preview control, which provides a standard MVC UI for bitmap data that supports tutorials, undo/redo, etc.
		\author Tim Shead (tshead@k-3d.com)
*/

#include "ui_component.h"

#include <k3dsdk/bitmap.h>

#include <gtkmm/buttonbox.h>

namespace Gtk { class Image; }
namespace k3d { class ihint; }
namespace k3d { class iproperty; }

namespace k3d
{

namespace ngui
{

namespace bitmap_preview
{

/////////////////////////////////////////////////////////////////////////////
// idata_proxy

/// Abstract interface for an object that proxies a data source for a bitmap_preview control (i.e. the "model" in model-view-controller)
class idata_proxy
{
public:
	virtual ~idata_proxy() {}

	/// Called to return the underlying data value
	virtual k3d::bitmap* value() = 0;
	/// Signal emitted if the underlying data changes
	typedef sigc::signal<void, k3d::ihint*> changed_signal_t;
	/// Signal emitted if the underlying data changes
	virtual changed_signal_t& changed_signal() = 0;

protected:
	idata_proxy() {}
	idata_proxy(const idata_proxy& RHS) {}
	idata_proxy& operator=(const idata_proxy& RHS) { return *this; }
};

/////////////////////////////////////////////////////////////////////////////
// control

/// Provides a UI for previewing bitmap images (i.e. the view and the controller from model-view-controller)
class control :
	public Gtk::HButtonBox,
	public ui_component
{
	typedef Gtk::HButtonBox base;

public:
	control(k3d::icommand_node& Parent, const std::string& Name, std::auto_ptr<idata_proxy> Data);

private:
	/// Called whenever the underlying data changes
	void data_changed(k3d::ihint*);

	boost::gil::rgb8_image_t m_image_buffer;
	boost::gil::rgb8_image_t m_alpha_buffer;

	/// Displays the image
	Gtk::Image* const m_image;
	/// Displays the alpha channel
	Gtk::Image* const m_alpha;
	/// Storeas a reference to the underlying data object
	std::auto_ptr<idata_proxy> m_data;
};

/// Provides an implementation of bitmap_preview::idata_proxy that supports any data source that supports the value(), set_value(), and changed_signal() concepts
template<typename data_t>
class data_proxy :
	public idata_proxy
{
public:
	data_proxy(data_t& Data) :
		m_data(Data)
	{
	}

	k3d::bitmap* value()
	{
		return m_data.value();
	}

	changed_signal_t& changed_signal()
	{
		return m_data.changed_signal();
	}

private:
	data_proxy(const data_proxy& RHS);
	data_proxy& operator=(const data_proxy& RHS);

	data_t& m_data;
};

/// Convenience factory function for creating bitmap_preview::idata_proxy objects
template<typename data_t>
std::auto_ptr<idata_proxy> proxy(data_t& Data)
{
	return std::auto_ptr<idata_proxy>(new data_proxy<data_t>(Data));
}

/// Specialization of proxy() for use with properties
std::auto_ptr<idata_proxy> proxy(k3d::iproperty& Data);

} // namespace bitmap_preview

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_BITMAP_PREVIEW_H


