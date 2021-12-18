#include <iostream>
#include "decoder.h"

namespace HuffEncode {
std::tuple<huffman_tree_node *, bit_vector, bool, uint8_t> parse_input(std::string &encoded_input) {
    // parse additional symbol
    bool has_additional_symbol = (unsigned char)encoded_input[0] % 2;
    // parse last byte free bits cnt
    uint8_t last_byte_free_bits = ((unsigned char)encoded_input[0] >> 1) % 2 + (((unsigned char)encoded_input[0] >> 2) % 2) * 2 + (((unsigned char)encoded_input[0] >> 3) % 2) * 4;
    // parse tree
    auto[decoded_tree, pos] = huffman_tree_node::decode_tree(encoded_input);
    // parse message
    bit_vector message;
    for (; pos < encoded_input.size() * 8; pos++) {
        bool current_bit = ((unsigned char) encoded_input[pos / 8] >> (pos % 8)) % 2 == 1;
        message.push_back(current_bit);
    }

    return {decoded_tree, message, has_additional_symbol, last_byte_free_bits};
}

std::string decode_message(bit_vector &message, huffman_tree_node *tree, bool has_additional_symbol, uint8_t last_byte_free_bits) {
    // TODO: some fancy iterator
    huffman_tree_node *cur = tree;
    std::vector<char> res;
    for (int i = 0; i < message.size() - last_byte_free_bits; i++) {
        bool current_bit = message[i];
        if (cur->children[current_bit] != nullptr) {
            cur = cur->children[current_bit].get();

            if (cur->children[0] == nullptr && cur->children[1] == nullptr) {
                res.push_back(cur->get_letter().first);
                res.push_back(cur->get_letter().second);
                cur = tree;
            }
        } else {
            throw std::runtime_error("never happens");
        }
    }

    std::string decoded(res.begin(), res.end());
    if (has_additional_symbol) {
        rollback_preprocess(decoded);
    }

    return decoded;
}

std::string decode(std::string &encoded_input) {
    auto[tree, message, has_additional_symbol, last_byte_free_bits] = parse_input(encoded_input);
    return decode_message(message, tree, has_additional_symbol, last_byte_free_bits);
}
}
