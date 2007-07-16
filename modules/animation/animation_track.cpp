// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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
		\brief A track object, containing keyframes of an animation
		\author Bart Janssens (bart.janssens@lid.kviv.be)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/icommand_node.h>
#include <k3dsdk/icommand_node_simple.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/property_group_collection.h>
#include <k3dsdk/string_cast.h>
#include <k3dsdk/tokens.h>
#include <k3dsdk/vectors.h>

#include "interpolator.h"

namespace libk3danimation
{
	
class set_keyframe : public k3d::icommand_node_simple
{
	virtual const k3d::icommand_node::result execute_command()
	{
		k3d::log() << debug << "Executing explicit keyframe" << std::endl;
		return k3d::icommand_node::RESULT_CONTINUE;
	}
};

template <typename time_t, typename value_t>
class animation_track :
	public k3d::persistent<k3d::node>,
	public k3d::property_group_collection
{
	typedef k3d::persistent<k3d::node> base;
	typedef k3d_data(time_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) time_property_t;
	typedef k3d_data(value_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) value_property_t;
	typedef std::map<time_property_t*, value_property_t*> keyframes_t;
	typedef std::map<time_t, time_property_t*> keytimes_t;
	typedef interpolator<time_t, value_t> interpolator_t;
public:
	animation_track(k3d::iplugin_factory& Factory, k3d::idocument& Document, time_t Time, value_t Value) :
		base(Factory, Document),
		m_keyframe_command(init_owner(*this) + init_name("keyframe_command") + init_label(("Set Keyframe")) + init_description(("Explicitely set a keyframe at the current time")) + init_value(new set_keyframe())),
		m_time_input(init_owner(*this) + init_name("time_input") + init_label(("Time Input")) + init_description(("Time for the animation")) + init_value(Time)),
		m_value_input(init_owner(*this) + init_name("value_input") + init_label(("Value Input")) + init_description(("Input that is keyframed when it changes")) + init_value(Value)),
		m_output_value(init_owner(*this) + init_name("output_value") + init_label(("Output Value")) + init_description(("Interpolated output value")) + init_slot(sigc::mem_fun(*this, &animation_track::on_output_request))),
		m_interpolator(init_owner(*this) + init_name("interpolator") + init_label("Interpolator") + init_description("Method used to interpolate keyframes") + init_value(static_cast<interpolator_t*>(0)))
	{
		m_time_input.changed_signal().connect(m_output_value.make_reset_slot());
		m_value_input.changed_signal().connect(sigc::mem_fun(*this, &animation_track::on_value_change));
	}
	
	value_t on_output_request()
	{
		interpolator_t* interpolator = m_interpolator.value();
		return_val_if_fail(interpolator, m_value_input.value());
		typename interpolator_t::keyframes_t keyframes;
		for (typename keyframes_t::iterator keyframe = m_keyframes.begin(); keyframe != m_keyframes.end(); ++keyframe)
			keyframes.insert(std::make_pair(keyframe->first->value(), keyframe->second->value()));
		time_t time = m_time_input.value();
		try
		{
			return interpolator->interpolate(time, keyframes);
		}
		catch (insufficient_data_exception& e)
		{
			k3d::log() << warning << name() << ": Insufficient keyframe data to calculate value at time " << time << std::endl;
			return m_value_input.value();
		}
	}
	
	void on_value_change(k3d::iunknown* Hint)
	{
		if(!document().state_recorder().current_change_set())
		{
			k3d::log() << debug << "Not recording keyframe because no undo recording active" << std::endl;
			return;
		}
		time_t time = m_time_input.value();
		typename keytimes_t::iterator time_property_it = m_keytimes.find(time);
		typename keyframes_t::iterator value_it;
		// Create new keyframe if a keyframe did not already exist for the current time
		if (time_property_it == m_keytimes.end())
		{
			std::string key_number = boost::lexical_cast<std::string>(m_keytimes.size());
			std::string time_name = "key_time_" + key_number;
			std::string time_label = "Key Time " + key_number;
			std::string value_name = "key_value_" + key_number;
			std::string value_label = "Key Value " + key_number;
			time_property_it = m_keytimes.insert(std::make_pair(time, new time_property_t(init_owner(*this) + init_name(k3d::make_token(time_name.c_str())) + init_label(k3d::make_token(time_label.c_str())) + init_description(("")) + init_value(time)))).first;
			value_it = m_keyframes.insert(std::make_pair(time_property_it->second, new value_property_t(init_owner(*this) + init_name(k3d::make_token(value_name.c_str())) + init_label(k3d::make_token(value_label.c_str())) + init_description(("")) + init_value(m_value_input.value())))).first;
			k3d::iproperty_group_collection::group key_group("Key " + key_number);
			key_group.properties.push_back(static_cast<k3d::iproperty*>(time_property_it->second));
			key_group.properties.push_back(static_cast<k3d::iproperty*>(value_it->second));
			register_property_group(key_group);
		}
		else
		{
			value_it = m_keyframes.find(time_property_it->second);
			value_it->second->set_value(m_value_input.value());
		}
		m_output_value.reset();
	}
	
	/// Make sure the keyframe structures get updated on load
	void load(k3d::xml::element& Element, const k3d::ipersistent::load_context& Context)
	{
		base::load(Element, Context);
		
		k3d::xml::element* const properties = k3d::xml::find_element(Element, "properties");
		return_if_fail(properties);
		
		// Keep track of frames by stored number
		std::map<std::string, time_property_t*> time_properties;
		
		// Fetch keyframe time properties
		for(k3d::xml::element::elements_t::iterator element = properties->children.begin(); element != properties->children.end(); ++element)
		{
			if(element->name == "property")
			{
				const std::string& property_name = k3d::xml::attribute_value<std::string>(*element, "name", "");
				if (property_name.find("key_time_") == 0)
				{
					std::string keynumber = property_name.substr(9, property_name.size() - 8);
					std::string label = "Key Time " + keynumber;
					time_t time = k3d::from_string<time_t>(element->text, time_t());
					time_property_t* time_property = new time_property_t(init_owner(*this) + init_name(k3d::make_token(property_name.c_str())) + init_label(k3d::make_token(label.c_str())) + init_description(("")) + init_value(time));
					m_keytimes.insert(std::make_pair(time, time_property));
					k3d::log() << debug << "storing key number: " << keynumber << std::endl;
					time_properties[keynumber] = time_property;
				}
			}
		}
		
		// Fetch the value properties
		for(k3d::xml::element::elements_t::iterator element = properties->children.begin(); element != properties->children.end(); ++element)
		{
			if(element->name == "property")
			{
				const std::string& property_name = k3d::xml::attribute_value<std::string>(*element, "name", "");
				if (property_name.find("key_value_") == 0)
				{
					std::string keynumber = property_name.substr(10, property_name.size() - 9);
					std::string label = "Key Value " + keynumber;
					value_t value = k3d::from_string<value_t>(element->text, value_t());
					k3d::log() << debug << "fetching key number: " << keynumber << std::endl;
					typename std::map<std::string, time_property_t*>::iterator time_property_it = time_properties.find(keynumber);
					return_if_fail(time_property_it != time_properties.end());
					value_property_t* value_property = new value_property_t(init_owner(*this) + init_name(k3d::make_token(property_name.c_str())) + init_label(k3d::make_token(label.c_str())) + init_description(("")) + init_value(value)); 
					m_keyframes.insert(std::make_pair(time_property_it->second, value_property));
					k3d::iproperty_group_collection::group key_group("Key " + keynumber);
					key_group.properties.push_back(static_cast<k3d::iproperty*>(time_property_it->second));
					key_group.properties.push_back(static_cast<k3d::iproperty*>(value_property));
					register_property_group(key_group);
				}
			}
		}
	}

private:
	k3d_data(k3d::icommand_node_simple*, immutable_name, change_signal, with_undo, local_storage, no_constraint, read_only_property, no_serialization) m_keyframe_command;
	time_property_t m_time_input;
	value_property_t m_value_input;
	k3d_data(value_t, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::no_undo, k3d::data::computed_storage, k3d::data::no_constraint, k3d::data::read_only_property, k3d::data::no_serialization) m_output_value;
	keyframes_t m_keyframes;
	keytimes_t m_keytimes;
	k3d_data(interpolator_t*, k3d::data::immutable_name, k3d::data::change_signal, k3d::data::with_undo, k3d::data::node_storage, k3d::data::no_constraint, k3d::data::node_property, k3d::data::node_serialization) m_interpolator;
};

/////////////////////////////////////////////////////////////////////////////
// specific instances and factories

class animation_track_double_matrix4 : public animation_track<double, k3d::matrix4>
{
	typedef animation_track<double, k3d::matrix4> base;
public:
	animation_track_double_matrix4(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document, 0.0, k3d::identity3D()) {}
	
	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<animation_track_double_matrix4>factory(
				k3d::uuid(0x00347e9b, 0x97486a2b, 0xb79e71ab, 0xc719354f),
				"AnimationTrackDoubleMatrix4",
				("Stores a series of keyframes for an animation, using 'double' as time type and 'matrix4' as value"),
				"Animation",
				k3d::iplugin_factory::EXPERIMENTAL);
		return factory;
	}
};

k3d::iplugin_factory& animation_track_double_matrix4_factory()
{
	return animation_track_double_matrix4::get_factory();
}

} // namespace libk3danimation
