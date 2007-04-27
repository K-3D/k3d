#include <k3dsdk/vectors.h>
#include <iostream>

int main(int argc, char* argv[])
{
	k3d::vector3 v(0, 0, 0);
	k3d::vector3 a(1, 2, 3);

	for(int i = 0; i != 10000000; ++i)
		v += a;

	std::cout << v << std::endl;

	return 0;
}

