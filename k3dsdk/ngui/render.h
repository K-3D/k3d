#ifndef NGUI_RENDER_H
#define NGUI_RENDER_H

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

namespace k3d { class icamera; }
namespace k3d { class irender_preview; }
namespace k3d { class irender_frame; }
namespace k3d { class irender_animation; }
namespace k3d { class irender_camera_preview; }
namespace k3d { class irender_camera_frame; }
namespace k3d { class irender_camera_animation; }
namespace k3d { namespace gl { class irender_viewport; } }

namespace libk3dngui
{

class document_state;

/// Returns a default camera, or 0
k3d::icamera* default_camera(document_state& DocumentState);
/// Returns a default preview render engine, or 0
k3d::irender_preview* default_preview_render_engine(document_state& DocumentState);
/// Returns a default still render engine, or 0
k3d::irender_frame* default_still_render_engine(document_state& DocumentState);
/// Returns a default animation render engine, or 0
k3d::irender_animation* default_animation_render_engine(document_state& DocumentState);
/// Returns a default preview render engine, or 0
k3d::irender_camera_preview* default_camera_preview_render_engine(document_state& DocumentState);
/// Returns a default still render engine, or 0
k3d::irender_camera_frame* default_camera_still_render_engine(document_state& DocumentState);
/// Returns a default animation render engine, or 0
k3d::irender_camera_animation* default_camera_animation_render_engine(document_state& DocumentState);
/// Returns a default OpenGL render engine, or 0
k3d::gl::irender_viewport* default_gl_render_engine(document_state& DocumentState);

/// Prompts the user to choose a camera, returns 0 on cancellation
k3d::icamera* pick_camera(document_state& DocumentState, const k3d::icamera* CurrentCamera = 0);
/// Prompts the user to choose a preview render engine, returns 0 on cancellation
k3d::irender_preview* pick_preview_render_engine(document_state& DocumentState);
/// Prompts the user to choose a still render engine, returns 0 on cancellation
k3d::irender_frame* pick_still_render_engine(document_state& DocumentState);
/// Prompts the user to choose an animation render engine, returns 0 on cancellation
k3d::irender_animation* pick_animation_render_engine(document_state& DocumentState);
/// Prompts the user to choose a preview render engine, returns 0 on cancellation
k3d::irender_camera_preview* pick_camera_preview_render_engine(document_state& DocumentState);
/// Prompts the user to choose a still render engine, returns 0 on cancellation
k3d::irender_camera_frame* pick_camera_still_render_engine(document_state& DocumentState);
/// Prompts the user to choose an animation render engine, returns 0 on cancellation
k3d::irender_camera_animation* pick_camera_animation_render_engine(document_state& DocumentState);
/// Prompts the user to choose an OpenGL render engine, returns 0 on cancellation
k3d::gl::irender_viewport* pick_gl_render_engine(document_state& DocumentState);

void test_render_engine(k3d::iunknown& Engine);

void render(k3d::irender_preview& Engine);
void render(k3d::irender_frame& Engine);
void render(document_state& DocumentState, k3d::irender_animation& Engine);
void render(k3d::icamera& Camera, k3d::irender_camera_preview& Engine);
void render(k3d::icamera& Camera, k3d::irender_camera_frame& Engine);
void render(document_state& DocumentState, k3d::icamera& Camera, k3d::irender_camera_animation& Engine);

} // namespace libk3dngui

#endif // !NGUI_RENDER_H

