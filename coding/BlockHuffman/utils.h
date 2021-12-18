#ifndef TEORCOD_UTILS_H
#define TEORCOD_UTILS_H

#include <string>

std::string read_file_to_string(const std::string &file_name);

void write_to_file(std::string &content, const std::string &file_name);

#endif //TEORCOD_UTILS_H
