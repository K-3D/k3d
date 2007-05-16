#include <k3dsdk/almost_equal.h>

#include <cassert>
#include <iomanip>
#include <iostream>
#include <stdexcept>

void test_difference(const double A, const double B, const int64_t Expected)
{
	const int64_t difference = k3d::representable_difference(A, B);

	std::cout << std::setprecision(17) << k3d::to_integer(A) << " " << k3d::to_integer(B) << " difference(" << A << ", " << B << ") = " << difference << std::endl;
	if(difference != Expected)
		throw std::runtime_error("unexpected value");
}

int main(int argc, char* argv[])
{
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
	test_difference(a, e, -2);
	test_difference(a, f, -4);

	return 0;
}

