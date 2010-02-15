#include <k3dsdk/nurbs.h>

int main(int argc, char* argv[])
{
	k3d::nurbs::curve3 curve(0);
	std::cin >> curve;
	std::cout << curve << std::endl;
	
	return 0;
}

