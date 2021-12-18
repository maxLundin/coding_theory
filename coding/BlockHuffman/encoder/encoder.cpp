#include "encoder.h"
#include "BlockHuffman/utils.h"
#include "BlockHuffman/entropy.h"

#include <iomanip>
#include <sstream>

namespace HuffEncode
{

    std::map<twogram, int> parse_dictionary(std::string &data)
    {
        std::map<twogram, int> twogram2cnt;
        char prev = data[0];
        bool has_prev = false;
        for (char cur : data)
        {
            if (!has_prev)
            {
                has_prev = true;
                continue;
            }

            twogram2cnt[{prev, cur}]++;

            prev = cur;
        }

        return twogram2cnt;
    }

    bit_vector encode_message(std::string &message, huffman_tree_node *tree, bool print_dictionary)
    {
        auto codes = tree->get_codes();

        if (print_dictionary)
        {
            std::cout << "Dictionary:\n";
            for (auto &[letter, code] : codes)
            {
                std::cout << letter.first << letter.second << " : " << code << '\n';
            }
        }

        bit_vector res;

        for (int i = 0; i < message.size(); i += 2)
        {
            twogram letter = {message[i], message[i + 1]};
            std::string code = codes[letter];

            res.push_code(code);
        }

        return res;
    }

    std::string encode(std::string &message)
    {
        bit_vector res;

        bool has_additional_symbol = false;
        if (message.size() % 2 == 1)
        {
            preprocess_message(message);
            has_additional_symbol = true;
        }

        res.push(has_additional_symbol);

        auto dict = parse_dictionary(message);
        auto tree = build_tree(dict);

        bit_vector tree_bit_vec = tree->encode_tree_with_letters();
        bit_vector message_bit_vec = encode_message(message, tree);

        uint8_t last_byte_free_bits_cnt = (8 - (4 + tree_bit_vec.size() + message_bit_vec.size()) % 8) % 8;

        res.push(last_byte_free_bits_cnt % 2);
        res.push((last_byte_free_bits_cnt >> 1) % 2);
        res.push((last_byte_free_bits_cnt >> 2) % 2);

        res.push(tree_bit_vec);
        res.push(message_bit_vec);

        if (has_additional_symbol)
        {
            rollback_preprocess(message);
        }

        return res.to_string();
    }

}
