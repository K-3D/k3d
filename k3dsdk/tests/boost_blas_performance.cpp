#define NDEBUG

#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>

int main(int argc, char* argv[])
{
	boost::numeric::ublas::c_vector<double, 3> v;
	v[0] = 0;
	v[1] = 0;
	v[2] = 0;

	boost::numeric::ublas::c_vector<double, 3> a;
	a[0] = 1;
	a[1] = 2;
	a[2] = 3;

	for(int i = 0; i != 10000000; ++i)
		v += a;

	std::cout << v << std::endl;

	return 0;
}

