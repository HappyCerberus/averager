#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

int parse_options(int argc, char *argv[]);

extern int time_column;
extern int data_column;
extern size_t average_frequency;
extern size_t average_span;
extern std::string input_filename;
extern std::string output_filename;

#endif // OPTIONS_H
