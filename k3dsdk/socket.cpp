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

#include "k3d-platform-config.h"
#include "result.h"
#include "socket.h"
#include "string_cast.h"

#include <cstring>

#ifdef K3D_API_WIN32

	#include "winsock2.h"

#else // K3D_API_WIN32

	#include <errno.h>
	#include <fcntl.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/types.h>

	extern int h_errno;

#endif // !K3D_API_WIN32

#ifdef K3D_API_DARWIN

	#define MSG_NOSIGNAL SO_NOSIGPIPE

#endif // K3D_API_DARWIN

namespace k3d
{

namespace socket
{

/////////////////////////////////////////////////////////////////////
// exception

exception::exception(const std::string& message) :
	std::runtime_error(message)
{
}

/////////////////////////////////////////////////////////////////////
// closed

closed::closed() :
	exception("socket closed")
{
}

/////////////////////////////////////////////////////////////////////
// would_block

would_block::would_block() :
	exception("socket operation would block")
{
}

/////////////////////////////////////////////////////////////////////
// endpoint::implementation

#ifdef K3D_API_WIN32

	/// Helper class that handles one-time winsock initialization
	class wsa_startup
	{
	public:
		wsa_startup()
		{
			if(initialized)
				return;

			const int result = WSAStartup(MAKEWORD(1, 1), &data);
			if(0 != result)
				throw socket::exception("winsock initialization error: " + k3d::string_cast(result));

			initialized = true;
		}

	private:
		static bool initialized;
		static WSADATA data;
	};

	bool wsa_startup::initialized = false;
	WSADATA wsa_startup::data;

	/// Provides a winsock implementation of k3d::socket::endpoint
	class endpoint::implementation
	{
	public:
		implementation(SOCKET Socket) :
			socket(Socket)
		{
			set_blocking();
		}

		implementation(const port& Port) :
			socket(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
		{
			if(INVALID_SOCKET == socket)
				throw_exception();

			set_blocking();

			SOCKADDR_IN address;
			address.sin_family = AF_INET;
			address.sin_port = htons(Port);
			address.sin_addr.s_addr = htonl(INADDR_ANY);

			if(SOCKET_ERROR == ::bind(socket, reinterpret_cast<SOCKADDR*>(&address), sizeof(address)))
				throw_exception();

			if(SOCKET_ERROR == ::listen(socket, SOMAXCONN))
				throw_exception();
		}

		implementation(const address& Address, const port& Port) :
			socket(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
		{
			if(INVALID_SOCKET == socket)
				throw_exception();

			set_blocking();

			hostent* const host = gethostbyname(Address.c_str());
			if(!host)
				throw_exception();

			SOCKADDR_IN address;
			memset(&address, 0, sizeof(address));
			address.sin_family = AF_INET;
			address.sin_port = htons(Port);
			memcpy(&address.sin_addr, host->h_addr, host->h_length);

			if(SOCKET_ERROR == ::connect(socket, reinterpret_cast<SOCKADDR*>(&address), sizeof(address)))
				throw_exception();
		}

		~implementation()
		{
			// Never throw exceptions from dtors!
			if(SOCKET_ERROR == ::closesocket(socket))
				k3d::log() << error << "error closing socket: " << WSAGetLastError() << std::endl;
		}

		void set_blocking()
		{
			int mode = 0;
			::ioctlsocket(socket, FIONBIO, reinterpret_cast<u_long*>(&mode));
		}

		void set_non_blocking()
		{
			int mode = 1;
			::ioctlsocket(socket, FIONBIO, reinterpret_cast<u_long*>(&mode));
		}

		endpoint accept()
		{
			const SOCKET new_connection = ::accept(socket, 0, 0);
			if(INVALID_SOCKET == new_connection)
				throw_exception();

			return endpoint(new implementation(new_connection));
		}

		void write(const char* buffer, const size_t length)
		{
			if(SOCKET_ERROR == ::send(socket, buffer, length, 0))
				throw_exception();
		}

		size_t read(char* buffer, const size_t length)
		{
			int bytes_read = ::recv(socket, buffer, length, 0);
			if(0 == bytes_read)
                throw socket::closed();
			if(SOCKET_ERROR == bytes_read)
				throw_exception();

			return bytes_read;
		}

	private:
		void throw_exception()
		{
			switch(WSAGetLastError())
			{
				case WSAECONNRESET:
					throw socket::closed();

				case WSAEWOULDBLOCK:
					throw socket::would_block();

				default:
                {
                    std::string buffer(2048, '\0');
                    buffer.resize(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), const_cast<char*>(buffer.data()), buffer.size(), 0));
                    std::replace(buffer.begin(), buffer.end(), '\r', ' ');
                    std::replace(buffer.begin(), buffer.end(), '\n', ' ');
                    throw socket::exception(buffer);
                }
			}
		}

