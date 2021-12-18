#include <iostream>
#include <bits/stdc++.h>

#include "encoder.h"

using namespace std;

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cout << "4 args" << std::endl;
        return 1;
    }
    std::vector<uint8_t> data1(10000000);
    std::vector<uint8_t> data2(10000000);

    if (argv[1][0] == 'e') {
        std::ifstream f(argv[2], std::ios::in | std::ios::binary | std::ifstream::ate );
        if (!f) {
            printf("Error opening the input file.\n");
            return 1;
        }
        auto f_size = f.tellg();

        f.seekg(std::ios_base::beg);
        f.clear();


        f.read(reinterpret_cast<char *>(data1.data()), f_size);

        if (f.gcount() != f_size) {
            std::cout << "ggggggggggggggggggggggggggggggggggggggggggggg";
        }
        auto size_cur = AEncoder::arithmetic_encode(data1.data(), data1.data() + f_size, data2.data());
        std::ofstream outout1(argv[3], std::ios::out | std::ios::binary);
        outout1.write((char *) data2.data(), size_cur);
    } else {
        std::ifstream f(argv[2], std::ios::in | std::ios::binary | std::ifstream::ate );
        if (!f) {
            printf("Error opening the input file.\n");
            return 1;
        }
        auto f_size = f.tellg();

        f.seekg(std::ios_base::beg);
        f.clear();


        f.read(reinterpret_cast<char *>(data1.data()), f_size);

        if (f.gcount() != f_size) {
            std::cout << "ggggggggggggggggggggggggggggggggggggggggggggg";
        }
        auto size_cur = ADecoder::arithmetic_decode(data1.data(), data2.data());
        std::ofstream outout1(argv[3], std::ios::out | std::ios::binary);
        outout1.write((char *) data2.data(), size_cur);
    }
}
