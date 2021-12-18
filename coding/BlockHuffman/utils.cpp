#include "utils.h"

#include <filesystem>
#include <fstream>
#include <vector>

std::string read_file_to_string(const std::string &file_name) {
    // open the file:
    std::streampos fileSize;
    std::ifstream file(file_name, std::ios::binary);

    // get its size:
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    std::vector<char> fileData(fileSize);
    file.read((char *) &fileData[0], fileSize);
    return {fileData.begin(), fileData.end()};
}

void write_to_file(std::string &content, const std::string &file_name) {
    std::ofstream out(file_name, std::ios::out | std::ios::binary);
    out << content;
    out.close();
}
