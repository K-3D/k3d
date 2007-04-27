#ifndef NGUI_RENDER_H
#define NGUI_RENDER_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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

namespace k3d { class icamera; }
namespace k3d { class ipreview_render_engine; }
namespace k3d { class istill_render_engine; }
namespace k3d { class ianimation_render_engine; }
namespace k3d { class icamera_preview_render_engine; }
namespace k3d { class icamera_still_render_engine; }
namespace k3d { class icamera_animation_render_engine; }
namespace k3d { namespace gl { class irender_engine; } }

namespace libk3dngui
{

class document_state;

/// Returns a default camera, or 0
k3d::icamera* default_camera(document_state& DocumentState);
/// Returns a default preview render engine, or 0
k3d::ipreview_render_engine* default_preview_render_engine(document_state& DocumentState);
/// Returns a default still render engine, or 0
k3d::istill_render_engine* default_still_render_engine(document_state& DocumentState);
/// Returns a default animation render engine, or 0
k3d::ianimation_render_engine* default_animation_render_engine(document_state& DocumentState);
/// Returns a default preview render engine, or 0
k3d::icamera_preview_render_engine* default_camera_preview_render_engine(document_state& DocumentState);
/// Returns a default still render engine, or 0
k3d::icamera_still_render_engine* default_camera_still_render_engine(document_state& DocumentState);
/// Returns a default animation render engine, or 0
k3d::icamera_animation_render_engine* default_camera_animation_render_engine(document_state& DocumentState);
/// Returns a default OpenGL render engine, or 0
k3d::gl::irender_engine* default_gl_render_engine(document_state& DocumentState);

/// Prompts the user to choose a camera, returns 0 on cancellation
k3d::icamera* pick_camera(document_state& DocumentState, const k3d::icamera* CurrentCamera = 0);
/// Prompts the user to choose a preview render engine, returns 0 on cancellation
k3d::ipreview_render_engine* pick_preview_render_engine(document_state& DocumentState);
/// Prompts the user to choose a still render engine, returns 0 on cancellation
k3d::istill_render_engine* pick_still_render_engine(document_state& DocumentState);
/// Prompts the user to choose an animation render engine, returns 0 on cancellation
k3d::ianimation_render_engine* pick_animation_render_engine(document_state& DocumentState);
/// Prompts the user to choose a preview render engine, returns 0 on cancellation
k3d::icamera_preview_render_engine* pick_camera_preview_render_engine(document_state& DocumentState);
/// Prompts the user to choose a still render engine, returns 0 on cancellation
k3d::icamera_still_render_engine* pick_camera_still_render_engine(document_state& DocumentState);
/// Prompts the user to choose an animation render engine, returns 0 on cancellation
k3d::icamera_animation_render_engine* pick_camera_animation_render_engine(document_state& DocumentState);
/// Prompts the user to choose an OpenGL render engine, returns 0 on cancellation
k3d::gl::irender_engine* pick_gl_render_engine(document_state& DocumentState);

void test_render_engine(k3d::iunknown& Engine);

void render_preview(k3d::ipreview_render_engine& Engine);
void render_frame(k3d::istill_render_engine& Engine);
void render_animation(document_state& DocumentState, k3d::ianimation_render_engine& Engine);
void render_camera_preview(k3d::icamera& Camera, k3d::icamera_preview_render_engine& Engine);
void render_camera_frame(k3d::icamera& Camera, k3d::icamera_still_render_engine& Engine);
void render_camera_animation(document_state& DocumentState, k3d::icamera& Camera, k3d::icamera_animation_render_engine& Engine);

} // namespace libk3dngui

#endif // !NGUI_RENDER_H

