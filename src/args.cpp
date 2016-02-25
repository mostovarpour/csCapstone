#include "args.h"

namespace po = boost::program_options;
using namespace std;

// Declare the supported options.
void parse_command_line(int ac, char** av){
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("compression", po::value<int>(), "set compression level")
		;

	//po::variables_map vm;
	//po::store(po::parse_command_line(ac, av, desc), vm);
	//po::notify(vm);

	//if (vm.count("help")) {
	//	cout << desc << "\n";
	//	return 1;
	//}

	//if (vm.count("compression")) {
	//	cout << "Compression level was set to "
	//		<< vm["compression"].as<int>() << ".\n";
	//}
	//else {
	//	cout << "Compression level was not set.\n";
	//}

	//int opt;
	//desc("Allowed options");
	//desc.add_options()
	//	("help", "produce help message")
	//	("optimization", po::value<int>(&opt)->default_value(10),
	//		"optimization level")
	//	("include-path,I", po::value< vector<string> >(),
	//		"include path")
	//	("input-file", po::value< vector<string> >(), "input file")
	//	;

	//po::positional_options_description p;
	//p.add("input-file", -1);

	//vm;
	//po::store(po::command_line_parser(ac, av).
	//	options(desc).positional(p).run(), vm);
	//po::notify(vm);


	//if (vm.count("include-path"))
	//{
	//	cout << "Include paths are: "
	//		<< vm["include-path"].as< vector<string> >() << "\n";
	//}

	//if (vm.count("input-file"))
	//{
	//	cout << "Input files are: "
	//		<< vm["input-file"].as< vector<string> >() << "\n";
	//}

	//cout << "Optimization level is " << opt << "\n";
}
