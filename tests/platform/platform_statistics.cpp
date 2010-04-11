#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>
using namespace boost::accumulators;

#include <boost/lexical_cast.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
	try
	{
		accumulator_set<bool, stats<tag::min, tag::mean, tag::max, tag::median, tag::lazy_variance> > model;
		for(int i = 1; i < argc; ++i)
		{
			model(boost::lexical_cast<bool>(argv[i]));
		}

		std::cerr << "count: " << count(model) << std::endl;
		std::cerr << "min: " << min(model) << std::endl;
		std::cerr << "mean: " << mean(model) << std::endl;
		std::cerr << "median: " << median(model) << std::endl;
		std::cerr << "max: " << max(model) << std::endl;
		std::cerr << "sd: " << std::sqrt(variance(model)) << std::endl;

		return 0;
	}
	catch(std::exception& e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
		return 1;
	}
}

