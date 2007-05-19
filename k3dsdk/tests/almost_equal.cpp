#include <k3dsdk/almost_equal.h>
#include <k3dsdk/types.h>
#include <k3dsdk/vectors.h>

#include <cassert>
#include <iomanip>
#include <iostream>
#include <stdexcept>

template<typename T>
void test_almost_equal(const T& A, const T& B, const boost::uint64_t Threshold)
{
	std::cout << std::setprecision(17) << k3d::demangle(typeid(T)) << ": " << A << " == " << B << std::endl;
	if(!k3d::almost_equal<T>(Threshold)(A, B))
		throw std::runtime_error("values not equal");
}

int main(int argc, char* argv[])
{
	try
	{
		test_almost_equal(true, true, 0);
		test_almost_equal(false, false, 0);

		test_almost_equal(123, 123, 0);

		const double a = 1.0;
		const double b = a + std::numeric_limits<double>::epsilon();
		const double c = b + std::numeric_limits<double>::epsilon();
		const double d = c + std::numeric_limits<double>::epsilon();
		const double e = a - std::numeric_limits<double>::epsilon();
		const double f = e - std::numeric_limits<double>::epsilon();

		test_almost_equal(a, a, 0);
		test_almost_equal(a, b, 1);
		test_almost_equal(a, c, 2);
		test_almost_equal(a, d, 3);
		test_almost_equal(a, e, 2);
		test_almost_equal(a, f, 4);

		test_almost_equal(k3d::point2(a, a), k3d::point2(a, a), 0);
		test_almost_equal(k3d::point2(a, a), k3d::point2(b, b), 1);

		test_almost_equal(k3d::point3(a, a, a), k3d::point3(a, a, a), 0);
		test_almost_equal(k3d::point3(a, a, a), k3d::point3(b, b, b), 1);

		test_almost_equal(k3d::point4(a, a, a, a), k3d::point4(a, a, a, a), 0);
		test_almost_equal(k3d::point4(a, a, a, a), k3d::point4(b, b, b, b), 1);

		test_almost_equal(k3d::vector2(a, a), k3d::vector2(a, a), 0);
		test_almost_equal(k3d::vector2(a, a), k3d::vector2(b, b), 1);

		test_almost_equal(k3d::vector3(a, a, a), k3d::vector3(a, a, a), 0);
		test_almost_equal(k3d::vector3(a, a, a), k3d::vector3(b, b, b), 1);

		test_almost_equal(k3d::normal3(a, a, a), k3d::normal3(a, a, a), 0);
		test_almost_equal(k3d::normal3(a, a, a), k3d::normal3(b, b, b), 1);

		return 0;
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
}

