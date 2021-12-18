#include "huffman_tree.h"

#include <iostream>
#include <memory>
#include <queue>
#include <set>

struct huffman_build_node {
    huffman_build_node(int cnt, huffman_tree_node *node) : cnt(cnt), node(node) {}

    int cnt;
    huffman_tree_node *node;
};

huffman_tree_node *build_tree(const std::map<twogram, int> &dict) {
    auto cmp = [](huffman_build_node const &a, huffman_build_node const &b) {
        return a.cnt < b.cnt;
    };
    std::multiset<huffman_build_node, decltype(cmp)> build_que;

    for (auto &[letter, cnt]: dict) {
        build_que.insert({cnt, new huffman_tree_node_terminal(letter)});
    }

    while (build_que.size() > 1) {
        auto first = *build_que.begin();
        build_que.erase(build_que.begin());
        auto second = *build_que.begin();
        build_que.erase(build_que.begin());

        auto *new_node = new huffman_tree_node();
        new_node->children[0].reset(first.node);
        new_node->children[1].reset(second.node);

        build_que.insert({first.cnt + second.cnt, new_node});
    }

    return build_que.begin()->node;
}

std::string print_indent(int indent) {
    std::string res;
    for (int i = 0; i < indent; i++) {
        res += "  ";
    }

    return res;
}

std::string huffman_tree_node::print_with_indent(int indent) {
    std::string res;
    res += print_indent(indent);
    res += print_self();
    res += '\n';

    for (int child_index: {0, 1}) {
        if (children[child_index] != nullptr) {
            res += print_indent(indent);
            res += std::to_string(child_index);
            res += "\n";
            res += children[child_index]->print_with_indent(indent + 1);
        }
    }

    return res;
}

std::pair<bit_vector, std::vector<twogram>> huffman_tree_node::encode_tree() {
    std::queue<huffman_tree_node *> que;
    que.push(this);

    bit_vector res;
    std::vector<twogram> letters;
    while (!que.empty()) {
        huffman_tree_node *head = que.front();

        if (head->children[0] != nullptr && head->children[1] != nullptr) {
            res.push_back(0);
            que.push(head->children[0].get());
            que.push(head->children[1].get());
        } else {
            res.push_back(1);
            letters.push_back(head->get_letter());
        }

        que.pop();
    }

    return {res, letters};
}

std::map<twogram, std::string> huffman_tree_node::get_codes() {
    std::map<twogram, std::string> res;

    for (int child_index: {0, 1}) {
        if (children[child_index] != nullptr) {
            auto from_child = children[child_index]->get_codes();
            for (auto &[letter, code]: from_child) {
                code = std::to_string(child_index) + code;
            }
            res.insert(from_child.begin(), from_child.end());
        }
    }

    return res;
}

bool get_bit(uint8_t pos, std::string &s) {
    return (s[pos / 8] << (pos % 8)) % 2 == 1;
}

void set_bit(uint8_t pos, bool bit, std::string &s) {
    s[pos / 8] |= ((1 << pos) * bit);
}

bit_vector huffman_tree_node::encode_tree_with_letters() {
    auto[tree_bit_vec, letters] = encode_tree();

    for (auto &[f, s]: letters) {
        for (unsigned char ch: {f, s}) {
            for (int i = 0; i < 8; i++) {
                tree_bit_vec.push_back(ch % 2);
                ch /= 2;
            }
        }
    }

    return tree_bit_vec;
}

std::pair<huffman_tree_node *, size_t>
huffman_tree_node::decode_tree(std::string &encoded, int start_pos, bool print_bits) {
    size_t pos = start_pos;

    auto *root = new huffman_tree_node();
    std::queue<huffman_tree_node *> parents_que;
    std::queue<huffman_tree_node *> nodes_que;
    nodes_que.push(root);
    parents_que.push(nullptr);

    std::vector<huffman_tree_node_terminal *> terminals_to_init;

    // parse tree
    if (print_bits) {
        std::cout << "Parsing tree:\n";
    }

    while (!nodes_que.empty()) {
        bool current_bit = ((unsigned char) encoded[pos / 8] >> (pos % 8)) % 2 == 1;
        if (print_bits) {
            std::cout << current_bit;
        }

        huffman_tree_node *parent = parents_que.front();
        huffman_tree_node *head = nodes_que.front();

        if (!current_bit) {
            auto left = new huffman_tree_node();
            auto right = new huffman_tree_node();

            head->children[0].reset(left);
            head->children[1].reset(right);

            nodes_que.push(left);
            parents_que.push(head);
            nodes_que.push(right);
            parents_que.push(head);
        } else {
            if (parent->children[0].get() == head) {
                auto *late_init_terminal = new huffman_tree_node_terminal();
                parent->children[0].reset(late_init_terminal);
                terminals_to_init.push_back(late_init_terminal);
            } else if (parent->children[1].get() == head) {
                auto *late_init_terminal = new huffman_tree_node_terminal();
                parent->children[1].reset(late_init_terminal);
                terminals_to_init.push_back(late_init_terminal);
            } else {
                throw std::runtime_error("something is fucked up!");
            }
        }

        parents_que.pop();
        nodes_que.pop();

        pos++;
    }

    if (print_bits) {
        std::cout << "\nParsing letters:\n";
    }

    // parse letters
    for (auto current_terminal: terminals_to_init) {
        if (print_bits) {
            std::cout << "bits: ";
        }

        twogram letter;
        for (int j = 0; j < 2; j++) {
            unsigned char letter_ch = 0;
            for (int8_t i = 0; i < 8; i++) {
                bool current_bit = ((unsigned char) encoded[pos / 8] >> (pos % 8)) % 2 == 1;

                if (print_bits) {
                    std::cout << current_bit;
                }

                letter_ch += (1 << i) * current_bit;
                pos++;
            }

            if (j == 0) {
                letter.first = (char) letter_ch;
            } else {
                letter.second = (char) letter_ch;
            }
        }

        if (print_bits) {
            std::cout << " : " << letter.first << letter.second << '\n';
        }

        current_terminal->letter = letter;
    }

    if (print_bits) {
        std::cout << '\n';
    }

    return {root, pos};
}

