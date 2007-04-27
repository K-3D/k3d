#ifndef K3DSDK_HIGH_RES_TIMER_H
#define K3DSDK_HIGH_RES_TIMER_H

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
		\brief Declares timer classes for performance tuning
		\author Ed Millard (emillard@direcway.com)
*/

#include "result.h"

#include <boost/utility.hpp>
#include <iostream>
#include <string>

#include <sys/time.h>

#ifdef K3D_PLATFORM_WIN32
#include <windows.h>
#endif // K3D_PLATFORM_WIN32

namespace k3d
{

////////////////////////////////////////////////////////////////////////////////////////////////
// nanotime

/* Un-comment this if you require more accurate timing for profiling purposes

/// Retrieves a timestamp using an x86-specific CPU counter.  Note that because we are dividing by a fixed number instead of the actual CPU frequency, the result doesn't represent real-world units, but it can still be used to obtain accurate relative timing for profiling.
inline double nanotime()
{
#if defined K3D_HAVE_X86

	unsigned long long int val;
	__asm__ __volatile__("rdtsc" : "=A" (val) : );
	return static_cast<double>(val) / 1000000000.0;
#endif
}

*/

#ifdef K3D_PLATFORM_WIN32

/// Retrieves a timestamp in seconds using the Win32 high performance counters
inline double nanotime()
{
	LARGE_INTEGER timestamp;
	LARGE_INTEGER frequency;
	return_val_if_fail(QueryPerformanceCounter(&timestamp) && QueryPerformanceFrequency(&frequency), 0.0);

	return static_cast<double>(timestamp.QuadPart) / static_cast<double>(frequency.QuadPart);
}

#else // K3D_PLATFORM_WIN32

/// Retrieves a timestamp in seconds using gettimeofday() for portable timing
inline double nanotime()
{
	timeval tv;
	gettimeofday(&tv, 0);

	return tv.tv_sec + static_cast<double>(tv.tv_usec) / 1000000;
}

#endif // !K3D_PLATFORM_WIN32

////////////////////////////////////////////////////////////////////////////////////////////////
// timer

/// Measures elapsed time using a high-resolution timer.  Intentionally modelled on boost::timer
class timer
{
public:
	timer() :
		m_start(nanotime())
	{
	}
	
	void restart()
	{
		m_start = nanotime();
	}
	
	double elapsed() const
	{
		return nanotime() - m_start;
	}
	
private:
	double m_start;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// progress_timer

/// Measures elapsed time using a high-resolution timer, printing the results to a stream on destruction.  Intentionally modelled on boost::progress_timer
class progress_timer :
	public timer,
	public boost::noncopyable
{
public:
	progress_timer(std::ostream& Stream = log()) :
		m_stream(Stream)
	{
	}
	
	~progress_timer()
	{
		m_stream << elapsed() << " s\n";
	}

private:
	std::ostream& m_stream;
};

} // namespace k3d

#endif // !K3DSDK_HIGH_RES_TIMER_H
