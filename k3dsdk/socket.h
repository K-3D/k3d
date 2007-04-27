#ifndef K3DSDK_SOCKET_H
#define K3DSDK_SOCKET_H

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
*/

#include <boost/cstdint.hpp>
#include <stdexcept>

namespace k3d
{

namespace socket
{

/// Defines storage for a host address (hostname or IP address in dotted-quad notation)
typedef std::string address;
/// Defines storage for a TCP port number
typedef boost::uint16_t port;

/// Base-class for all exceptions thrown by sockets
class exception :
	public std::runtime_error
{
public:
	exception(const std::string& message);
};

/// Exception thrown when the opposite end of a connection has closed
class closed :
	public exception
{
public:
	closed();
};

/// Exception thrown when a non-blocking socket operation would block
class would_block :
	public exception
{
public:
	would_block();
};

/// Encapsulates one end of an open socket connection
class endpoint
{
public:
	~endpoint();

	/// Sets blocking behavior for the endpoint (endpoints are blocking by default)
	void set_blocking();
	/// Sets non-blocking behavior for the endpoint (non-blocking endpoints throw a would_block exception if an operation would block)
	void set_non_blocking();

	/// Accepts an incoming connection, returning an endpoint for communicating with the remote host (server sockets only)
	/** \note The new connection will be blocking, regardless of the state of the listening socket */
	endpoint accept();

	/// Writes a buffer of given length to the socket
	void write(const char* Buffer, const size_t Length);
	/// Writes a string to the socket
	void write(const std::string& Buffer);
	
	/// Reads up to Length bytes from the socket into the given buffer, returns the number of bytes read
	size_t read(char* Buffer, const size_t Length);
	/// Reads up to Length bytes from the socket into a string, resizing the string to reflect the number of bytes read
	void read(std::string& Buffer, const size_t Length = 4096);

private:
	class implementation;
	implementation* const m_implementation;

	endpoint(implementation* const);

	friend endpoint listen(const port&);
	friend endpoint connect(const address&, const port&);
};

/// Creates a listening socket, bound to a local port (a server), throws on failure
endpoint listen(const port& Port);
/// Connects to a listening server, throws on failure
endpoint connect(const address& Host, const port& Port);

} // namespace socket

} // namespace k3d

#endif // !K3DSDK_SOCKET_H


