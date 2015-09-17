#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <boost/program_options.hpp>

// program options
namespace {
	int time_column;
	int data_column;
	int average_frequency;
	int average_span;
	std::string input_filename;
	std::string output_filename;
}

namespace po = boost::program_options;

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

typedef double data_t;

bool read_data_point(std::istream& s, time_t& timestamp, data_t& value) {
	std::string line;
	if (!std::getline(s,line).good()) {
		return false;
	}

	std::stringstream parser(line);
	for (int i = 1; i <= std::max(time_column,data_column); ++i) {
		if (i == time_column) {
			parser >> timestamp;
		} else if (i == data_column) {
			parser >> value;
		} else {
			std::string junk;
			parser >> junk;
		}
	}

	return true;
}

int main(int argc, char *argv[])
{
	int ret;
	if ((ret = parse_options(argc,argv)) != 0) {
		return ret;
	}

	std::fstream input;
	input.open(input_filename.c_str(),std::ios_base::in);
	if (!input.is_open()) {
		std::cerr << "Could not open input filename." << std::endl;
		return 1;
	}

	std::fstream output;
	output.open(output_filename.c_str(),std::ios_base::out);
	if (!output.is_open()) {
		std::cerr << "Could not open output filename." << std::endl;
		return 1;
	}

	time_t stamp;
	data_t value;

	std::deque< std::pair<time_t,data_t> > data;
	bool data_finished = false;

	while (true) {

		// fill in the data by reading values from the file and interpolating the missing ones
		while (data.size() < average_span) {
			if (data.size() != 0) {
				if (stamp - data.back().first > 1) {
					// interpolate data between the last item and the next read item
					time_t distance = stamp - data.back().first;
					double step = (value - data.back().second) / distance;
					while (data.size() < average_span && distance > 1) {
						data.push_back(std::make_pair(data.back().first+1,data.back().second+step));
						--distance;
					}
				} else {
					if (data_finished) { // no more data, finish
						input.close();
						output.close();
						break;
					}

					data.push_back(std::make_pair(stamp,value));
					if (!read_data_point(input,stamp,value)) {
						data_finished = true;
					}
				}
			} else {
				if (!read_data_point(input,stamp,value)) {
					std::cerr << "Could not read first value." << std::endl;
					return 1;
				}

				data.push_back(std::make_pair(stamp,value));
			}
		}

		data_t sum = 0;
		// calcuate the next average
		for (const auto &i : data) {
			sum += i.second;
		}

		output << data.front().first + data.size()/2 << "\t" << sum/data.size() << std::endl;

		if (data_finished) {
			break;
		}

		int remove = average_frequency;
		while (remove > 0) {
			data.pop_front();
			--remove;
		}


	}

	input.close();
	output.close();


	return 0;
}

