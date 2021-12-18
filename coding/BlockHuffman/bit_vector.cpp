#include "bit_vector.h"

void bit_vector::push(bool bit) {
    if (pos_in_char > 7) {
        content.push_back(0);
        pos_in_char = 0;
        power_of_two = 1;
    }

    content[content.size() - 1] = (char) ((unsigned char) content[content.size() - 1] + power_of_two * (bit ? 1 : 0));
    power_of_two <<= 1;
    pos_in_char++;
}

void bit_vector::push_code(std::string &str) {
    for (char ch: str) {
        push(ch == '1');
    }
}

bit_vector bit_vector::from_string(std::string &s, size_t len) {
    if (len == -1) {
        len = 8 * s.size();
    }

    bit_vector res;
    for (char ch: s) {
        for (int i = 0; i < std::min((size_t) 8, len); i++) {
            res.push(((unsigned char) ch >> i) % 2 == 1);
        }
        len -= 8;
    }

    return res;
}

std::string bit_vector::to_string() {
    return {content.begin(), content.end()};
}

bool bit_vector::operator[](int i) {
    return ((unsigned char) content[i / 8] >> (i % 8)) % 2;
}

size_t bit_vector::size() {
    return (content.size() - 1) * 8 + pos_in_char;
}

void bit_vector::push_back(bool bit) {
    push(bit);
}

std::string bit_vector::to_bit_string() {
    std::vector<char> res(size());
    for (int i = 0; i < size(); i++) {
        res[i] = (*this)[i] ? '1' : '0';
    }

    return {res.begin(), res.end()};
}

void bit_vector::push(bit_vector &other) {
    for (int i = 0; i < other.size(); i++) {
        push(other[i]);
    }
}

