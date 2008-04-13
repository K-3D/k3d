#include <boost/cstdint.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>

namespace po = boost::program_options;

void generate_resource(const std::string& InputPath, std::istream& Input, const std::string& ResourcePath, std::ostream& Output)
{
	Output << "//////////////////////////////////////////\n";
	Output << "// Generated file, do not edit by hand! //\n";
	Output << "//////////////////////////////////////////\n\n";

	Output << "#include <k3dsdk/resource/resource.h>\n\n";

	Output << "// Resource compiled from " << InputPath << "\n\n";

	Output << "static unsigned char resource_data[] = {";
	int64_t byte_count = 0;
	Output << std::hex << std::setfill('0');
	for(int c = Input.get(); Input; c = Input.get(), ++byte_count)
	{
		if(0 != byte_count)
			Output << ", ";

		if(0 == byte_count % 8)
			Output << "\n\t";

		Output << "0x" << std::setw(2) << c;
	}
	Output << std::dec << "\n};\n\n";

	Output << "static bool success = k3d::resource::initialize(\"" << ResourcePath << "\", &resource_data[0], " << byte_count << ");\n\n";
}

int main(int argc, char* argv[])
{
	try
	{
		po::options_description visible_options("Usage: k3d-resource-compiler [OPTIONS] --input FILE --path RESOURCE --output FILE");
		visible_options.add_options()
			("help,h", "Prints this help message and exits.")
			("input", po::value<std::string>(), "Input file to be compiled.")
			("path", po::value<std::string>(), "Output resource path.")
			("output", po::value<std::string>(), "Output resource file. Use \"-\" for stdout.")
			;

		po::variables_map options;
		po::store(po::parse_command_line(argc, argv, visible_options), options);
		po::notify(options);

		if(options.count("help"))
		{
			std::cout << visible_options << "\n";
			return 0;
		}

		if(!options.count("input"))
			throw std::runtime_error("missing input file");
		std::string input_path = options["input"].as<std::string>();
		std::ifstream input_stream(input_path.c_str());

		if(!options.count("path"))
			throw std::runtime_error("missing output path");
		std::string resource_path = options["path"].as<std::string>();

		std::string output_path = "-";
		if(options.count("output"))
			output_path = options["output"].as<std::string>();

		if(output_path == "-")
		{
			generate_resource(input_path, input_stream, resource_path, std::cout);
		}
		else
		{
			std::ofstream output_stream(output_path.c_str());
			generate_resource(input_path, input_stream, resource_path, output_stream);
		}

		return 0;
	}
	catch(std::exception& e)
	{
		std::cerr << "K-3D Resource Compiler: " << e.what() << std::endl;
	}
	catch(...)
	{
		std::cerr << "K-3D Resource Compiler: unknown exception" << std::endl;
	}

	return 1;
}

