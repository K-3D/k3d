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
	\author Joaquin Duo (joaduo@lugmen.org.ar)
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/fstream.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/node.h>
#include <k3dsdk/properties.h>
#include <k3dsdk/string_source.h>

// #include <iostream>

namespace module
{

namespace papagayo_lipsync_reader
{

namespace detail
{

class papagayo_lipsync_reader_implementation
{
	public:
		papagayo_lipsync_reader_implementation()
		{
		
		}
		
		void load_papagayo_file(k3d::filesystem::path path_papagayo_file)
		{
			//clear the last file loaded (if any)
			times_vector.clear();
			mouth_names_vector.clear();
			
			k3d::filesystem::ifstream file(path_papagayo_file);
			if(!file)
			{
				k3d::log() << error << k3d_file_reference << ": error opening [" << path_papagayo_file.native_console_string() << "]" << std::endl;
				return ;
			}
			
			//Check the file header for "MohoSwitch1" and warn if fails
			k3d::string_t line_buffer;
			k3d::getline(file, line_buffer);
			if(line_buffer.compare("MohoSwitch1"))
			{
				k3d::log() << warning << "Probably this is not a papagayo file format. First line should be \"MohoSwitch1\"" << std::endl;
				return ;
			}
			
			//Add a rest state at the beginning for simplicity
			times_vector.push_back(0);
			mouth_names_vector.push_back("rest");
			
			//Load all the other mouths
			k3d::uint_t line_count = 0; // Track the number of lines parsed
			for(k3d::getline(file, line_buffer); file; k3d::getline(file, line_buffer))
			{
				++line_count;
				std::istringstream line_stream(line_buffer);
				k3d::uint_t mouth_frame;
				k3d::string_t mouth_name;
				try
				{
					line_stream >> mouth_frame;
					line_stream >> mouth_name;
					times_vector.push_back(mouth_frame);
					mouth_names_vector.push_back(mouth_name);
				}
				catch(...)
				{
					k3d::log() << error << "Could not parse file at line: ["<< line_count << "] "<< line_buffer << std::endl;
				}
			}
			
			//Add rest mouth at the end for simplicity
			times_vector.push_back(times_vector[times_vector.size()-1]+1);
			mouth_names_vector.push_back("rest");
		}
		
		//Calculate the value for a mouth, given a time value
		double get_mouth_value(k3d::string_t mouth_name,k3d::double_t frame_rate, k3d::double_t time)
		{
			if(times_vector.empty())
				return 0.;
			k3d::uint_t index = std::upper_bound( times_vector.begin(), times_vector.end(), static_cast<k3d::uint_t>(time*frame_rate) ) - times_vector.begin();
			if(index > 0)
				--index;
			if(mouth_names_vector[index].compare(mouth_name) == 0)
				return 1.;
			return 0.;
		}
		
