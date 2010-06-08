#include <k3d-version-config.h>

#include <k3dsdk/log.h>
#include <k3dsdk/uuid.h>

#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
	k3d::log() << "k3d-uuidgen version " << K3D_VERSION << std::endl;
	k3d::log() << "You can paste the following unique ID into your new K-3D plugin:" << std::endl;

	const k3d::uuid result = k3d::uuid::random();

	std::cout << std::hex << std::setfill('0');
	for(int i = 0; i != result.size(); ++i)
	{
		if(i && (0 == i % 4))
			std::cout << ", ";
		if(0 == i % 4)
			std::cout << "0x";
		std::cout << std::setw(2) << static_cast<k3d::uint32_t>(result.data[i]);
	}
	std::cout << std::endl;

	return 0;
}

