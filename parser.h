#ifndef PARSER_H
#define PARSER_H

#include "options.h"
#include <time.h>
#include <string>
#include <iostream>
#include <deque>
#include <sstream>

template <typename data_t>
bool read_data_point(std::istream& s, time_t& timestamp, data_t& value) {
	std::string line;
	if (!std::getline(s,line).good())
		return false;

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

template <typename data_t>
bool process_data(std::istream& is, std::ostream& os) {
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
					if (data_finished)
						return true;

					data.push_back(std::make_pair(stamp,value));
					if (!read_data_point(is,stamp,value)) {
						data_finished = true;
					}
				}
			} else {
				if (!read_data_point(is,stamp,value)) {
					std::cerr << "Could not read first value." << std::endl;
					return false;
				}

				data.push_back(std::make_pair(stamp,value));
			}
		}

		data_t sum = 0;
		// calcuate the next average
		for (const auto &i : data) {
			sum += i.second;
		}

		data_t value = sum/data.size();
		if (value < 1) {
			value = 0;
		}

		os << data.front().first + data.size()/2 << "\t" << sum/data.size() << std::endl;

		if (data_finished) {
			break;
		}

		int remove = average_frequency;
		while (remove > 0) {
			data.pop_front();
			--remove;
		}
	}

	return true;
}

#endif // PARSER_H

