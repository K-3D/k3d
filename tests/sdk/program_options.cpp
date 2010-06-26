#include <boost/program_options.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

#define test_expression(expression) \
	if(!(expression)) \
	{ \
		std::ostringstream buffer; \
		buffer << #expression << " failed at " << __FILE__ << ": " << __LINE__; \
		throw std::runtime_error(buffer.str()); \
	} \

int main(int argc, char* argv[])
{
	try
	{
		boost::program_options::options_description description("test options");
		description.add_options()
			("script,e", boost::program_options::value<std::string>(), "")
			("script-file,f", boost::program_options::value<std::string>(), "")
			;

		std::vector<std::string> raw_arguments;
		raw_arguments.push_back("--script");
		raw_arguments.push_back("# python\nimport k3d\n");

		typedef std::vector<boost::program_options::basic_option<char> > arguments_t;
		const arguments_t arguments = boost::program_options::command_line_parser(raw_arguments)
			.style(
				boost::program_options::command_line_style::allow_long
				| boost::program_options::command_line_style::allow_short
				| boost::program_options::command_line_style::allow_dash_for_short
				| boost::program_options::command_line_style::long_allow_adjacent
				| boost::program_options::command_line_style::long_allow_next
				| boost::program_options::command_line_style::short_allow_adjacent
				| boost::program_options::command_line_style::short_allow_next
				)
			.options(description)
			.allow_unregistered()
			.run()
			.options;

		test_expression(arguments.size() == 1);
		test_expression(arguments[0].string_key == "script");
	}
	catch(std::exception& e)
	{
		std::cerr << "uncaught exception: " << e.what() << std::endl;
		return 1;
	}
	catch(...)
	{
		std::cerr << "unknown exception" << std::endl;
		return 1;
	}

	return 0;
}

