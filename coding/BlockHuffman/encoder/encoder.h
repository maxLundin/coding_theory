#ifndef TEORCOD_ENCODER_H
#define TEORCOD_ENCODER_H

#include <iostream>
#include "BlockHuffman/bit_vector.h"
#include "BlockHuffman/huffman_tree.h"
#include "BlockHuffman/preprocess.h"

namespace HuffEncode
{

    std::map<twogram, int> parse_dictionary(std::string &data);

    bit_vector encode_message(std::string &message, huffman_tree_node *tree, bool print_dictionary = false);

    std::string encode(std::string &message);

}

#endif //TEORCOD_ENCODER_H
