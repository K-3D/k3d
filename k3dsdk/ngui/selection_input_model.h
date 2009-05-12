#ifndef K3DSDK_NGUI_SELECTION_INPUT_MODEL_H
#define K3DSDK_NGUI_SELECTION_INPUT_MODEL_H

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
		\author Tim Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/signal_system.h>

namespace k3d
{

namespace ngui
{

/////////////////////////////////////////////////////////////////////////////
// selection_input_model

/// Input model implementation that handles interactive selection
class selection_input_model
{
public:
	selection_input_model(document_state& DocumentState);
	~selection_input_model();

	void set_extended_mode(const bool Enabled);
	void set_extended_component_mode(const bool Enabled);
	void set_paint_mode(const bool Enabled);
	void set_double_click_mode(const bool Enabled);
	void set_pick_backfacing(const bool Enabled);
	void set_paint_backfacing(const bool Enabled);
	void set_rubber_band_backfacing(const bool Enabled);

	void on_button_down(viewport::control& Viewport, const GdkEventButton& Event);
	void on_button_click(viewport::control& Viewport, const GdkEventButton& Event);
	void on_button_double_click(viewport::control& Viewport, const GdkEventButton& Event);
	void on_button_start_drag(viewport::control& Viewport, const GdkEventMotion& Event);
	void on_button_drag(viewport::control& Viewport, const GdkEventMotion& Event);
	void on_button_end_drag(viewport::control& Viewport, const GdkEventButton& Event);

private:
	struct implementation;
	implementation* const m_implementation;
};

} // namespace ngui

} // namespace k3d

#endif // !K3DSDK_NGUI_SELECTION_INPUT_MODEL_H

