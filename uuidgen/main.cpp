#include <k3dsdk/log.h>
#include <k3dsdk/uuid.h>
#include <k3dsdk/version.h>

#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
	k3d::log() << "k3d-uuidgen version " << K3D_VERSION << std::endl;
	k3d::log() << "You can paste the following unique ID into your new K-3D plugin:" << std::endl;

	const k3d::uuid result = k3d::uuid::random();

	std::cout << std::hex << std::setfill('0') << "0x" << std::setw(8) << result.data1 << ", 0x" << std::setw(8) << result.data2 << ", 0x" << std::setw(8) << result.data3 << ", 0x" << std::setw(8) << result.data4 << std::endl;

	return 0;
}

