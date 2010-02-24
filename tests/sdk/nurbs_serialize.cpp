#include <k3dsdk/nurbs.h>

int main(int argc, char* argv[])
{
	k3d::nurbs::curve3 curve(4);
	
	curve.knots.push_back(0);
	curve.knots.push_back(0);
	curve.knots.push_back(0);
	curve.knots.push_back(0);
	curve.knots.push_back(1);
	curve.knots.push_back(1);
	curve.knots.push_back(1);
	curve.knots.push_back(1);

	curve.control_points.push_back(k3d::nurbs::curve3::control_point(k3d::point3(0, 0, 0)));
	curve.control_points.push_back(k3d::nurbs::curve3::control_point(k3d::point3(0, 5, 0)));
	curve.control_points.push_back(k3d::nurbs::curve3::control_point(k3d::point3(5, 5, 0)));
	curve.control_points.push_back(k3d::nurbs::curve3::control_point(k3d::point3(5, 0, 0)));

	std::cout << curve << std::endl;
	
	return 0;
}

