#ifndef K3DSDK_RENDERABLE_RI_H
#define K3DSDK_RENDERABLE_RI_H

// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
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

#include "data.h"
#include "k3d-i18n-config.h"
#include "istream_ri.h"
#include "irenderable_ri.h"
#include "render_state_ri.h"

namespace k3d
{

class idocument;
class iproperty_collection;
	
namespace ri
{

/////////////////////////////////////////////////////////////////////////////
// motion_begin

/// Convenience function that opens a RIB motion block iff motion-blur is enabled
void motion_begin(const render_state& State);

/////////////////////////////////////////////////////////////////////////////
// motion_end

/// Convenience function that closes a RIB motion block iff motion-blur is enabled
void motion_end(const render_state& State);

/////////////////////////////////////////////////////////////////////////////
// motion_blur

/// Convenience function that returns true iff motion-blurred rendering is enabled
bool motion_blur(const render_state& State);

/////////////////////////////////////////////////////////////////////////////
// first_sample

/// Convenience function that returns true iff rendering the first sample in an image
bool first_sample(const render_state& State);

/////////////////////////////////////////////////////////////////////////////
// last_sample

/// Convenience function that returns true iff rendering the last sample in an image
bool last_sample(const render_state& State);

/////////////////////////////////////////////////////////////////////////////
// convert

/// Converts a k3d::matrix4 into a form usable with RenderMan
const matrix convert(const k3d::matrix4& Matrix);

/////////////////////////////////////////////////////////////////////////////
// setup_material

void setup_material(iunknown* const Material, const render_state& State);

/////////////////////////////////////////////////////////////////////////////
// set_attributes

/// Extracts RenderMan data from a property collection, generating corresponding calls to RiAttribute()
void set_attributes(iproperty_collection& Properties, istream& Engine);

/////////////////////////////////////////////////////////////////////////////
// set_options

/// Extracts RenderMan data from a property collection, generating corresponding calls to RiOption()
void set_options(iproperty_collection& Properties, istream& Engine);

/////////////////////////////////////////////////////////////////////////////
// renderable

/// Adds a boilerplate implementation of k3d::ri::irender to a transformable base class, using the parameterized inheritance idiom
template<typename base_t>
class renderable :
	public base_t,
	public irenderable
{
public:
	renderable(iplugin_factory& Factory, idocument& Document) :
		base_t(Factory, Document),
		m_render_final(init_owner(*this) + init_name("render_final") + init_label(_("Render")) + init_description(_("Controls whether this node will be visible in the final rendered image.")) + init_value(true)),
		m_render_shadows(init_owner(*this) + init_name("render_shadows") + init_label(_("Render Shadows")) + init_description(_("Controls whether this node will be visible during shadow-map render passes.")) + init_value(true)),
		m_motion_blur(init_owner(*this) + init_name("motion_blur") + init_label(_("Motion Blur")) + init_description("If enabled, this node will be rendered with motion-blur.") + init_value(false))
	{
	}

	void renderman_render(const render_state& State)
	{
		// If this is a normal pass and we're hidden, we're done ...
		if(State.render_context == FINAL_FRAME && !m_render_final.pipeline_value())
			return;
		// If this is a shadow pass and we don't cast shadows, we're done ...
		if(State.render_context == SHADOW_MAP && !m_render_shadows.pipeline_value())
			return;

		// If this is the first sample in the frame, reset the sample list ...
		if(first_sample(State))
			m_motion_blur_samples.clear();

		// Push our current state onto the sample list ...
		m_motion_blur_samples.push_back(base_t::matrix());

		// Only generate RIB on the last sample ...
		if(last_sample(State))
		{
			State.stream.RiAttributeBegin();

			if(motion_blur(State) && m_motion_blur.pipeline_value())
			{
				State.stream.RiMotionBeginV(State.sample_times);

				for(unsigned int i = 0; i < m_motion_blur_samples.size(); ++i)
					State.stream.RiConcatTransform(convert(m_motion_blur_samples[i]));

				State.stream.RiMotionEnd();
			}
			else
			{
				State.stream.RiConcatTransform(convert(m_motion_blur_samples.front()));
			}

			set_attributes(*this, State.stream);
			on_renderman_render(State);

			State.stream.RiAttributeEnd();
		}
	}

	void renderman_render_complete(const render_state& State)
	{
		on_renderman_render_complete(State);
	}

private:
	virtual void on_renderman_render(const render_state& State) = 0;

	virtual void on_renderman_render_complete(const render_state& State)
	{
	}

	k3d_data(bool, data::immutable_name, data::change_signal, data::with_undo, data::local_storage, data::no_constraint, data::writable_property, data::with_serialization) m_render_final;
	k3d_data(bool, data::immutable_name, data::change_signal, data::with_undo, data::local_storage, data::no_constraint, data::writable_property, data::with_serialization) m_render_shadows;
	k3d_data(bool, data::immutable_name, data::change_signal, data::with_undo, data::local_storage, data::no_constraint, data::writable_property, data::with_serialization) m_motion_blur;
	std::vector<k3d::matrix4> m_motion_blur_samples;
};

} // namespace ri

} // namespace k3d

#endif // !K3DSDK_RENDERABLE_RI_H

