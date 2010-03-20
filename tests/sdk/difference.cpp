#include <k3dsdk/difference.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/vectors.h>

#include <cassert>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include <limits>

template<typename T>
void test_difference(const T& A, const T& B, const k3d::uint64_t Threshold)
{
	std::cout << std::setprecision(17) << k3d::demangle(typeid(T)) << ": " << A << " == " << B << std::endl;

	const k3d::difference::accumulator result = k3d::difference::test(A, B);

	if(boost::accumulators::count(result.exact) && boost::accumulators::min(result.exact) == false)
		throw std::runtime_error("exact value mismatch");

	if(boost::accumulators::count(result.ulps) && boost::accumulators::max(result.ulps) > Threshold)
		throw std::runtime_error("ulps exceed threshold");
}

int main(int argc, char* argv[])
{
	try
	{
		test_difference(true, true, 0);
		test_difference(false, false, 0);

		test_difference(123, 123, 0);

		const double a = 1.0;
		const double b = a + std::numeric_limits<double>::epsilon();
		const double c = b + std::numeric_limits<double>::epsilon();
		const double d = c + std::numeric_limits<double>::epsilon();
		const double e = a - std::numeric_limits<double>::epsilon();
		const double f = e - std::numeric_limits<double>::epsilon();

		test_difference(a, a, 0);
		test_difference(a, b, 1);
		test_difference(a, c, 2);
		test_difference(a, d, 3);
		test_difference(a, e, 2);
		test_difference(a, f, 4);

		test_difference(k3d::point2(a, a), k3d::point2(a, a), 0);
		test_difference(k3d::point2(a, a), k3d::point2(b, b), 1);

		test_difference(k3d::point3(a, a, a), k3d::point3(a, a, a), 0);
		test_difference(k3d::point3(a, a, a), k3d::point3(b, b, b), 1);

		test_difference(k3d::point4(a, a, a, a), k3d::point4(a, a, a, a), 0);
		test_difference(k3d::point4(a, a, a, a), k3d::point4(b, b, b, b), 1);

		test_difference(k3d::vector2(a, a), k3d::vector2(a, a), 0);
		test_difference(k3d::vector2(a, a), k3d::vector2(b, b), 1);

		test_difference(k3d::vector3(a, a, a), k3d::vector3(a, a, a), 0);
		test_difference(k3d::vector3(a, a, a), k3d::vector3(b, b, b), 1);

		test_difference(k3d::normal3(a, a, a), k3d::normal3(a, a, a), 0);
		test_difference(k3d::normal3(a, a, a), k3d::normal3(b, b, b), 1);

		return 0;
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
}

