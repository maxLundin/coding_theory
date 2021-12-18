#ifndef TEORCOD_DECODER_H
#define TEORCOD_DECODER_H

#include "BlockHuffman/huffman_tree.h"
#include "BlockHuffman/preprocess.h"

namespace HuffEncode
{
    std::string decode(std::string &encoded_input);
}
#endif //TEORCOD_DECODER_H
