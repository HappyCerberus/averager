#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

int time_column;
int data_column;
int average_frequency;
int average_span;
std::string input_filename;
std::string output_filename;

int parse_options(int argc, char *argv[]) {

	po::options_description desc("Available options");
	desc.add_options()
			("help", "produce this help message")
			("time-column", po::value<int>()->default_value(1), "column that contains time stamps")
			("data-column", po::value<int>()->default_value(2), "column that contains data")
			("average-frequency", po::value<int>()->default_value(5*30), "how often is average generated")
			("average-span", po::value<int>()->default_value(5*60), "how big time period is the average generated")
			("input-file,i", po::value<std::string>(), "input file")
			("output-file,o", po::value<std::string>(), "output file");

	po::variables_map vm;
	po::store(po::parse_command_line(argc,argv,desc), vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 1;
	}

	if (vm.count("input-file")) {
		input_filename = vm["input-file"].as<std::string>();
	} else {
		std::cout << "Input file not specified." << std::endl;
		return 1;
	}

	if (vm.count("output-file")) {
		output_filename = vm["output-file"].as<std::string>();
	} else {
		std::cout << "Output file not specified." << std::endl;
		return 1;
	}

	time_column = vm["time-column"].as<int>();
	data_column = vm["data-column"].as<int>();
	average_frequency = vm["average-frequency"].as<int>();
	average_span = vm["average-span"].as<int>();

	if (average_frequency >= average_span) {
		std::cerr << "Frequence of computing averages needs to be strictly lower than average span." << std::endl;
		return 1;
	}

	return 0;
}