		//Calculate the value for a mouth, given a time value
		double get_mouth_value_interpolated(k3d::string_t mouth_name,k3d::double_t frame_rate, k3d::double_t time, k3d::double_t interpolation_time)
		{
			if(times_vector.empty())
				return 0.;
			k3d::uint_t index_next = std::upper_bound( times_vector.begin(), times_vector.end(), static_cast<k3d::uint_t>(time*frame_rate) ) - times_vector.begin();
			k3d::uint_t index_previous;
			k3d::uint_t index;
			
			//Check extreme cases
			if(index_next <= 1)
			{
				if(mouth_names_vector[0].compare(mouth_name) == 0)
					return 1.;
				else
					return 0.;
			}
			else if(index_next == times_vector.size())
			{
				if(mouth_names_vector[index_next-1].compare(mouth_name) == 0)
					return 1.;
				else
					return 0.;
			}
			
			index = index_next - 1;
			index_previous = index - 1;
			
			//Check if interpolation_time is too long, and correct it
			if( interpolation_time > (times_vector[index_next]/frame_rate - times_vector[index]/frame_rate))
			{
				interpolation_time = (times_vector[index_next]/frame_rate - times_vector[index]/frame_rate)/4.;
			}
			
			if(time <= times_vector[index]/frame_rate + interpolation_time/2.)
			{
				if(mouth_names_vector[index         ].compare(mouth_name) == 0)
					return .5 + .5*(time - times_vector[index]/frame_rate)/interpolation_time;
				if(mouth_names_vector[index_previous].compare(mouth_name) == 0)
					return .5 - .5*(time - times_vector[index]/frame_rate)/interpolation_time;
				return 0.;
			}
			else if(time > times_vector[index_next]/frame_rate - interpolation_time/2.)
			{
				if(mouth_names_vector[index     ].compare(mouth_name) == 0)
					return 1. - .5*(times_vector[index_next]/frame_rate - time)/interpolation_time;
				if(mouth_names_vector[index_next].compare(mouth_name) == 0)
					return      .5*(times_vector[index_next]/frame_rate - time)/interpolation_time;
				return 0.;
			}
			else
			{
				if(mouth_names_vector[index].compare(mouth_name) == 0)
					return 1.;
				return 0.;
			}
		}
	private:
		std::vector<k3d::uint_t> times_vector;
		std::vector<k3d::string_t> mouth_names_vector;
}; //class papagayo_lipsync_reader_implementation

} //namespace detail

/////////////////////////////////////////////////////////////////////////////
// papagayo_lipsync_reader

/// Reads a papagayo .dat file and outputs the values for morphing
class papagayo_lipsync_reader :
	public k3d::node
{
	typedef k3d::node base;

public:
	papagayo_lipsync_reader(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_papagayo_file(init_owner(*this) + init_name("papagayo_file") + init_label(_("Papagayo File")) + init_description(_("Papagayo Lipsync File to Load")) + init_value(k3d::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type("")),
		m_frame_rate(init_owner(*this) + init_name("frame_rate") + init_label(_("Frame Rate")) + init_description(_("Frame rate of the papagayo lipsync file. Frames are converted to time units according to this value.")) + init_value(30.0) + init_step_increment(1) + init_units(typeid(k3d::measurement::scalar)) + init_constraint(constraint::minimum(std::numeric_limits<double>::epsilon()))),
		m_time(init_owner(*this) + init_name("time") + init_label(_("Time")) + init_description(_("Time for the current mouths' values(should be connected to a time source)")) + init_value(0.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::time))),
		m_interpolate(init_owner(*this) + init_name("interpolate") + init_label(_("Interpolate")) + init_description(_("Enables linear interpolation between mouths values.")) + init_value(true)),
		m_interpolation_time(init_owner(*this) + init_name("interpolation_time") + init_label(_("Interpolation Time")) + init_description(_("Time for the transition from mouth to mouth")) + init_value(0.2) + init_step_increment(0.05) + init_units(typeid(k3d::measurement::time))),
		// Mouths names:	AI  E  etc  FV  L  MBP  O  rest  U  WQ
		#define INITIALIZE_MOUTH_PROPERTY(name_mouth) m_##name_mouth(init_owner(*this) + init_name(#name_mouth) + init_label(_(#name_mouth)) + init_description(_(#name_mouth" mouth value."))  + init_value(0.0))
		INITIALIZE_MOUTH_PROPERTY(AI),
		INITIALIZE_MOUTH_PROPERTY(E),
		INITIALIZE_MOUTH_PROPERTY(etc),
		INITIALIZE_MOUTH_PROPERTY(FV),
		INITIALIZE_MOUTH_PROPERTY(L),
		INITIALIZE_MOUTH_PROPERTY(MBP),
		INITIALIZE_MOUTH_PROPERTY(O),
		INITIALIZE_MOUTH_PROPERTY(rest),
		INITIALIZE_MOUTH_PROPERTY(U),
		INITIALIZE_MOUTH_PROPERTY(WQ)
	{
		m_papagayo_file.changed_signal().connect(sigc::mem_fun(*this, &papagayo_lipsync_reader::load_papagayo_file));
		
		#define CONNECT_UPDATE_SLOT(name_mouth) m_##name_mouth.set_update_slot(sigc::mem_fun(*this, &papagayo_lipsync_reader::get_value_##name_mouth))
			CONNECT_UPDATE_SLOT(AI);
			CONNECT_UPDATE_SLOT(E);
			CONNECT_UPDATE_SLOT(etc);
			CONNECT_UPDATE_SLOT(FV);
			CONNECT_UPDATE_SLOT(L);
			CONNECT_UPDATE_SLOT(MBP);
			CONNECT_UPDATE_SLOT(O);
			CONNECT_UPDATE_SLOT(rest);
			CONNECT_UPDATE_SLOT(U);
			CONNECT_UPDATE_SLOT(WQ);
			
		#define CONNECT_CHANGE_SIGNAL(property,name_mouth) property.changed_signal().connect(m_##name_mouth.make_slot())
			//Chaging the file changes the mouths
			CONNECT_CHANGE_SIGNAL(m_papagayo_file,AI);
			CONNECT_CHANGE_SIGNAL(m_papagayo_file,E);
			CONNECT_CHANGE_SIGNAL(m_papagayo_file,etc);
			CONNECT_CHANGE_SIGNAL(m_papagayo_file,FV);
			CONNECT_CHANGE_SIGNAL(m_papagayo_file,L);
			CONNECT_CHANGE_SIGNAL(m_papagayo_file,MBP);
			CONNECT_CHANGE_SIGNAL(m_papagayo_file,O);
			CONNECT_CHANGE_SIGNAL(m_papagayo_file,rest);
			CONNECT_CHANGE_SIGNAL(m_papagayo_file,U);
			CONNECT_CHANGE_SIGNAL(m_papagayo_file,WQ);
			//Chaging the interpolate bool changes the mouths
			CONNECT_CHANGE_SIGNAL(m_interpolate,AI);
			CONNECT_CHANGE_SIGNAL(m_interpolate,E);
			CONNECT_CHANGE_SIGNAL(m_interpolate,etc);
			CONNECT_CHANGE_SIGNAL(m_interpolate,FV);
			CONNECT_CHANGE_SIGNAL(m_interpolate,L);
			CONNECT_CHANGE_SIGNAL(m_interpolate,MBP);
			CONNECT_CHANGE_SIGNAL(m_interpolate,O);
			CONNECT_CHANGE_SIGNAL(m_interpolate,rest);
			CONNECT_CHANGE_SIGNAL(m_interpolate,U);
			CONNECT_CHANGE_SIGNAL(m_interpolate,WQ);
			//Chaging the interpolation_time changes the mouths
			CONNECT_CHANGE_SIGNAL(m_interpolation_time,AI);
			CONNECT_CHANGE_SIGNAL(m_interpolation_time,E);
			CONNECT_CHANGE_SIGNAL(m_interpolation_time,etc);
			CONNECT_CHANGE_SIGNAL(m_interpolation_time,FV);
			CONNECT_CHANGE_SIGNAL(m_interpolation_time,L);
			CONNECT_CHANGE_SIGNAL(m_interpolation_time,MBP);
			CONNECT_CHANGE_SIGNAL(m_interpolation_time,O);
			CONNECT_CHANGE_SIGNAL(m_interpolation_time,rest);
			CONNECT_CHANGE_SIGNAL(m_interpolation_time,U);
			CONNECT_CHANGE_SIGNAL(m_interpolation_time,WQ);
			//Chaging the time changes the mouths
			CONNECT_CHANGE_SIGNAL(m_time,AI);
			CONNECT_CHANGE_SIGNAL(m_time,E);
			CONNECT_CHANGE_SIGNAL(m_time,etc);
			CONNECT_CHANGE_SIGNAL(m_time,FV);
			CONNECT_CHANGE_SIGNAL(m_time,L);
			CONNECT_CHANGE_SIGNAL(m_time,MBP);
			CONNECT_CHANGE_SIGNAL(m_time,O);
			CONNECT_CHANGE_SIGNAL(m_time,rest);
			CONNECT_CHANGE_SIGNAL(m_time,U);
			CONNECT_CHANGE_SIGNAL(m_time,WQ);
	}

k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<papagayo_lipsync_reader, k3d::interface_list<k3d::istring_source> > factory(
			k3d::uuid(0x9e43b56a, 0x3c4198c9, 0xfd0d46b4, 0xb18446ae),
			"PapagayoLipsyncReader",
			_("Load a papagayo lipsync (.dat) file and output its values given a time source"),
			"Lipsync",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
private:
	k3d_data(k3d::filesystem::path, immutable_name, change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization)  m_papagayo_file;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, with_constraint, measurement_property, with_serialization) m_frame_rate;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_time;
	k3d_data(k3d::bool_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization)   m_interpolate;
	k3d_data(k3d::double_t, immutable_name, change_signal, with_undo, local_storage, no_constraint, writable_property, with_serialization) m_interpolation_time;
	//mouths
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_AI;
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_E;
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_etc;
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_FV;
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_L;
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_MBP;
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_O;
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_rest;
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_U;
	k3d_data(k3d::double_t, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_WQ;

	detail::papagayo_lipsync_reader_implementation implementation;
	
	void load_papagayo_file(k3d::ihint* Hint)
	{
		implementation.load_papagayo_file(m_papagayo_file.pipeline_value());
	}

	k3d::double_t get_property_value(k3d::string_t name)
	{
		if(m_interpolate.pipeline_value())
			return implementation.get_mouth_value_interpolated
				(
					name,m_frame_rate.pipeline_value(),m_time.pipeline_value(),m_interpolation_time.pipeline_value()
				);
		else
			return implementation.get_mouth_value(name,m_frame_rate.pipeline_value(),m_time.pipeline_value());
	}

// 	void scalar_source::execute(const std::vector<k3d::ihint*>& Hints, k3d::double_t& Output)
// 	#define FUNCTION_GET_VALUE_MOUTH(name_mouth) k3d::double_t get_value_##name_mouth() {return get_property_value(#name_mouth);}
	#define FUNCTION_GET_VALUE_MOUTH(name_mouth) void get_value_##name_mouth(const std::vector<k3d::ihint*>& Hints, k3d::double_t& Output) { Output = get_property_value(#name_mouth);}
		FUNCTION_GET_VALUE_MOUTH(AI)
		FUNCTION_GET_VALUE_MOUTH(E)
		FUNCTION_GET_VALUE_MOUTH(etc)
		FUNCTION_GET_VALUE_MOUTH(FV)
		FUNCTION_GET_VALUE_MOUTH(L)
		FUNCTION_GET_VALUE_MOUTH(MBP)
		FUNCTION_GET_VALUE_MOUTH(O)
		FUNCTION_GET_VALUE_MOUTH(rest)
		FUNCTION_GET_VALUE_MOUTH(U)
		FUNCTION_GET_VALUE_MOUTH(WQ)
	
}; //class papagayo_lipsync_reader


/////////////////////////////////////////////////////////////////////////////
// papagayo_lipsync_reader_factory

k3d::iplugin_factory& papagayo_lipsync_reader_factory()
{
	return papagayo_lipsync_reader::get_factory();
}

} //namespace papagayo_lipsync_reader

} // namespace module

