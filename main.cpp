#include <iostream>

#include "coding/coding.h"

using namespace std;

using namespace Compress;

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cout << "4 args" << std::endl;
        return 1;
    }

//    BestOfCoder<
//            Coder<BWT, MTF, RLE, ARITH<1>>,
//            Coder<BWT, MTF, RLE, ARITH<1>, ARITH<1>>,
//            Coder<BWT, MTF, RLE, ARITH_BIN>,
//            Coder<BWT, MTF, RLE, ARITH<2>>,
//            Coder<BWT, MTF, RLE, ARITH<3>>,
//            Coder<BWT, MTF, ARITH<1>>,
//            Coder<BWT, MTF, ARITH_BIN>,
//            Coder<BWT, MTF, ARITH<2>>,
//            Coder<BWT, MTF, ARITH<3>>,
//            Coder<RLE, BWT, MTF, ARITH<1>>,
//            Coder<RLE, BWT, MTF, ARITH<1>>,
//            Coder<RLE, BWT, MTF, ARITH<1>>,
//            Coder<ARITH<1>>,
//            Coder<ARITH<2>>,
//            Coder<ARITH<3>>,
//            Coder<ARITH<4>>,
//            HUFF,
//            Coder<BWT, MTF, RLE, HUFF>,
//            Coder<BWT, MTF, HUFF>
////            RANS
//    > coder;
    RANS_ADAPTIVE coder;
//    ARITH<1> coder;

    std::ifstream f(argv[2], std::ios::in | std::ios::binary | std::ifstream::ate);
    if (!f) {
        printf("Error opening the input file.\n");
        return 1;
    }
    auto f_size = f.tellg();

    f.seekg(std::ios_base::beg);
    f.clear();

    bytes data1(f_size);

    f.read(reinterpret_cast<char *>(data1.data()), f_size);

    const auto output = [&]() {
        if (argv[1][0] == 'e') {
            return coder.encode(data1);
        } else if (argv[1][0] == 'd') {
            return coder.decode(data1);
        } else {
            std::cerr << "e/d for type" << std::endl;
            return bytes{};
        }
    }();

    std::ofstream outout1(argv[3], std::ios::out | std::ios::binary);
    outout1.write((char *) output.data(), output.size());
}
