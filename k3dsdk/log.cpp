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

#include "log.h"
#include "log_control.h"
#include "k3d-platform-config.h"
#include "result.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef K3D_API_WIN32

	#include <time.h>
	#include "win32.h"

#elif defined K3D_API_DARWIN

	#include <syslog.h>

#else

	#include <syslog.h>
	#include <execinfo.h>

#endif

namespace k3d
{

namespace detail
{

///////////////////////////////////////////////////////////
// log_level_index

int log_level_index()
{
	static int index = std::ios::xalloc();
	return index;
}

///////////////////////////////////////////////////////////
// log_level

long& log_level(std::ostream& Stream)
{
	return Stream.iword(log_level_index());
}

bool g_log_show_timestamp = false;
std::string g_log_tag;
bool g_log_color_level = false;
bool g_log_show_level = false;
bool g_log_syslog = false;
int g_log_minimum_level = K3D_LOG_LEVEL_WARNING;

std::vector<time_t> g_log_timestamp_cache;
std::vector<log_level_t> g_log_level_cache;
std::vector<std::string> g_log_message_cache;

///////////////////////////////////////////////////////////
// log_cerr

void log_cerr(const time_t Timestamp, const log_level_t Level, const std::string& Message)
{
	if(Level > g_log_minimum_level)
		return;

	if(g_log_show_timestamp)
	{
		std::string buffer(256, '\0');
		buffer.resize(strftime(&buffer[0], buffer.size(), "%m/%d/%Y %H:%M:%S ", localtime(&Timestamp)));
		std::cerr << buffer;
	}

	if(!g_log_tag.empty())
	{
		std::cerr << g_log_tag;
	}

	if(g_log_color_level)
	{
		switch(Level)
		{
			case K3D_LOG_LEVEL_CRITICAL:
				std::cerr << "\e[1;31m";
				break;
			case K3D_LOG_LEVEL_ERROR:
				std::cerr << "\e[1;31m";
				break;
			case K3D_LOG_LEVEL_WARNING:
				std::cerr << "\e[1;33m";
				break;
			case K3D_LOG_LEVEL_INFO:
				std::cerr << "\e[0m";
				break;
			case K3D_LOG_LEVEL_DEBUG:
				std::cerr << "\e[1;32m";
				break;
		}
	}

	if(g_log_show_level)
	{
		switch(Level)
		{
			case K3D_LOG_LEVEL_CRITICAL:
				std::cerr << "CRITICAL: ";
				break;
			case K3D_LOG_LEVEL_ERROR:
				std::cerr << "ERROR ";
				break;
			case K3D_LOG_LEVEL_WARNING:
				std::cerr << "WARNING: ";
				break;
			case K3D_LOG_LEVEL_INFO:
				std::cerr << "INFO: ";
				break;
			case K3D_LOG_LEVEL_DEBUG:
				std::cerr << "DEBUG: ";
				break;
		}
	}

	std::cerr << Message;

	if(g_log_color_level)
	{
		std::cerr << "\e[0m";
	}

	std::cerr << std::flush;
}

///////////////////////////////////////////////////////////
// log_cache

void log_cache(const time_t Timestamp, const log_level_t Level, const std::string& Message)
{
	g_log_timestamp_cache.push_back(Timestamp);
	g_log_level_cache.push_back(Level);
	g_log_message_cache.push_back(Message);
}

///////////////////////////////////////////////////////////
// log_syslog

#ifndef K3D_API_WIN32

void log_syslog(const time_t Timestamp, const log_level_t Level, const std::string& Message)
{
	if(!g_log_syslog)
		return;

	int priority = LOG_INFO;

	switch(Level)
	{
		case K3D_LOG_LEVEL_CRITICAL:
			priority = LOG_CRIT;
			break;
		case K3D_LOG_LEVEL_ERROR:
			priority = LOG_ERR;
			break;
		case K3D_LOG_LEVEL_WARNING:
			priority = LOG_WARNING;
			break;
		case K3D_LOG_LEVEL_INFO:
			priority = LOG_INFO;
			break;
		case K3D_LOG_LEVEL_DEBUG:
			priority = LOG_DEBUG;
			break;
	}

	syslog(LOG_USER | priority, "%s", Message.c_str());
}

#endif // !K3D_API_WIN32

///////////////////////////////////////////////////////////
// log_output_debug_string

#ifdef K3D_API_WIN32

void log_output_debug_string(const time_t Timestamp, const log_level_t Level, const std::string& Message)
{
	OutputDebugString((Message).c_str());
}

#endif // !K3D_API_WIN32

///////////////////////////////////////////////////////////
// signal_buf

/// When attached to an output stream, emits a signal containing each line of output
class signal_buf :
	public std::streambuf
{
public:
	signal_buf() :
		m_stream(0)
	{
		connect(sigc::ptr_fun(log_cerr));
		connect(sigc::ptr_fun(log_cache));

#ifdef K3D_API_WIN32
		connect(sigc::ptr_fun(log_output_debug_string));
#else // K3D_API_WIN32
		connect(sigc::ptr_fun(log_syslog));
#endif // !K3D_API_WIN32
	}

	void init(std::ostream& Stream)
	{
		m_stream = &Stream;
	}

	sigc::connection connect(const sigc::slot<void, const time_t, const log_level_t, const std::string&>& Slot)
	{
		return m_signal.connect(Slot);
	}

protected:
	int overflow(int c)
	{
		if(traits_type::eq_int_type(c, traits_type::eof()))
			return traits_type::eof();

		m_buffer.push_back(c);

		if(c == '\n')
		{
			const time_t current_time = time(0);
			const log_level_t current_log_level = static_cast<log_level_t>(log_level(*m_stream));

			m_signal.emit(current_time, current_log_level, m_buffer);
			m_buffer.clear();

			log_level(*m_stream) = 0;
		}

		return traits_type::not_eof(c);
	}

private:
	std::ostream* m_stream;
	std::string m_buffer;
	sigc::signal<void, const time_t, const log_level_t, const std::string&> m_signal;
};

///////////////////////////////////////////////////////////
// log_stream_init

class log_stream_init
{
public:
	signal_buf* buf()
	{
		return &m_signal_buffer;
	}

private:
	signal_buf m_signal_buffer;
};

///////////////////////////////////////////////////////////
// log_stream

class log_stream :
	private virtual log_stream_init,
	public std::ostream
{
public:
	static log_stream& instance()
	{
		static log_stream m_instance;
		return m_instance;
	}

	sigc::connection connect(const sigc::slot<void, const time_t, const log_level_t, const std::string&>& Slot)
	{
		return log_stream_init::buf()->connect(Slot);
	}

private:
	log_stream() :
		log_stream_init(),
		std::ostream(log_stream_init::buf())
	{
		log_stream_init::buf()->init(*this);
	}
};

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// log

std::ostream& log()
{
	return detail::log_stream::instance();
}

/////////////////////////////////////////////////////////////////////////////
// critical

std::ostream& critical(std::ostream& Stream)
{
	detail::log_level(Stream) = K3D_LOG_LEVEL_CRITICAL;
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// error

std::ostream& error(std::ostream& Stream)
{
	detail::log_level(Stream) = K3D_LOG_LEVEL_ERROR;
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// warning

std::ostream& warning(std::ostream& Stream)
{
	detail::log_level(Stream) = K3D_LOG_LEVEL_WARNING;
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// info

std::ostream& info(std::ostream& Stream)
{
	detail::log_level(Stream) = K3D_LOG_LEVEL_INFO;
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// debug

std::ostream& debug(std::ostream& Stream)
{
	detail::log_level(Stream) = K3D_LOG_LEVEL_DEBUG;
	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// backtrace

std::ostream& backtrace(std::ostream& Stream)
{
#if defined K3D_API_WIN32

	Stream << "Backtrace unavailable on Win32\n";

#elif defined K3D_API_DARWIN

	Stream << "Backtrace unavailable on Darwin\n";

#else

	void* array[25];
	int size = ::backtrace(array, 25);
	char** symbols = ::backtrace_symbols(array, size);

	for(int i = 0; i != size; ++i)
		Stream << symbols[i] << "\n";

	free(symbols);

#endif

	return Stream;
}

/////////////////////////////////////////////////////////////////////////////
// print_debug

void print_debug(const std::string& Message)
{
	log() << debug << Message << std::endl;
}

void log_show_timestamps(bool Enable)
{
	detail::g_log_show_timestamp = Enable;
}

void log_set_tag(const std::string& Tag)
{
	detail::g_log_tag = Tag + " ";
}

void log_color_level(bool Enable)
{
	detail::g_log_color_level = Enable;
}

void log_show_level(bool Enable)
{
	detail::g_log_show_level = Enable;
}

void log_syslog(bool Enable)
{
	detail::g_log_syslog = Enable;
}

void log_minimum_level(const log_level_t Level)
{
	detail::g_log_minimum_level = Level;
}

/////////////////////////////////////////////////////////////////////////////
// connect_log_message

sigc::connection connect_log_message(const sigc::slot<void, const time_t, const log_level_t, const std::string&>& Slot)
{
	return detail::log_stream::instance().connect(Slot);
}

/////////////////////////////////////////////////////////////////////////////
// get_log_cache

void get_log_cache(const sigc::slot<void, const time_t, const log_level_t, const std::string&>& Slot)
{
	for(size_t i = 0; i != detail::g_log_timestamp_cache.size(); ++i)
		Slot(detail::g_log_timestamp_cache[i], detail::g_log_level_cache[i], detail::g_log_message_cache[i]);
}

} // namespace k3d
