#include <k3dsdk/socket.h>
#include <k3dsdk/system.h>
#include <iostream>

int main(int argc, char* argv[])
{
	const int port = 10000;
	
	try
	{
		k3d::socket::endpoint server = k3d::socket::listen(port);
		server.set_non_blocking();
		std::cerr << "listening on port " << port << std::endl;
			
		while(true)
		{
			try
			{
				k3d::socket::endpoint connection = server.accept();
				std::cerr << "client connected" << std::endl;

				while(true)
				{
					std::string buffer;
					connection.read(buffer);
					connection.write(buffer);
					std::cerr << buffer << std::flush;
				}
			}
			catch(k3d::socket::would_block& e)
			{
				std::cerr << "waiting" << std::endl;
				k3d::system::sleep(1);
			}
		}
	}
	catch(k3d::socket::closed& e)
	{
		std::cerr << "client disconnected" << std::endl;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;	
	}
}

