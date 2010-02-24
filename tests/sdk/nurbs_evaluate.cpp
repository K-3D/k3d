#include <k3dsdk/nurbs.h>

int main(int argc, char* argv[])
{
	k3d::nurbs::curve3 curve(0);
	std::cin >> curve;

	double parameter = 0;
	for(std::cin >> parameter; std::cin; std::cin >> parameter)
		std::cout << k3d::nurbs::evaluate(curve, parameter) << " ";
	
	return 0;
}

