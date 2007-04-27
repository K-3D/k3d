#include <k3dsdk/legacy_mesh.h>
#include <k3dsdk/new_mesh.h>

#include <iostream>

int main(int argc, char* argv[])
{
	std::cout << "sizeof(k3d::legacy::mesh): " << sizeof(k3d::legacy::mesh) << std::endl;

	std::cout << "sizeof(k3d::dev::mesh::points_t): " << sizeof(k3d::dev::mesh::points_t) << std::endl;
	std::cout << "sizeof(k3d::dev::mesh): " << sizeof(k3d::dev::mesh) << std::endl;

	return 0;
}

