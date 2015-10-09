#include "options.h"
#include "parser.h"

#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
	if (auto ret = parse_options(argc,argv) != 0) {
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

	if (!process_data<double>(input,output)) {
		return 1;
	}

	input.close();
	output.close();

	return 0;
}