		wsa_startup startup;
		const SOCKET socket;
	};

#else // K3D_API_WIN32

	/// Provides a posix implementation of k3d::socket::endpoint
	class endpoint::implementation
	{
	public:
		implementation(const int Socket) :
			socket(Socket)
		{
			set_blocking();
		}

		implementation(const port& Port) :
			socket(::socket(AF_INET, SOCK_STREAM, 0))
		{
			if(-1 == socket)
				throw_exception();

			set_blocking();

			sockaddr_in address;
			memset(&address, 0, sizeof(address));
			address.sin_family = AF_INET;
			address.sin_port = htons(Port);
			address.sin_addr.s_addr = htonl(INADDR_ANY);

			if(-1 == ::bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)))
				throw_exception();

			if(-1 == ::listen(socket, SOMAXCONN))
				throw_exception();
		}

		implementation(const address& Address, const port& Port) :
			socket(::socket(AF_INET, SOCK_STREAM, 0))
		{
			if(-1 == socket)
				throw_exception();

			set_blocking();

			hostent* const host = gethostbyname(Address.c_str());
			if(!host)
				throw socket::exception(hstrerror(h_errno));

			sockaddr_in address;
			memset(&address, 0, sizeof(address));
			address.sin_family = AF_INET;
			address.sin_port = htons(Port);
			memcpy(&address.sin_addr, host->h_addr, host->h_length);

			if(-1 == ::connect(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)))
				throw_exception();
		}

		~implementation()
		{
			// Never throw exceptions from a dtor!
			if(-1 == ::close(socket))
				k3d::log() << error << "error closing socket: " << strerror(errno) << std::endl;
		}

		void set_blocking()
		{
			int options = fcntl(socket, F_GETFL, 0);
			options &= ~O_NONBLOCK;

			if(-1 == ::fcntl(socket, F_SETFL, options))
				throw_exception();
		}

		void set_non_blocking()
		{
			int options = fcntl(socket, F_GETFL, 0);
			options |= O_NONBLOCK;

			if(-1 == ::fcntl(socket, F_SETFL, options))
				throw_exception();
		}

		endpoint accept()
		{
			const int new_connection = ::accept(socket, 0, 0);
			if(-1 == new_connection)
				throw_exception();

			return endpoint(new implementation(new_connection));
		}

		void write(const char* buffer, const size_t length)
		{
			if(-1 == ::send(socket, buffer, length, MSG_NOSIGNAL))
				throw_exception();
		}

		size_t read(char* buffer, const size_t length)
		{
			int bytes_read = ::recv(socket, buffer, length, 0);
			if(-1 == bytes_read)
				throw_exception();

			if(0 == bytes_read)
				throw socket::closed();

			return bytes_read;
		}

	private:
		void throw_exception()
		{
			switch(errno)
			{
				case EAGAIN:
					throw socket::would_block();

				case ECONNRESET:
				case EPIPE:
					throw socket::closed();

				default:
					throw socket::exception(strerror(errno));
			}
		}

		const int socket;
	};

#endif // !K3D_API_WIN32

/////////////////////////////////////////////////////////////////////
// endpoint

endpoint::endpoint(implementation* const Implementation) :
	m_implementation(Implementation)
{
}

endpoint::~endpoint()
{
	delete m_implementation;
}

void endpoint::set_blocking()
{
	m_implementation->set_blocking();
}

void endpoint::set_non_blocking()
{
	m_implementation->set_non_blocking();
}

endpoint endpoint::accept()
{
	return m_implementation->accept();
}

void endpoint::write(const char* Buffer, const size_t Length)
{
	m_implementation->write(Buffer, Length);
}

void endpoint::write(const std::string& Buffer)
{
	m_implementation->write(Buffer.data(), Buffer.size());
}

size_t endpoint::read(char* Buffer, const size_t Length)
{
	return m_implementation->read(Buffer, Length);
}

void endpoint::read(std::string& Buffer, const size_t Length)
{
	Buffer.resize(Length, '\0');
	const size_t bytes_read = m_implementation->read(const_cast<char*>(Buffer.data()), Buffer.size());
	Buffer.resize(bytes_read);
}

/////////////////////////////////////////////////////////////////////
// listen

endpoint listen(const port& Port)
{
	return endpoint(new endpoint::implementation(Port));
}

/////////////////////////////////////////////////////////////////////
// connect

endpoint connect(const address& Host, const port& Port)
{
	return endpoint(new endpoint::implementation(Host, Port));
}

} // namespace socket

} // namespace k3d
